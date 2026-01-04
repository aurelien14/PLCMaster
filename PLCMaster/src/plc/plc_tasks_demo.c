/* Demo PLC tasks. */

#include "plc/plc_tasks_demo.h"

#include <stdio.h>

int plc_demo_task_blink(void* ctx)
{
	PlcDemoBlinkCtx_t* blink = (PlcDemoBlinkCtx_t*)ctx;

	if (blink != NULL)
	{
		blink->counter++;
		printf("[PLC] blink count=%u\n", blink->counter);
	}

	return 0;
}

int plc_demo_task_control(void* ctx)
{
	float temp_sp = 0.0f;
	PlcDemoControlCtx_t* control = (PlcDemoControlCtx_t*)ctx;

	if (control == NULL || control->runtime == NULL)
	{
		return -1;
	}

	if (control->temp_sp_id == 0)
	{
		printf("[PLC] control: temp_sp id missing (TODO bind)\n");
		return 0;
	}

	if (tag_read_real(control->runtime, control->temp_sp_id, &temp_sp) != 0)
	{
		return -1;
	}

	printf("[PLC] control temp_sp=%.2f\n", temp_sp);
	temp_sp += 0.25f;

	return tag_write_real(control->runtime, control->temp_sp_id, temp_sp);
}
