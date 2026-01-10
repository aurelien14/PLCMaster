/* Tag API stub implementations. TODO: connect to actual tag backend. */

#include "tag_api.h"

#include <stddef.h>
#include <string.h>

static BackendDriver_t *find_backend(Runtime_t *rt, const char *name)
{
	size_t i;

	if (rt == NULL || name == NULL) {
		return NULL;
	}

	for (i = 0; i < rt->backend_count; ++i) {
		BackendDriver_t *drv = &rt->backends[i];
		if (strcmp(drv->name, name) == 0) {
			return drv;
		}
	}

	return NULL;
}

static const TagEntry_t *get_resolved_entry(Runtime_t* rt, TagId_t id, TagType_t expected_type, int for_write, int depth)
{
	const TagEntry_t *entry;

	if (rt == NULL) {
		return NULL;
	}

	if (depth > 4) {
		return NULL;
	}

	entry = tag_table_get(&rt->tag_table, id);
	if (entry == NULL) {
		return NULL;
	}

	if (entry->type != expected_type) {
		return NULL;
	}

	if (entry->kind == TAGK_HMI_ALIAS) {
		if (for_write && entry->hmi_access == 0U) {
			return NULL;
		}
		if (entry->alias_target == 0U) {
			return NULL;
		}
		return get_resolved_entry(rt, entry->alias_target, expected_type, for_write, depth + 1);
	}

	return entry;
}

static ProcValue_t *get_proc_value(Runtime_t *rt, const TagEntry_t *entry)
{
	ProcValue_t *value;

	if (rt == NULL || entry == NULL) {
		return NULL;
	}

	if (entry->proc_index >= rt->proc_store.count) {
		return NULL;
	}

	value = &rt->proc_store.values[entry->proc_index];
	if (value->type != entry->type) {
		return NULL;
	}

	return value;
}

static const uint8_t *get_io_read_base(Runtime_t *rt, const TagEntry_t *entry, size_t *size_out)
{
	BackendDriver_t *drv;

	if (size_out != NULL) {
		*size_out = 0U;
	}

	if (rt == NULL || entry == NULL || entry->device_cfg == NULL) {
		return NULL;
	}

	drv = find_backend(rt, entry->backend_name);
	if (drv == NULL || drv->ops == NULL) {
		return NULL;
	}

	if (entry->dir == TAG_DIR_IN) {
		if (drv->ops->get_input_data == NULL) {
			return NULL;
		}
		return drv->ops->get_input_data(drv, entry->device_cfg, size_out);
	}

	if (drv->ops->get_output_data == NULL) {
		return NULL;
	}
	return drv->ops->get_output_data(drv, entry->device_cfg, size_out);
}

static uint8_t *get_io_write_base(Runtime_t *rt, const TagEntry_t *entry, size_t *size_out)
{
	BackendDriver_t *drv;

	if (size_out != NULL) {
		*size_out = 0U;
	}

	if (rt == NULL || entry == NULL || entry->device_cfg == NULL) {
		return NULL;
	}

	if (entry->dir != TAG_DIR_OUT) {
		return NULL;
	}

	drv = find_backend(rt, entry->backend_name);
	if (drv == NULL || drv->ops == NULL || drv->ops->get_output_data == NULL) {
		return NULL;
	}

	return drv->ops->get_output_data(drv, entry->device_cfg, size_out);
}

static int read_io_value(Runtime_t *rt, const TagEntry_t *entry, TagValue_u *out_value)
{
	const uint8_t *base;
	size_t size;

	if (out_value == NULL || entry == NULL) {
		return -1;
	}

	base = get_io_read_base(rt, entry, &size);
	if (base == NULL) {
		return -1;
	}

	if (entry->type == TAG_T_BOOL) {
		if (entry->bit_index >= 8U || entry->offset_byte >= size) {
			return -1;
		}
		out_value->b = ((base[entry->offset_byte] >> entry->bit_index) & 0x1U) != 0U;
		return 0;
	}

	switch (entry->type) {
	case TAG_T_U8:
		if (entry->offset_byte + sizeof(uint8_t) > size) {
			return -1;
		}
		out_value->u8 = base[entry->offset_byte];
		return 0;
	case TAG_T_U16:
		if (entry->offset_byte + sizeof(uint16_t) > size) {
			return -1;
		}
		memcpy(&out_value->u16, base + entry->offset_byte, sizeof(uint16_t));
		return 0;
	case TAG_T_U32:
		if (entry->offset_byte + sizeof(uint32_t) > size) {
			return -1;
		}
		memcpy(&out_value->u32, base + entry->offset_byte, sizeof(uint32_t));
		return 0;
	case TAG_T_REAL:
		if (entry->offset_byte + sizeof(float) > size) {
			return -1;
		}
		memcpy(&out_value->f, base + entry->offset_byte, sizeof(float));
		return 0;
	case TAG_T_BOOL:
	default:
		return -1;
	}
}

