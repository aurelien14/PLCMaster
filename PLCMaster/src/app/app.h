/* Application entrypoints. */

#ifndef APP_H
#define APP_H

#include "app/config/config_static.h"
#include "app/plc/plc_app.h"
#include "core/plc/plc_scheduler.h"
#include "core/runtime/runtime.h"

const SystemConfig_t *app_get_config(void);
int app_bind(PlcApp_t *app, Runtime_t *rt);
int app_register_tasks(PlcScheduler_t *sched, Runtime_t *rt, PlcApp_t *app);

#endif /* APP_H */
