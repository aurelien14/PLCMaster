/* PLC application bindings. */

#ifndef PLC_APP_H
#define PLC_APP_H

#include "core/tag/tag_table.h"
#include "app/io/io_view.h"
#include "app/plc/plc_app.h"
#include "core/runtime/runtime.h"

typedef struct
{
	IOView_t io;
	Runtime_t* runtime;
} PlcApp_t;

int plc_app_bind(PlcApp_t* app, const TagTable_t* tags);
int plc_dio_test(void* ctx);

#endif /* PLC_APP_H */
