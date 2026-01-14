/* PLC application data. */

#ifndef PLC_APP_H
#define PLC_APP_H

#include "app/io/io_view.h"
#include "app/io/hmi_view.h"
#include "app/io/proc_view.h"

typedef struct
{
	IOView_t io;
	HMIView_t hmi;
	ProcView_t proc;
} PlcApp_t;

#endif /* PLC_APP_H */
