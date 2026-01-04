/* PLC runtime orchestration helpers. */

#ifndef PLC_RUNTIME_H
#define PLC_RUNTIME_H

#include "core/plc/plc_scheduler.h"
#include "core/runtime/runtime.h"

int plc_runtime_run(Runtime_t *rt, PlcScheduler_t *sched);

#endif /* PLC_RUNTIME_H */
