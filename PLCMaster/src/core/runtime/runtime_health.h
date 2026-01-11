/* Runtime health evaluation. */

#ifndef RUNTIME_HEALTH_H
#define RUNTIME_HEALTH_H

typedef enum PlcHealthLevel
{
	PLC_HEALTH_OK = 0,
	PLC_HEALTH_WARN = 1,
	PLC_HEALTH_FAULT = 2
} PlcHealthLevel_t;

#endif /* RUNTIME_HEALTH_H */
