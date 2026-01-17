/* PLC runtime store initialization helpers. */

#ifndef PLC_STORE_INIT_H
#define PLC_STORE_INIT_H

#include "app/config/config_static.h"
#include "core/runtime/runtime.h"

int proc_init(Runtime_t *rt, const ProcessVarDesc_t *descs, size_t count, StartMode_t mode);
int hmi_init(Runtime_t *rt, const HmiTagDesc_t *descs, size_t count, StartMode_t mode);

int proc_load_retained(Runtime_t *rt, const ProcessVarDesc_t *descs, size_t count);
int proc_save_retained(Runtime_t *rt, const ProcessVarDesc_t *descs, size_t count);
int proc_erase_retained_storage(void);

int hmi_load_retained(Runtime_t *rt, const HmiTagDesc_t *descs, size_t count);
int hmi_save_retained(Runtime_t *rt, const HmiTagDesc_t *descs, size_t count);
int hmi_erase_retained_storage(void);

#endif /* PLC_STORE_INIT_H */
