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
	bool run = false;
	if (c == NULL || c->rt == NULL || c->io == NULL || c->hmi == NULL || c->proc == NULL)
	{
		return -1;
	}
	state = !state;
	if (tag_read_bool(c->rt, c->hmi->run, &run) != 0)
	{
		printf("error to read hmi.run\n");
		return -1;
	}

	if (run)
	{
		tag_write_bool(c->rt, c->io->X15_Out0, state);
		tag_write_bool(c->rt, c->io->X14_Out5, !state);
	}

	{
		uint32_t counter_val = 0;
		if (tag_read_u32(c->rt, c->proc->counter_test, &counter_val) != 0)
		{
			printf("error to read proc.counter_test\n");
			return -1;
		}
		counter_val++;
		tag_write_u32(c->rt, c->proc->counter_test, counter_val);
		if (counter_val % 10 == 0)
		{
			printf("[TASK] counter = %d\n", counter_val);
		}
	}
	return 0;
}

int process_analog_inputs(void* ctx)
{
	PlcTaskCtx_t* c = (PlcTaskCtx_t*)ctx;
	float temp_cuve;
	bool temp_cuve_state, alarm_state;


	tag_read_bool(c->rt, c->io->X21_CPU_Pt1_Pt_State, &temp_cuve_state);
	if (temp_cuve_state == 0) {
		//error
		alarm_state = 1;
	}
	else {
		tag_read_real(c->rt, c->io->X21_CPU_Pt1_Pt_Value, &temp_cuve);
	}
	

	//tag_write_bool(c->rt, c->app->.HMI_alarm_state, alarm_state);
}
