#ifndef PLATFORM_SOCKET_H
#define PLATFORM_SOCKET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "platform_detect.h"

#if PLAT_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

typedef struct
{
#if PLAT_WINDOWS
	SOCKET sock;
#else
	int fd;
#endif
	bool valid;
} PlatSocket_t;

int plat_socket_init(void);
void plat_socket_shutdown(void);

int plat_socket_listen(PlatSocket_t* s, uint16_t port, int backlog);
int plat_socket_accept(PlatSocket_t* server, PlatSocket_t* out_client);

int plat_socket_recv(PlatSocket_t* s, void* buf, size_t len, size_t* out_n);
int plat_socket_send_all(PlatSocket_t* s, const void* buf, size_t len);

void plat_socket_close(PlatSocket_t* s);

int plat_socket_set_nonblock(PlatSocket_t* s, bool enable);
int plat_socket_set_nodelay(PlatSocket_t* s, bool enable);

#endif /* PLATFORM_SOCKET_H */
