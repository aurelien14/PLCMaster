/* HMI view bindings. */

#ifndef HMI_VIEW_H
#define HMI_VIEW_H

#include "core/tag/tag_table.h"

typedef struct
{
	TagId_t temp_setpoint;		/* hmi.temp_setpoint */
	TagId_t run;				/* hmi.run */
	TagId_t alarm_state;		/* hmi.alarm_state (RO) */
	TagId_t alarm_code;			/* hmi.alarm_code (RO) */
	TagId_t counter_test;		/* hmi.counter_test (alias -> proc.counter_test) */
} HMIView_t;

int hmi_view_bind(HMIView_t *hmi, const TagTable_t *tags);

#endif /* HMI_VIEW_H */
