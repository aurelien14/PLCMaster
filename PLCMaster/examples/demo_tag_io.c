/* Optional tag read/write demo, compiled only in DEV builds. */

#ifdef DEV

#include "examples/demo_tag_io.h"

#include <stdbool.h>
#include <stdio.h>

#include "core/tag/tag_api.h"

int demo_tag_io_run(Runtime_t *rt)
{
	TagId_t temp_sp_id;
	TagId_t run_cmd_id;
	TagId_t hmi_temp_sp_id;
	TagId_t hmi_alarm_code_id;
	float temp_read = 0.0f;
	bool run_read = false;
	uint16_t alarm_read = 0U;

	if (rt == NULL)
	{
		return -1;
	}

	temp_sp_id = tag_table_find_id(&rt->tag_table, "proc.temp_sp");
	run_cmd_id = tag_table_find_id(&rt->tag_table, "proc.run_cmd");
	hmi_temp_sp_id = tag_table_find_id(&rt->tag_table, "hmi.temp_setpoint");
	hmi_alarm_code_id = tag_table_find_id(&rt->tag_table, "hmi.alarm_code");

	if (temp_sp_id == 0 || run_cmd_id == 0 || hmi_temp_sp_id == 0 || hmi_alarm_code_id == 0)
	{
		return -1;
	}

	if (tag_read_real(rt, temp_sp_id, &temp_read) != 0)
	{
		return -1;
	}

	printf("proc.temp_sp initial = %.2f\n", temp_read);

	if (tag_write_real(rt, temp_sp_id, 25.5f) != 0)
	{
		return -1;
	}

	if (tag_read_real(rt, temp_sp_id, &temp_read) != 0)
	{
		return -1;
	}

	printf("proc.temp_sp after write = %.2f\n", temp_read);

	if (tag_read_bool(rt, run_cmd_id, &run_read) != 0)
	{
		return -1;
	}

	printf("proc.run_cmd initial = %s\n", run_read ? "true" : "false");

	if (tag_write_bool(rt, run_cmd_id, true) != 0)
	{
		return -1;
	}

	if (tag_read_bool(rt, run_cmd_id, &run_read) != 0)
	{
		return -1;
	}

	printf("proc.run_cmd after write = %s\n", run_read ? "true" : "false");

	if (tag_read_real(rt, hmi_temp_sp_id, &temp_read) != 0)
	{
		return -1;
	}

	printf("hmi.temp_setpoint initial = %.2f\n", temp_read);

	if (tag_write_real(rt, hmi_temp_sp_id, 30.0f) != 0)
	{
		return -1;
	}

	if (tag_read_real(rt, temp_sp_id, &temp_read) != 0)
	{
		return -1;
	}

	printf("proc.temp_sp after hmi write = %.2f\n", temp_read);

	if (tag_write_u16(rt, hmi_alarm_code_id, 1234U) == 0)
	{
		return -1;
	}

	if (tag_read_u16(rt, hmi_alarm_code_id, &alarm_read) != 0)
	{
		return -1;
	}

	printf("hmi.alarm_code after failed write = %u\n", alarm_read);

	return 0;
}

#endif /* DEV */
