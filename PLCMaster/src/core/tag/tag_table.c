/* Tag table stub implementations. TODO: implement tag registration and lookup. */

#include "tag_table.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

int tag_table_init(TagTable_t *t)
{
	if (t == NULL)
	{
		return -1;
	}

	memset(t, 0, sizeof(*t));
	return 0;
}

void tag_table_deinit(TagTable_t *t)
{
	if (t == NULL)
	{
		return;
	}

	memset(t, 0, sizeof(*t));
}

int tag_table_add(TagTable_t *t, const TagEntry_t *e)
{
	size_t i;
	TagEntry_t *dest;
	int rc;

	if (t == NULL || e == NULL || e->full_name[0] == '\0')
	{
		return -1;
	}

	for (i = 0; i < t->count; ++i)
	{
		if (strncmp(t->entries[i].full_name, e->full_name, sizeof(t->entries[i].full_name)) == 0)
		{
			return -1;
		}
	}

	if (t->count >= MAX_TAGS)
	{
		return -1;
	}

	dest = &t->entries[t->count];
	memset(dest, 0, sizeof(*dest));

	rc = snprintf(dest->full_name, sizeof(dest->full_name), "%s", e->full_name);
	if (rc < 0 || (size_t)rc >= sizeof(dest->full_name))
	{
		memset(dest, 0, sizeof(*dest));
		return -1;
	}

	rc = snprintf(dest->backend_name, sizeof(dest->backend_name), "%s", e->backend_name);
	if (rc < 0 || (size_t)rc >= sizeof(dest->backend_name))
	{
		memset(dest, 0, sizeof(*dest));
		return -1;
	}

	dest->type = e->type;
	dest->dir = e->dir;
	dest->offset_byte = e->offset_byte;
	dest->bit_index = e->bit_index;
	dest->kind = e->kind;
	dest->proc_index = e->proc_index;

	t->count++;
	return 0;
}

TagId_t tag_table_find_id(const TagTable_t *t, const char *full_name)
{
	uint32_t i;

	if (t == NULL || full_name == NULL)
	{
		return 0;
	}

	for (i = 0; i < t->count; ++i)
	{
		if (strncmp(t->entries[i].full_name, full_name, sizeof(t->entries[i].full_name)) == 0)
		{
			return (TagId_t)(i + 1U);
		}
	}

	return 0;
}

const TagEntry_t *tag_table_get(const TagTable_t *t, TagId_t id)
{
	if (t == NULL || id == 0 || id > t->count)
	{
		return NULL;
	}

	return &t->entries[id - 1U];
}

uint32_t tag_table_count(const TagTable_t *t)
{
	if (t == NULL)
	{
		return 0U;
	}

	return t->count;
}
