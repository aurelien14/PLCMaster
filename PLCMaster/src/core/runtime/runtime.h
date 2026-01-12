/* Runtime lifecycle interface. TODO: expand runtime structure. */

#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/runtime/runtime_health.h"
#include "core/tag/tag_table.h"
#include "core/backend/backend_iface.h"

typedef union
{
	bool b;
	int i;
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	float f;
} TagValue_u;

typedef struct
{
	TagType_t type;
	TagValue_u v;
} ProcValue_t;

typedef struct
{
	ProcValue_t values[64];
	size_t count;
} ProcessStore_t;

typedef struct Runtime
{
	TagTable_t tag_table;
	ProcessStore_t proc_store;
	void* status_view;
	BackendDriver_t** backends;
	size_t backend_count;
	BackendDriver_t* backend_array[MAX_BACKENDS];
} Runtime_t;

int runtime_init(Runtime_t* rt);
void runtime_deinit(Runtime_t* rt);
int runtime_backends_start(Runtime_t* rt);
int runtime_backends_stop(Runtime_t* rt);
void runtime_backends_cycle_begin(Runtime_t* rt);
void runtime_backends_sync_outputs(Runtime_t* rt);
void runtime_backends_process_nonrt(Runtime_t* rt);
PlcHealthLevel_t runtime_get_health_level(const Runtime_t* rt);

#endif /* RUNTIME_H */
