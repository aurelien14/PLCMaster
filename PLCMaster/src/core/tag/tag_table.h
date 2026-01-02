/* Tag table interface for registering and retrieving tags. TODO: complete definitions. */

#ifndef TAG_TABLE_H
#define TAG_TABLE_H

#include <stdint.h>

typedef uint32_t TagId_t;

typedef struct TagTable TagTable_t;

int tag_table_init(TagTable_t *t);
void tag_table_deinit(TagTable_t *t);
TagId_t tag_table_find_id(const TagTable_t *t, const char *full_name);

#endif /* TAG_TABLE_H */
