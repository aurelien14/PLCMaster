/* PLC application bindings. */

#ifndef PLC_APP_H
#define PLC_APP_H

#include "core/tag/tag_table.h"
#include "plc/io_view.h"

typedef struct
{
	IOView_t io;
} PlcApp_t;

int plc_app_bind(PlcApp_t* app, const TagTable_t* tags);

#endif /* PLC_APP_H */
