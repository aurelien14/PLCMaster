/* Tag API stub implementations. TODO: connect to actual tag backend. */

#include "tag_api.h"

#include <stddef.h>
#include <string.h>

static const TagTable_t *g_tags = NULL;
static uint8_t *g_proc_storage = NULL;
static size_t g_proc_storage_size = 0U;

static size_t get_type_size(TagType_t type)
{
	switch (type) {
	case TAG_T_BOOL:
		return 1U;
	case TAG_T_U8:
		return 1U;
	case TAG_T_U16:
		return 2U;
	case TAG_T_U32:
		return 4U;
	case TAG_T_REAL:
		return sizeof(float);
	default:
		return 0U;
	}
}

static int get_proc_entry(TagId_t id, TagType_t expected_type, const TagEntry_t **out_entry)
{
	const TagEntry_t *entry;

	if (g_tags == NULL || g_proc_storage == NULL || out_entry == NULL) {
		return -1;
	}

	entry = tag_table_get(g_tags, id);
	if (entry == NULL) {
		return -1;
	}

	if (strncmp(entry->backend_name, "proc", sizeof(entry->backend_name)) != 0) {
		return -1;
	}

	if (entry->type != expected_type) {
		return -1;
	}

	if (entry->bit_index >= 8U) {
		return -1;
	}

	if (entry->offset_byte + get_type_size(entry->type) > g_proc_storage_size) {
		return -1;
	}

	*out_entry = entry;
	return 0;
}

int tag_api_bind_proc_storage(const TagTable_t *t, uint8_t *buffer, size_t size)
{
	if (t == NULL || buffer == NULL || size == 0U) {
		g_tags = NULL;
		g_proc_storage = NULL;
		g_proc_storage_size = 0U;
		return -1;
	}

	g_tags = t;
	g_proc_storage = buffer;
	g_proc_storage_size = size;
	return 0;
}

int tag_read_bool(TagId_t id, bool *out)
{
	const TagEntry_t *entry;
	uint8_t byte_val;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_entry(id, TAG_T_BOOL, &entry) != 0) {
		return -1;
	}

	byte_val = g_proc_storage[entry->offset_byte];
	*out = ((byte_val >> entry->bit_index) & 0x1U) != 0U;
	return 0;
}

int tag_write_bool(TagId_t id, bool v)
{
	const TagEntry_t *entry;
	uint8_t *byte_ptr;

	if (get_proc_entry(id, TAG_T_BOOL, &entry) != 0) {
		return -1;
	}

	byte_ptr = &g_proc_storage[entry->offset_byte];
	if (v) {
		*byte_ptr = (uint8_t)(*byte_ptr | (uint8_t)(1U << entry->bit_index));
	} else {
		*byte_ptr = (uint8_t)(*byte_ptr & (uint8_t)~(uint8_t)(1U << entry->bit_index));
	}

	return 0;
}

int tag_read_u8(TagId_t id, uint8_t *out)
{
	const TagEntry_t *entry;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_entry(id, TAG_T_U8, &entry) != 0) {
		return -1;
	}

	memcpy(out, &g_proc_storage[entry->offset_byte], sizeof(uint8_t));
	return 0;
}

int tag_write_u8(TagId_t id, uint8_t v)
{
	const TagEntry_t *entry;

	if (get_proc_entry(id, TAG_T_U8, &entry) != 0) {
		return -1;
	}

	memcpy(&g_proc_storage[entry->offset_byte], &v, sizeof(uint8_t));
	return 0;
}

int tag_read_u16(TagId_t id, uint16_t *out)
{
	const TagEntry_t *entry;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_entry(id, TAG_T_U16, &entry) != 0) {
		return -1;
	}

	memcpy(out, &g_proc_storage[entry->offset_byte], sizeof(uint16_t));
	return 0;
}

int tag_write_u16(TagId_t id, uint16_t v)
{
	const TagEntry_t *entry;

	if (get_proc_entry(id, TAG_T_U16, &entry) != 0) {
		return -1;
	}

	memcpy(&g_proc_storage[entry->offset_byte], &v, sizeof(uint16_t));
	return 0;
}

int tag_read_u32(TagId_t id, uint32_t *out)
{
	const TagEntry_t *entry;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_entry(id, TAG_T_U32, &entry) != 0) {
		return -1;
	}

	memcpy(out, &g_proc_storage[entry->offset_byte], sizeof(uint32_t));
	return 0;
}

int tag_write_u32(TagId_t id, uint32_t v)
{
	const TagEntry_t *entry;

	if (get_proc_entry(id, TAG_T_U32, &entry) != 0) {
		return -1;
	}

	memcpy(&g_proc_storage[entry->offset_byte], &v, sizeof(uint32_t));
	return 0;
}

int tag_read_real(TagId_t id, float *out)
{
	const TagEntry_t *entry;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_entry(id, TAG_T_REAL, &entry) != 0) {
		return -1;
	}

	memcpy(out, &g_proc_storage[entry->offset_byte], sizeof(float));
	return 0;
}

int tag_write_real(TagId_t id, float v)
{
	const TagEntry_t *entry;

	if (get_proc_entry(id, TAG_T_REAL, &entry) != 0) {
		return -1;
	}

	memcpy(&g_proc_storage[entry->offset_byte], &v, sizeof(float));
	return 0;
}
