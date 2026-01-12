#include "platform_socket.h"

#include <string.h>

#if PLAT_WINDOWS
#include <windows.h>
#else
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#endif

static void plat_socket_invalidate(PlatSocket_t* s)
{
	if (s == NULL)
	{
		return;
	}
#if PLAT_WINDOWS
	s->sock = INVALID_SOCKET;
#else
	s->fd = -1;
#endif
	s->valid = false;
}

int plat_socket_init(void)
{
#if PLAT_WINDOWS
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}
#endif
	return 0;
}

void plat_socket_shutdown(void)
{
#if PLAT_WINDOWS
	WSACleanup();
#endif
}

int plat_socket_listen(PlatSocket_t* s, uint16_t port, int backlog)
{
	int rc = -1;
	int opt = 1;

	if (s == NULL)
	{
		return -1;
	}

	plat_socket_invalidate(s);

#if PLAT_WINDOWS
	s->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s->sock == INVALID_SOCKET)
	{
		return -1;
	}
#else
	s->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (s->fd < 0)
	{
		return -1;
	}
#endif

#if PLAT_WINDOWS
	rc = setsockopt(s->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
	rc = setsockopt(s->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
	if (rc != 0)
	{
		plat_socket_close(s);
		return -1;
	}

	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

#if PLAT_WINDOWS
		rc = bind(s->sock, (struct sockaddr *)&addr, sizeof(addr));
#else
		rc = bind(s->fd, (struct sockaddr *)&addr, sizeof(addr));
#endif
		if (rc != 0)
		{
			plat_socket_close(s);
			return -1;
		}
	}

#if PLAT_WINDOWS
	rc = listen(s->sock, backlog);
#else
	rc = listen(s->fd, backlog);
#endif
	if (rc != 0)
	{
		plat_socket_close(s);
		return -1;
	}

	s->valid = true;
	return 0;
}

int plat_socket_accept(PlatSocket_t* server, PlatSocket_t* out_client)
{
#if PLAT_WINDOWS
	SOCKET client;
#else
	int client;
#endif

	if (server == NULL || out_client == NULL || !server->valid)
	{
		return -1;
	}

	plat_socket_invalidate(out_client);

#if PLAT_WINDOWS
	client = accept(server->sock, NULL, NULL);
	if (client == INVALID_SOCKET)
	{
		return -1;
	}
	out_client->sock = client;
#else
	client = accept(server->fd, NULL, NULL);
	if (client < 0)
	{
		return -1;
	}
	out_client->fd = client;
#endif

	out_client->valid = true;
	return 0;
}

int plat_socket_recv(PlatSocket_t* s, void* buf, size_t len, size_t* out_n)
{
	int rc;

	if (out_n != NULL)
	{
		*out_n = 0U;
	}

	if (s == NULL || !s->valid || buf == NULL || len == 0U)
	{
		return -1;
	}

#if PLAT_WINDOWS
	for (;;)
	{
		rc = recv(s->sock, (char *)buf, (int)len, 0);
		if (rc == SOCKET_ERROR && WSAGetLastError() == WSAEINTR)
		{
			continue;
		}
		break;
	}
	if (rc == 0)
	{
		return 0;
	}
	if (rc == SOCKET_ERROR)
	{
		return -1;
	}
#else
	for (;;)
	{
		rc = (int)recv(s->fd, buf, len, 0);
		if (rc < 0 && errno == EINTR)
		{
			continue;
		}
		break;
	}
	if (rc == 0)
	{
		return 0;
	}
	if (rc < 0)
	{
		return -1;
	}
#endif

	if (out_n != NULL)
	{
		*out_n = (size_t)rc;
	}
	return 1;
}

int plat_socket_send_all(PlatSocket_t* s, const void* buf, size_t len)
{
	const uint8_t* p;
	size_t remaining;

	if (s == NULL || !s->valid || buf == NULL)
	{
		return -1;
	}

	p = (const uint8_t *)buf;
	remaining = len;

	while (remaining > 0U)
	{
		int rc;
		size_t chunk = remaining > (size_t)INT32_MAX ? (size_t)INT32_MAX : remaining;
#if PLAT_WINDOWS
		for (;;)
		{
			rc = send(s->sock, (const char *)p, (int)chunk, 0);
			if (rc == SOCKET_ERROR && WSAGetLastError() == WSAEINTR)
			{
				continue;
			}
			break;
		}
		if (rc == SOCKET_ERROR)
		{
			return -1;
		}
#else
		for (;;)
		{
			rc = (int)send(s->fd, p, chunk, 0);
			if (rc < 0 && errno == EINTR)
			{
				continue;
			}
			break;
		}
		if (rc < 0)
		{
			return -1;
		}
#endif
		p += (size_t)rc;
		remaining -= (size_t)rc;
	}

	return 0;
}

void plat_socket_close(PlatSocket_t* s)
{
	if (s == NULL || !s->valid)
	{
		return;
	}

#if PLAT_WINDOWS
	closesocket(s->sock);
#else
	close(s->fd);
#endif
	plat_socket_invalidate(s);
}

int plat_socket_set_nonblock(PlatSocket_t* s, bool enable)
{
	if (s == NULL || !s->valid)
	{
		return -1;
	}

#if PLAT_WINDOWS
	{
		u_long mode = enable ? 1UL : 0UL;
		return ioctlsocket(s->sock, FIONBIO, &mode) == 0 ? 0 : -1;
	}
#else
	{
		int flags = fcntl(s->fd, F_GETFL, 0);
		if (flags < 0)
		{
			return -1;
		}
		if (enable)
		{
			flags |= O_NONBLOCK;
		}
		else
		{
			flags &= ~O_NONBLOCK;
		}
		return fcntl(s->fd, F_SETFL, flags) == 0 ? 0 : -1;
	}
#endif
}

int plat_socket_set_nodelay(PlatSocket_t* s, bool enable)
{
	int opt;

	if (s == NULL || !s->valid)
	{
		return -1;
	}

	opt = enable ? 1 : 0;
#if PLAT_WINDOWS
	return setsockopt(s->sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(opt)) == 0 ? 0 : -1;
#else
	return setsockopt(s->fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) == 0 ? 0 : -1;
#endif
}
