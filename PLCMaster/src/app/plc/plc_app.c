/* PLC application bindings. */

#include "app/plc/plc_app.h"

int plc_app_bind(PlcApp_t* app, const TagTable_t* tags)
{
	TagId_t id;

	if (app == NULL || tags == NULL)
	{
		return -1;
	}

	id = tag_table_find_id(tags, "CPU_IO.X15_Out0");
	if (id == 0)
	{
		return -1;
	}

	app->io.X15_Out0 = id;

	return 0;
}
