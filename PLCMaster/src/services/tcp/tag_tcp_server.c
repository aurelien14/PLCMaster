#include "tag_tcp_server.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define TAG_TCP_MAX_LINE 512

static void *tag_tcp_server_thread(void *arg);

static void socket_invalidate(PlatSocket_t *s)
{
\tif (s == NULL)
\t{
\t\treturn;
\t}
#if PLAT_WINDOWS
\ts->sock = INVALID_SOCKET;
#else
\ts->fd = -1;
#endif
\ts->valid = false;
}

static const char *skip_ws(const char *s)
{
	while (s != NULL && *s != '\0' && isspace((unsigned char)*s))
	{
		++s;
	}
	return s;
}

static bool parse_token(const char **cursor, char *out, size_t out_len)
{
	const char *p;
	size_t len = 0U;

	if (cursor == NULL || *cursor == NULL || out == NULL || out_len == 0U)
	{
		return false;
	}

	p = skip_ws(*cursor);
	if (p == NULL || *p == '\0')
	{
		return false;
	}

	while (*p != '\0' && !isspace((unsigned char)*p))
	{
		if (len + 1U < out_len)
		{
			out[len++] = *p;
		}
		p++;
	}
	out[len] = '\0';
	*cursor = p;
	return len > 0U;
}

static void format_response(const TagRpcResponse_t *rsp, char *out, size_t out_len)
{
	if (rsp == NULL || out == NULL || out_len == 0U)
	{
		return;
	}

	if (rsp->status == 0)
	{
		if (rsp->type[0] != '\0' && rsp->value[0] != '\0')
		{
			snprintf(out, out_len, "OK %s %s\n", rsp->type, rsp->value);
		}
		else
		{
			snprintf(out, out_len, "OK\n");
		}
		return;
	}

	if (rsp->msg[0] == '\0')
	{
		snprintf(out, out_len, "ERR %d error\n", rsp->status);
	}
	else
	{
		snprintf(out, out_len, "ERR %d %s\n", rsp->status, rsp->msg);
	}
}

static void handle_request(TagTcpServer_t *s, const char *line, char *out, size_t out_len)
{
	TagRpcRequest_t req;
	TagRpcResponse_t rsp;
	char cmd[16];
	char name[TAGRPC_NAME_MAX];
	char value[TAGRPC_VALUE_MAX];
	const char *cursor = line;
	static uint32_t next_id = 1U;

	memset(&req, 0, sizeof(req));
	memset(&rsp, 0, sizeof(rsp));
	value[0] = '\0';

	if (!parse_token(&cursor, cmd, sizeof(cmd)))
	{
		snprintf(out, out_len, "ERR -1 bad_cmd\n");
		return;
	}

	if (strcmp(cmd, "READ") == 0)
	{
		if (!parse_token(&cursor, name, sizeof(name)))
		{
			snprintf(out, out_len, "ERR -1 bad_cmd\n");
			return;
		}
		req.op = TAGRPC_READ;
	}
	else if (strcmp(cmd, "WRITE") == 0)
	{
		if (!parse_token(&cursor, name, sizeof(name)))
		{
			snprintf(out, out_len, "ERR -1 bad_cmd\n");
			return;
		}
		if (!parse_token(&cursor, value, sizeof(value)))
		{
			snprintf(out, out_len, "ERR -1 bad_cmd\n");
			return;
		}
		req.op = TAGRPC_WRITE;
	}
	else
	{
		snprintf(out, out_len, "ERR -1 bad_cmd\n");
		return;
	}

	req.id = next_id++;
	snprintf(req.name, sizeof(req.name), "%s", name);
	if (req.op == TAGRPC_WRITE)
	{
		snprintf(req.value, sizeof(req.value), "%s", value);
	}

	if (!tag_rpc_send_request(s->ch, &req))
	{
		snprintf(out, out_len, "ERR -2 queue_full\n");
		return;
	}

	for (;;)
	{
		if (plat_atomic_load_bool(&s->stop_flag))
		{
			snprintf(out, out_len, "ERR -3 stopping\n");
			return;
		}
		if (tag_rpc_recv_response(s->ch, &rsp))
		{
			if (rsp.id == req.id)
			{
				format_response(&rsp, out, out_len);
				return;
			}
		}
		plat_thread_sleep_ms(1);
	}
}

static void *tag_tcp_server_thread(void *arg)
{
	TagTcpServer_t *s = (TagTcpServer_t *)arg;
	char recv_buf[TAG_TCP_MAX_LINE];
	char line_buf[TAG_TCP_MAX_LINE];
	size_t line_len = 0U;

	if (s == NULL || s->ch == NULL)
	{
		return NULL;
	}

	socket_invalidate(&s->server_sock);
	socket_invalidate(&s->client_sock);

	if (plat_socket_init() != 0)
	{
		return NULL;
	}

	if (plat_socket_listen(&s->server_sock, s->port, 1) != 0)
	{
		plat_socket_shutdown();
		return NULL;
	}

	for (;;)
	{
		size_t rx = 0U;
		int rc;

		if (plat_atomic_load_bool(&s->stop_flag))
		{
			break;
		}

		if (!s->client_sock.valid)
		{
			if (plat_socket_accept(&s->server_sock, &s->client_sock) != 0)
			{
				plat_thread_sleep_ms(10);
				continue;
			}
			line_len = 0U;
		}

		rc = plat_socket_recv(&s->client_sock, recv_buf, sizeof(recv_buf), &rx);
		if (rc == 0)
		{
			plat_socket_close(&s->client_sock);
			line_len = 0U;
			continue;
		}
		if (rc < 0)
		{
			plat_socket_close(&s->client_sock);
			line_len = 0U;
			continue;
		}

		for (size_t i = 0U; i < rx; ++i)
		{
			char c = recv_buf[i];
			if (c == '\n')
			{
				char response[TAG_TCP_MAX_LINE];
				line_buf[line_len] = '\0';
				handle_request(s, line_buf, response, sizeof(response));
				if (plat_socket_send_all(&s->client_sock, response, strlen(response)) != 0)
				{
					plat_socket_close(&s->client_sock);
					line_len = 0U;
					break;
				}
				line_len = 0U;
				continue;
			}
			if (c == '\r')
			{
				continue;
			}
			if (line_len + 1U < sizeof(line_buf))
			{
				line_buf[line_len++] = c;
			}
		}
	}

	plat_socket_close(&s->client_sock);
	plat_socket_close(&s->server_sock);
	plat_socket_shutdown();
	return NULL;
}

int tag_tcp_server_start(TagTcpServer_t* s, TagRpcChannel_t* ch, uint16_t port)
{
	if (s == NULL || ch == NULL)
	{
		return -1;
	}

	memset(s, 0, sizeof(*s));
	s->ch = ch;
	s->port = port;
	plat_atomic_store_bool(&s->stop_flag, false);
	socket_invalidate(&s->server_sock);
	socket_invalidate(&s->client_sock);

	if (plat_thread_create(&s->thread, PLAT_THREAD_NORMAL, NULL, tag_tcp_server_thread, s) != 0)
	{
		return -1;
	}

	return 0;
}

void tag_tcp_server_stop(TagTcpServer_t* s)
{
	if (s == NULL)
	{
		return;
	}

	plat_atomic_store_bool(&s->stop_flag, true);
	plat_socket_close(&s->client_sock);
	plat_socket_close(&s->server_sock);
	plat_thread_join(&s->thread);
}
