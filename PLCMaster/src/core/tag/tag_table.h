/* Tag table interface for registering and retrieving tags. TODO: complete definitions. */

#ifndef TAG_TABLE_H
#define TAG_TABLE_H

#include <stdint.h>
#include "core/device/device_desc.h"

#define MAX_TAGS 256U

typedef uint32_t TagId_t; /* 0 = invalid */

typedef enum TagKind
{
	TAGK_INVALID = 0,
	TAGK_IO = 1,
	TAGK_PROC = 2,
	TAGK_HMI_ALIAS = 3
} TagKind_t;

typedef struct TagIORef
{
	uint16_t device_index;
	uint16_t backend_index;
	TagDir_t dir;
	uint32_t byte_offset;
	uint8_t bit;
} TagIORef_t;

typedef struct TagProcRef
{
	uint32_t index;
} TagProcRef_t;

typedef struct TagHmiAliasRef
{
	TagId_t target;
	uint8_t access;
} TagHmiAliasRef_t;

typedef struct TagEntry
{
	const char *full_name;
	char full_name_storage[128];
	TagType_t type;
	TagKind_t kind;
	union
	{
		TagIORef_t io;
		TagProcRef_t proc;
		TagHmiAliasRef_t hmi_alias;
	} ref;
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
uint32_t tag_table_count(const TagTable_t *t);

#endif /* TAG_TABLE_H */
