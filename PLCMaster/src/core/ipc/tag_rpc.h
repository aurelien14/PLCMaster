#ifndef TAG_RPC_H
#define TAG_RPC_H

#include <stdbool.h>
#include <stdint.h>

#include "core/ipc/spsc_ring.h"

#define TAGRPC_NAME_MAX 64
#define TAGRPC_VALUE_MAX 64
#define TAGRPC_ERRMSG_MAX 64

typedef enum
{
	TAGRPC_READ = 1,
	TAGRPC_WRITE = 2
} TagRpcOp_t;

typedef struct
{
	uint32_t id;
	TagRpcOp_t op;
	char name[TAGRPC_NAME_MAX];
	char value[TAGRPC_VALUE_MAX];
} TagRpcRequest_t;

typedef struct
{
	uint32_t id;
	int32_t status;
	char type[16];
	char value[TAGRPC_VALUE_MAX];
	char msg[TAGRPC_ERRMSG_MAX];
} TagRpcResponse_t;

typedef struct
{
	SpscRing_t req_q;
	SpscRing_t rsp_q;
} TagRpcChannel_t;

int tag_rpc_init(TagRpcChannel_t* ch);
bool tag_rpc_send_request(TagRpcChannel_t* ch, const TagRpcRequest_t* req);
bool tag_rpc_recv_request(TagRpcChannel_t* ch, TagRpcRequest_t* out);
bool tag_rpc_send_response(TagRpcChannel_t* ch, const TagRpcResponse_t* rsp);
bool tag_rpc_recv_response(TagRpcChannel_t* ch, TagRpcResponse_t* out);

#endif /* TAG_RPC_H */
