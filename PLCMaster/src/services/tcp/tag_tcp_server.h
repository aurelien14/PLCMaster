#ifndef TAG_TCP_SERVER_H
#define TAG_TCP_SERVER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/ipc/tag_rpc.h"
#include "core/platform/platform_atomic.h"
#include "core/platform/platform_socket.h"
#include "core/platform/platform_thread.h"

typedef struct
{
	plat_thread_t thread;
	TagRpcChannel_t* ch;
	uint16_t port;
	plat_atomic_bool_t stop_flag;
	PlatSocket_t server_sock;
	PlatSocket_t client_sock;
} TagTcpServer_t;

int tag_tcp_server_start(TagTcpServer_t* s, TagRpcChannel_t* ch, uint16_t port);
void tag_tcp_server_stop(TagTcpServer_t* s);

#endif /* TAG_TCP_SERVER_H */
