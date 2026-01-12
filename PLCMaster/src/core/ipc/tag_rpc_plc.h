#ifndef TAG_RPC_PLC_H
#define TAG_RPC_PLC_H

#include <stdint.h>

#include "core/ipc/tag_rpc.h"
#include "core/runtime/runtime.h"

void tag_rpc_plc_poll(TagRpcChannel_t* ch, Runtime_t* rt, uint32_t max_per_cycle);

#endif /* TAG_RPC_PLC_H */
