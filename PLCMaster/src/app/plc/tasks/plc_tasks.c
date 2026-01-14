/* PLC task registry implementation. */

#include <stdbool.h>
#include <stdio.h>
#include "app/plc/tasks/plc_tasks.h"
#include "temp_converter.h"

int plc_task_noop(void* ctx)
{
	(void)ctx;
	return 0;
}

int plc_task_heartbeat(void* ctx)
{
	PlcTaskCtx_t *c = (PlcTaskCtx_t*)ctx;
	static bool state = false;
	if (c == NULL || c->rt == NULL || c->app == NULL)
	{
		return -1;
	}
	state = !state;
	tag_write_bool(c->rt, c->app->io.X15_Out0, state);
	tag_write_bool(c->rt, c->app->io.X14_Out5, !state);

	TagId_t counter = tag_table_find_id(&c->rt->tag_table, "proc.counter_test");
	if (counter == 0) {
		printf("error to find tag\n");
	}
	uint32_t counter_val;
	if (tag_read_u32(c->rt, counter, &counter_val) < 0) {
		printf("error to read tag\n");
	}
	counter_val++;
	tag_write_u32(c->rt, counter, counter_val);
	if (counter_val % 10 == 0) {
		printf("[TASK] counter = %d\n", counter_val);
	}
	return 0;
}

int process_analog_inputs(void* ctx)
{
	PlcTaskCtx_t* c = (PlcTaskCtx_t*)ctx;
	float temp_cuve;
	bool temp_cuve_state, alarm_state;


	tag_read_bool(c->rt, c->app->io.X21_CPU_Pt1_Pt_State, &temp_cuve_state);
	if (temp_cuve_state == 0) {
		//error
		alarm_state = 1;
	}
	else {
		tag_read_real(c->rt, c->app->io.X21_CPU_Pt1_Pt_Value, &temp_cuve);
	}
	

	//tag_write_bool(c->rt, c->app->.HMI_alarm_state, alarm_state);
}