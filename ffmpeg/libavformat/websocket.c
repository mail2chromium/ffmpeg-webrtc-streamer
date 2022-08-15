/**
 * @file
 * WebSocket protocol
 */

#define _DEFAULT_SOURCE
#define _BSD_SOURCE     /* Needed for using struct ip_mreq with recent glibc */

#include "avformat.h"
#include "avio_internal.h"
#include "libavutil/avassert.h"
#include "libavutil/parseutils.h"
#include "libavutil/fifo.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/avstring.h"
#include "libavutil/opt.h"
#include "libavutil/log.h"
#include "libavutil/time.h"
#include "internal.h"
#include "network.h"
#include "os_support.h"
#include "url.h"
#include "ip.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if HAVE_PTHREAD_CANCEL
#include <pthread.h>
#endif

#ifndef IPV6_ADD_MEMBERSHIP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif

#define WS_TX_BUF_SIZE 32768
#define WS_MAX_PKT_SIZE 65536
#define WS_HEADER_SIZE 8

typedef struct WSContext {
    const AVClass *class;
    int udp_fd;
    int ttl;
    int udplite_coverage;
    int buffer_size;
    int pkt_size;
    int is_multicast;
    int is_broadcast;
    int local_port;
    int reuse_socket;
    int overrun_nonfatal;
    struct sockaddr_storage dest_addr;
    int dest_addr_len;
    int is_connected;

    /* Circular Buffer variables for use in UDP receive code */
    int circular_buffer_size;
    AVFifoBuffer *fifo;
    int circular_buffer_error;
    int64_t bitrate; /* number of bits to send per second */
    int64_t burst_bits;
    int close_req;
#if HAVE_PTHREAD_CANCEL
    pthread_t circular_buffer_thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int thread_started;
#endif
    uint8_t tmp[WS_MAX_PKT_SIZE+4];
    int remaining_in_dg;
    char *localaddr;
    int timeout;
    struct sockaddr_storage local_addr_storage;
    char *sources;
    char *block;
    IPSourceFilters filters;
} WSContext;

#define OFFSET(x) offsetof(WSContext, x)
#define D AV_OPT_FLAG_DECODING_PARAM
#define E AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "buffer_size",    "System data size (in bytes)",                     OFFSET(buffer_size),    AV_OPT_TYPE_INT,    { .i64 = -1 },    -1, INT_MAX, .flags = D|E },
    { "bitrate",        "Bits to send per second",                         OFFSET(bitrate),        AV_OPT_TYPE_INT64,  { .i64 = 0  },     0, INT64_MAX, .flags = E },
    { "burst_bits",     "Max length of bursts in bits (when using bitrate)", OFFSET(burst_bits),   AV_OPT_TYPE_INT64,  { .i64 = 0  },     0, INT64_MAX, .flags = E },
    { "localport",      "Local port",                                      OFFSET(local_port),     AV_OPT_TYPE_INT,    { .i64 = -1 },    -1, INT_MAX, D|E },
    { "local_port",     "Local port",                                      OFFSET(local_port),     AV_OPT_TYPE_INT,    { .i64 = -1 },    -1, INT_MAX, .flags = D|E },
    { "localaddr",      "Local address",                                   OFFSET(localaddr),      AV_OPT_TYPE_STRING, { .str = NULL },               .flags = D|E },
    { "udplite_coverage", "choose WSLite head size which should be validated by checksum", OFFSET(udplite_coverage), AV_OPT_TYPE_INT, {.i64 = 0}, 0, INT_MAX, D|E },
    { "pkt_size",       "Maximum WS packet size",                         OFFSET(pkt_size),       AV_OPT_TYPE_INT,    { .i64 = 1472 },  -1, INT_MAX, .flags = D|E },
    { "reuse",          "explicitly allow reusing WS sockets",            OFFSET(reuse_socket),   AV_OPT_TYPE_BOOL,   { .i64 = -1 },    -1, 1,       D|E },
    { "reuse_socket",   "explicitly allow reusing WS sockets",            OFFSET(reuse_socket),   AV_OPT_TYPE_BOOL,   { .i64 = -1 },    -1, 1,       .flags = D|E },
    { "broadcast", "explicitly allow or disallow broadcast destination",   OFFSET(is_broadcast),   AV_OPT_TYPE_BOOL,   { .i64 = 0  },     0, 1,       E },
    { "ttl",            "Time to live (multicast only)",                   OFFSET(ttl),            AV_OPT_TYPE_INT,    { .i64 = 16 },     0, INT_MAX, E },
    { "connect",        "set if connect() should be called on socket",     OFFSET(is_connected),   AV_OPT_TYPE_BOOL,   { .i64 =  0 },     0, 1,       .flags = D|E },
    { "fifo_size",      "set the WS receiving circular buffer size, expressed as a number of packets with size of 188 bytes", OFFSET(circular_buffer_size), AV_OPT_TYPE_INT, {.i64 = 7*4096}, 0, INT_MAX, D },
    { "overrun_nonfatal", "survive in case of WS receiving circular buffer overrun", OFFSET(overrun_nonfatal), AV_OPT_TYPE_BOOL, {.i64 = 0}, 0, 1,    D },
    { "timeout",        "set raise error timeout (only in read mode)",     OFFSET(timeout),        AV_OPT_TYPE_INT,    { .i64 = 0 },      0, INT_MAX, D },
    { "sources",        "Source list",                                     OFFSET(sources),        AV_OPT_TYPE_STRING, { .str = NULL },               .flags = D|E },
    { "block",          "Block list",                                      OFFSET(block),          AV_OPT_TYPE_STRING, { .str = NULL },               .flags = D|E },
    { NULL }
};

