/* PLC task implementations for the application layer. */

#ifndef APP_PLC_TASKS_H
#define APP_PLC_TASKS_H

#include "app/plc/tasks/plc_task_ctx.h"
#include "core/tag/tag_api.h"

int plc_task_noop(void *ctx);
int plc_task_heartbeat(void *ctx);

#endif /* APP_PLC_TASKS_H */
