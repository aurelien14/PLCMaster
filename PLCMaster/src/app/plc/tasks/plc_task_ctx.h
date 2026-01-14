/* PLC task context definition. */

#ifndef PLC_TASK_CTX_H
#define PLC_TASK_CTX_H

#include "core/runtime/runtime.h"
#include "app/io/io_view.h"
#include "app/io/hmi_view.h"
#include "app/io/proc_view.h"

typedef struct PlcTaskCtx
{
	Runtime_t *rt;
	IOView_t *io;
	HMIView_t *hmi;
	ProcView_t *proc;
} PlcTaskCtx_t;

#endif /* PLC_TASK_CTX_H */
