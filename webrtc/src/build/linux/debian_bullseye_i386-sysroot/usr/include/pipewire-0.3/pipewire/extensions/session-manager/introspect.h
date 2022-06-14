/* PipeWire
 *
 * Copyright © 2019 Collabora Ltd.
 *   @author George Kiagiadakis <george.kiagiadakis@collabora.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef PIPEWIRE_EXT_SESSION_MANAGER_INTROSPECT_H
#define PIPEWIRE_EXT_SESSION_MANAGER_INTROSPECT_H

#include <spa/utils/defs.h>
#include <spa/utils/dict.h>
#include <spa/param/param.h>

#ifdef __cplusplus
extern "C" {
#endif

enum pw_endpoint_link_state {
	PW_ENDPOINT_LINK_STATE_ERROR = -1,
	PW_ENDPOINT_LINK_STATE_PREPARING,
	PW_ENDPOINT_LINK_STATE_INACTIVE,
	PW_ENDPOINT_LINK_STATE_ACTIVE,
};

struct pw_session_info {
#define PW_VERSION_SESSION_INFO			0
	uint32_t version;			/**< version of this structure */
	uint32_t id;				/**< the session id (global) */
#define PW_SESSION_CHANGE_MASK_PROPS		(1 << 0)
#define PW_SESSION_CHANGE_MASK_PARAMS		(1 << 1)
#define PW_SESSION_CHANGE_MASK_ALL		((1 << 2)-1)
	uint32_t change_mask;			/**< bitfield of changed fields since last call */
	struct spa_dict *props;			/**< extra properties */
	struct spa_param_info *params;		/**< parameters */
	uint32_t n_params;			/**< number of items in \a params */
};

struct pw_endpoint_info {
#define PW_VERSION_ENDPOINT_INFO		0
	uint32_t version;			/**< version of this structure */
	uint32_t id;				/**< the endpoint id (global) */
	char *name;				/**< name of the endpoint */
	char *media_class;			/**< media class of the endpoint */
	enum pw_direction direction;		/**< direction of the endpoint */
#define PW_ENDPOINT_FLAG_PROVIDES_SESSION	(1 << 0)
	uint32_t flags;				/**< additional flags */
#define PW_ENDPOINT_CHANGE_MASK_STREAMS		(1 << 0)
#define PW_ENDPOINT_CHANGE_MASK_SESSION		(1 << 1)
#define PW_ENDPOINT_CHANGE_MASK_PROPS		(1 << 2)
#define PW_ENDPOINT_CHANGE_MASK_PARAMS		(1 << 3)
#define PW_ENDPOINT_CHANGE_MASK_ALL		((1 << 4)-1)
	uint32_t change_mask;			/**< bitfield of changed fields since last call */
	uint32_t n_streams;			/**< number of streams available */
	uint32_t session_id;			/**< the id of the controlling session */
	struct spa_dict *props;			/**< extra properties */
	struct spa_param_info *params;		/**< parameters */
	uint32_t n_params;			/**< number of items in \a params */
};

struct pw_endpoint_stream_info {
#define PW_VERSION_ENDPOINT_STREAM_INFO		0
	uint32_t version;			/**< version of this structure */
	uint32_t id;				/**< the stream id (local or global) */
	uint32_t endpoint_id;			/**< the endpoint id (global) */
	char *name;				/**< name of the stream */
#define PW_ENDPOINT_STREAM_CHANGE_MASK_LINK_PARAMS	(1 << 0)
#define PW_ENDPOINT_STREAM_CHANGE_MASK_PROPS		(1 << 1)
#define PW_ENDPOINT_STREAM_CHANGE_MASK_PARAMS		(1 << 2)
#define PW_ENDPOINT_STREAM_CHANGE_MASK_ALL		((1 << 3)-1)
	uint32_t change_mask;			/**< bitfield of changed fields since last call */
	struct spa_pod *link_params;		/**< information for linking this stream */
	struct spa_dict *props;			/**< extra properties */
	struct spa_param_info *params;		/**< parameters */
	uint32_t n_params;			/**< number of items in \a params */
};

struct pw_endpoint_link_info {
#define PW_VERSION_ENDPOINT_LINK_INFO		0
	uint32_t version;			/**< version of this structure */
	uint32_t id;				/**< the link id (global) */
	uint32_t session_id;			/**< the session id (global) */
	uint32_t output_endpoint_id;		/**< the output endpoint id (global) */
	uint32_t output_stream_id;		/**< the output stream id (local or global) */
	uint32_t input_endpoint_id;		/**< the input endpoint id (global) */
	uint32_t input_stream_id;		/**< the input stream id (local or global) */
#define PW_ENDPOINT_LINK_CHANGE_MASK_STATE	(1 << 0)
#define PW_ENDPOINT_LINK_CHANGE_MASK_PROPS	(1 << 1)
#define PW_ENDPOINT_LINK_CHANGE_MASK_PARAMS	(1 << 2)
#define PW_ENDPOINT_LINK_CHANGE_MASK_ALL	((1 << 3)-1)
	uint32_t change_mask;			/**< bitfield of changed fields since last call */
	enum pw_endpoint_link_state state;	/**< the state of the link */
	char *error;				/**< error string if state == ERROR */
	struct spa_dict *props;			/**< extra properties */
	struct spa_param_info *params;		/**< parameters */
	uint32_t n_params;			/**< number of items in \a params */
};

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PIPEWIRE_EXT_SESSION_MANAGER_INTROSPECT_H */
