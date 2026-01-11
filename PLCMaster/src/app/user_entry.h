/* Application entrypoints. */

#ifndef USER_ENTRY_H
#define USER_ENTRY_H

#include "app/config/config_static.h"
#include "app/plc/plc_app.h"
#include "core/plc/plc_scheduler.h"
#include "core/runtime/runtime.h"

const SystemConfig_t *user_get_config(void);
int user_bind(PlcApp_t *app, Runtime_t *rt);
int user_register_plc_tasks(PlcScheduler_t *sched, Runtime_t *rt, PlcApp_t *app);

#endif /* USER_ENTRY_H */
