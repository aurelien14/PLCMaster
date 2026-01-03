/* Runtime lifecycle interface. TODO: expand runtime structure. */

#ifndef RUNTIME_H
#define RUNTIME_H

#include "core/tag/tag_table.h"

typedef struct Runtime
{
	TagTable_t tag_table;
	void* status_view;
	void* backends;
} Runtime_t;

int runtime_init(Runtime_t* rt);
void runtime_deinit(Runtime_t* rt);

#endif /* RUNTIME_H */
