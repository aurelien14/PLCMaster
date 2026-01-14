/* Proc view bindings. */

#ifndef PROC_VIEW_H
#define PROC_VIEW_H

#include "core/tag/tag_table.h"

typedef struct
{
	TagId_t counter_test;	/* proc.counter_test */
} ProcView_t;

int proc_view_bind(ProcView_t *proc, const TagTable_t *tags);

#endif /* PROC_VIEW_H */
