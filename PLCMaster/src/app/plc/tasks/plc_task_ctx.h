/* PLC task context definition. */

#ifndef PLC_TASK_CTX_H
#define PLC_TASK_CTX_H

#include "core/runtime/runtime.h"
#include "app/plc/plc_app.h"

typedef struct PlcTaskCtx
{
	Runtime_t *rt;
	PlcApp_t *app;
} PlcTaskCtx_t;

#endif /* PLC_TASK_CTX_H */
