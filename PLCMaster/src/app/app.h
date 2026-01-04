/* Application entrypoints. */

#ifndef APP_H
#define APP_H

#include "core/plc/plc_scheduler.h"
#include "core/runtime/runtime.h"
#include "app/config/config_static.h"

const SystemConfig_t *app_get_config(void);
int app_register_plc_tasks(PlcScheduler_t *sched, Runtime_t *rt);
void app_log_bindings(void);

#endif /* APP_H */
