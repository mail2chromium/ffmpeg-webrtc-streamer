#ifndef DATAGRAMSOCKET_H_INCLUDED
#define DATAGRAMSOCKET_H_INCLUDED



#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#endif


//Simple socket class for datagrams.  Platform independent between
//unix and Windows.
class DatagramSocket
{
private:
#ifdef WIN32
    WSAData wsaData;
    SOCKET sock;
#else
    int sock;
#endif
    long retval;
    sockaddr_in outaddr;
    char ip[30];
    char received[30];


public:
    DatagramSocket(int port, char* address, bool Datagram, bool reusesock);
    ~DatagramSocket();

    long receive(uint8_t* msg, int msgsize);
    char* received_from();
    long send(const char* msg, int msgsize);
    long sendTo(const char* msg, int msgsize, const char* name);
    int getAddress(const char * name, char * addr);
    const char* getAddress(const char * name);

};



#endif // DATAGRAMSOCKET_H_INCLUDED
