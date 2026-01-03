/* Tag API stub implementations. TODO: connect to actual tag backend. */

#include "tag_api.h"

#include <stddef.h>

static int get_proc_value(Runtime_t* rt, TagId_t id, TagType_t expected_type, const TagEntry_t **out_entry, ProcValue_t **out_value)
{
	const TagEntry_t *entry;

	if (rt == NULL || out_entry == NULL || out_value == NULL) {
		return -1;
	}

	entry = tag_table_get(&rt->tag_table, id);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind != TAGK_PROC) {
		return -1;
	}

	if (entry->type != expected_type) {
		return -1;
	}

	if (entry->proc_index >= rt->proc_store.count) {
		return -1;
	}

	*out_entry = entry;
	*out_value = &rt->proc_store.values[entry->proc_index];
	if ((*out_value)->type != expected_type) {
		return -1;
	}
	return 0;
}

int tag_read_bool(Runtime_t* rt, TagId_t id, bool *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_value(rt, id, TAG_T_BOOL, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	*out = v->v.b;
	return 0;
}

int tag_write_bool(Runtime_t* rt, TagId_t id, bool value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (get_proc_value(rt, id, TAG_T_BOOL, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	v->v.b = value;
	return 0;
}

int tag_read_u8(Runtime_t* rt, TagId_t id, uint8_t *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_value(rt, id, TAG_T_U8, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	*out = v->v.u8;
	return 0;
}

int tag_write_u8(Runtime_t* rt, TagId_t id, uint8_t value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (get_proc_value(rt, id, TAG_T_U8, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	v->v.u8 = value;
	return 0;
}

int tag_read_u16(Runtime_t* rt, TagId_t id, uint16_t *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_value(rt, id, TAG_T_U16, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	*out = v->v.u16;
	return 0;
}

int tag_write_u16(Runtime_t* rt, TagId_t id, uint16_t value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (get_proc_value(rt, id, TAG_T_U16, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	v->v.u16 = value;
	return 0;
}

int tag_read_u32(Runtime_t* rt, TagId_t id, uint32_t *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_value(rt, id, TAG_T_U32, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	*out = v->v.u32;
	return 0;
}

int tag_write_u32(Runtime_t* rt, TagId_t id, uint32_t value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (get_proc_value(rt, id, TAG_T_U32, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	v->v.u32 = value;
	return 0;
}

int tag_read_real(Runtime_t* rt, TagId_t id, float *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (out == NULL) {
		return -1;
	}

	if (get_proc_value(rt, id, TAG_T_REAL, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	*out = v->v.f;
	return 0;
}

int tag_write_real(Runtime_t* rt, TagId_t id, float value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;

	if (get_proc_value(rt, id, TAG_T_REAL, &entry, &v) != 0) {
		return -1;
	}

	(void)entry;
	v->v.f = value;
	return 0;
}
