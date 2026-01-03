/* Tag table interface for registering and retrieving tags. TODO: complete definitions. */

#ifndef TAG_TABLE_H
#define TAG_TABLE_H

#include <stdint.h>
#include "devices/device_desc.h"

#define MAX_TAGS 256U

typedef uint32_t TagId_t; /* 0 = invalid */

typedef struct TagEntry
{
	char full_name[128];
	TagType_t type;
	TagDir_t dir;
	uint32_t offset_byte;
	uint8_t bit_index;
	char backend_name[16];
} TagEntry_t;

typedef struct TagTable
{
	TagEntry_t entries[MAX_TAGS];
	uint32_t count;
} TagTable_t;


int tag_table_init(TagTable_t *t);
void tag_table_deinit(TagTable_t *t);
int tag_table_add(TagTable_t *t, const TagEntry_t *e);
TagId_t tag_table_find_id(const TagTable_t *t, const char *full_name);
const TagEntry_t *tag_table_get(const TagTable_t *t, TagId_t id);

#endif /* TAG_TABLE_H */
