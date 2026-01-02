/* Tag table stub implementations. TODO: implement tag registration and lookup. */

#include "tag_table.h"

#include <stddef.h>

struct TagTable
{
    int unused;
};

int tag_table_init(TagTable_t *t)
{
    if (t == NULL)
    {
        return -1;
    }

    t->unused = 0;
    return 0;
}

void tag_table_deinit(TagTable_t *t)
{
    if (t == NULL)
    {
        return;
    }

    t->unused = 0;
}

TagId_t tag_table_find_id(const TagTable_t *t, const char *full_name)
{
    (void)t;
    (void)full_name;
    return 0;
}