static const AVClass ws_class = {
    .class_name = "ws",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
};

typedef enum
{
    GET_DELIM_OK=0,
    GET_DELIM_NOT_FOUND_DELIM,
    GET_DELIM_PREFIX_TOO_LONG,
    GET_ID_PREFIX_TOO_LONG
} GET_DELIM_RESULT;

/**
 * Copy prefix of string pointed by pbuf before first occurance of delim
 * to dst. Increment pbuf so that it points to part of string right after
 * occurance of delim. Return 0 if operation succeeds, error code (to specify)
 * otherwise.
 * @param dst_size size of buffer dst
 */

static GET_DELIM_RESULT split_(const char** pbuf, const char* delim, char* dst, char* id, int dst_size, int id_size)
{
    const char* sub = strstr(*pbuf,delim);
    if(sub==NULL)
        return GET_DELIM_NOT_FOUND_DELIM;
    else
    {
        int prefix_len = sub-(*pbuf);
        if(prefix_len+1>dst_size)
            return GET_DELIM_PREFIX_TOO_LONG;
        // strncat requires dst to be null-terminated
        *dst=0;
        strncat(dst,*pbuf,prefix_len);
        *pbuf+=prefix_len+strlen(delim);

        *id=0;
        int id_length = strlen(*pbuf)-(prefix_len+strlen(delim));
        if(id_length+1>id_size)
            return GET_ID_PREFIX_TOO_LONG;

        strncat(id,*pbuf,id_length);
        *pbuf+=id_length;

        return GET_DELIM_OK;
    }
}

/* put it in WS context */
/* return non zero if error */
static int ws_open(URLContext *h, const char *uri, int flags)
{
    av_log(h, AV_LOG_INFO, "inside 'ws_open' function callback uri=%s\n", uri);
    return 0;
}

static int ws_write(URLContext *h, const uint8_t *buf, int size)
{
    av_log(h, AV_LOG_INFO, "inside 'ws_write' function callback %d bytes: %2x %2x %2x %2x\n", size, buf[0], buf[1], buf[2], buf[3]);
    return size;
}

static int ws_read(URLContext *h, uint8_t *buf, int size)
{
    av_log(h, AV_LOG_ERROR, "inside 'ws_read' function callback\n");
    return 0;
}

const URLProtocol ff_ws_protocol = {
    .name                = "ws",
    .url_open            = ws_open,
    .url_read            = ws_read,
    .url_write           = ws_write,
    .url_close           = NULL,
    .url_get_file_handle = NULL,
    .priv_data_size      = sizeof(WSContext),
    .priv_data_class     = &ws_class,
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};