static int write_io_value(Runtime_t *rt, const TagEntry_t *entry, TagValue_u value)
{
	uint8_t *base;
	size_t size;

	if (entry == NULL) {
		return -1;
	}

	base = get_io_write_base(rt, entry, &size);
	if (base == NULL) {
		return -1;
	}

	if (entry->type == TAG_T_BOOL) {
		uint8_t mask;
		if (entry->bit_index >= 8U || entry->offset_byte >= size) {
			return -1;
		}
		mask = (uint8_t)(1U << entry->bit_index);
		if (value.b) {
			base[entry->offset_byte] |= mask;
		} else {
			base[entry->offset_byte] &= (uint8_t)~mask;
		}
		return 0;
	}

	switch (entry->type) {
	case TAG_T_U8:
		if (entry->offset_byte + sizeof(uint8_t) > size) {
			return -1;
		}
		base[entry->offset_byte] = value.u8;
		return 0;
	case TAG_T_U16:
		if (entry->offset_byte + sizeof(uint16_t) > size) {
			return -1;
		}
		memcpy(base + entry->offset_byte, &value.u16, sizeof(uint16_t));
		return 0;
	case TAG_T_U32:
		if (entry->offset_byte + sizeof(uint32_t) > size) {
			return -1;
		}
		memcpy(base + entry->offset_byte, &value.u32, sizeof(uint32_t));
		return 0;
	case TAG_T_REAL:
		if (entry->offset_byte + sizeof(float) > size) {
			return -1;
		}
		memcpy(base + entry->offset_byte, &value.f, sizeof(float));
		return 0;
	case TAG_T_BOOL:
	default:
		return -1;
	}
}

int tag_read_bool(Runtime_t* rt, TagId_t id, bool *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u value;

	if (out == NULL) {
		return -1;
	}

	entry = get_resolved_entry(rt, id, TAG_T_BOOL, 0, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		*out = v->v.b;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		if (read_io_value(rt, entry, &value) != 0) {
			return -1;
		}
		*out = value.b;
		return 0;
	}

	return -1;
}

int tag_write_bool(Runtime_t* rt, TagId_t id, bool value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u write_value;

	entry = get_resolved_entry(rt, id, TAG_T_BOOL, 1, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		v->v.b = value;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		write_value.b = value;
		return write_io_value(rt, entry, write_value);
	}

	return -1;
}

int tag_read_u8(Runtime_t* rt, TagId_t id, uint8_t *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u value;

	if (out == NULL) {
		return -1;
	}

	entry = get_resolved_entry(rt, id, TAG_T_U8, 0, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		*out = v->v.u8;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		if (read_io_value(rt, entry, &value) != 0) {
			return -1;
		}
		*out = value.u8;
		return 0;
	}

	return -1;
}

int tag_write_u8(Runtime_t* rt, TagId_t id, uint8_t value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u write_value;

	entry = get_resolved_entry(rt, id, TAG_T_U8, 1, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		v->v.u8 = value;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		write_value.u8 = value;
		return write_io_value(rt, entry, write_value);
	}

	return -1;
}

int tag_read_u16(Runtime_t* rt, TagId_t id, uint16_t *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u value;

	if (out == NULL) {
		return -1;
	}

	entry = get_resolved_entry(rt, id, TAG_T_U16, 0, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		*out = v->v.u16;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		if (read_io_value(rt, entry, &value) != 0) {
			return -1;
		}
		*out = value.u16;
		return 0;
	}

	return -1;
}

int tag_write_u16(Runtime_t* rt, TagId_t id, uint16_t value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u write_value;

	entry = get_resolved_entry(rt, id, TAG_T_U16, 1, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		v->v.u16 = value;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		write_value.u16 = value;
		return write_io_value(rt, entry, write_value);
	}

	return -1;
}

int tag_read_u32(Runtime_t* rt, TagId_t id, uint32_t *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u value;

	if (out == NULL) {
		return -1;
	}

	entry = get_resolved_entry(rt, id, TAG_T_U32, 0, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		*out = v->v.u32;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		if (read_io_value(rt, entry, &value) != 0) {
			return -1;
		}
		*out = value.u32;
		return 0;
	}

	return -1;
}

int tag_write_u32(Runtime_t* rt, TagId_t id, uint32_t value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u write_value;

	entry = get_resolved_entry(rt, id, TAG_T_U32, 1, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		v->v.u32 = value;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		write_value.u32 = value;
		return write_io_value(rt, entry, write_value);
	}

	return -1;
}

int tag_read_real(Runtime_t* rt, TagId_t id, float *out)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u value;

	if (out == NULL) {
		return -1;
	}

	entry = get_resolved_entry(rt, id, TAG_T_REAL, 0, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		*out = v->v.f;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		if (read_io_value(rt, entry, &value) != 0) {
			return -1;
		}
		*out = value.f;
		return 0;
	}

	return -1;
}

int tag_write_real(Runtime_t* rt, TagId_t id, float value)
{
	const TagEntry_t *entry;
	ProcValue_t *v;
	TagValue_u write_value;

	entry = get_resolved_entry(rt, id, TAG_T_REAL, 1, 0);
	if (entry == NULL) {
		return -1;
	}

	if (entry->kind == TAGK_PROC) {
		v = get_proc_value(rt, entry);
		if (v == NULL) {
			return -1;
		}
		v->v.f = value;
		return 0;
	}

	if (entry->kind == TAGK_IO) {
		write_value.f = value;
		return write_io_value(rt, entry, write_value);
	}

	return -1;
}
