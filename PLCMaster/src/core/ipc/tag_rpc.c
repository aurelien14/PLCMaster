#include "tag_rpc.h"

#include <string.h>

#define TAG_RPC_CAPACITY 64U

static TagRpcRequest_t g_req_storage[TAG_RPC_CAPACITY];
static TagRpcResponse_t g_rsp_storage[TAG_RPC_CAPACITY];

int tag_rpc_init(TagRpcChannel_t* ch)
{
	if (ch == NULL)
	{
		return -1;
	}

	memset(ch, 0, sizeof(*ch));
	if (spsc_ring_init(&ch->req_q, g_req_storage, sizeof(g_req_storage[0]), TAG_RPC_CAPACITY) != 0)
	{
		return -1;
	}
	if (spsc_ring_init(&ch->rsp_q, g_rsp_storage, sizeof(g_rsp_storage[0]), TAG_RPC_CAPACITY) != 0)
	{
		return -1;
	}
	return 0;
}

bool tag_rpc_send_request(TagRpcChannel_t* ch, const TagRpcRequest_t* req)
{
	if (ch == NULL)
	{
		return false;
	}

	return spsc_ring_push(&ch->req_q, req);
}

bool tag_rpc_recv_request(TagRpcChannel_t* ch, TagRpcRequest_t* out)
{
	if (ch == NULL)
	{
		return false;
	}

	return spsc_ring_pop(&ch->req_q, out);
}

bool tag_rpc_send_response(TagRpcChannel_t* ch, const TagRpcResponse_t* rsp)
{
	if (ch == NULL)
	{
		return false;
	}

	return spsc_ring_push(&ch->rsp_q, rsp);
}

bool tag_rpc_recv_response(TagRpcChannel_t* ch, TagRpcResponse_t* out)
{
	if (ch == NULL)
	{
		return false;
	}

	return spsc_ring_pop(&ch->rsp_q, out);
}
