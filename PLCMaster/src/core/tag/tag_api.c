/* Tag API stub implementations. TODO: connect to actual tag backend. */

#include "tag_api.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

static const TagEntry_t *resolve_entry(Runtime_t* rt, TagId_t id, int for_write, int depth)
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

	if (entry->kind == TAGK_HMI_ALIAS) {
		if (for_write && entry->ref.hmi_alias.access == 0U) {
			return NULL;
		}
		if (entry->ref.hmi_alias.target == 0U) {
			return NULL;
		}
		return resolve_entry(rt, entry->ref.hmi_alias.target, for_write, depth + 1);
	}

	if (entry->kind == TAGK_HMI_VAR) {
		if (for_write && entry->ref.hmi_var.access == 0U) {
			return NULL;
		}
	}

	return entry;
}

static size_t tag_type_size(TagType_t type)
{
	switch (type) {
	case TAG_T_BOOL:
		return sizeof(bool);
	case TAG_T_U8:
		return sizeof(uint8_t);
	case TAG_T_U16:
		return sizeof(uint16_t);
	case TAG_T_U32:
		return sizeof(uint32_t);
	case TAG_T_REAL:
		return sizeof(float);
	default:
		return 0U;
	}
}

static ProcValue_t *get_proc_value(Runtime_t *rt, const TagEntry_t *entry)
{
	ProcValue_t *value;

	if (rt == NULL || entry == NULL) {
		return NULL;
	}

	if (entry->ref.proc.index >= rt->proc_store.count) {
		return NULL;
	}

	value = &rt->proc_store.values[entry->ref.proc.index];
	if (value->type != entry->type) {
		return NULL;
	}

	return value;
}

static HmiValue_t *get_hmi_value(Runtime_t *rt, const TagEntry_t *entry)
{
	HmiValue_t *value;

	if (rt == NULL || entry == NULL) {
		return NULL;
	}

	if (entry->ref.hmi_var.index >= rt->hmi_store.count) {
		return NULL;
	}

	value = &rt->hmi_store.values[entry->ref.hmi_var.index];
	if (value->type != entry->type) {
		return NULL;
	}

	return value;
}

static const uint8_t *get_io_read_base(Runtime_t *rt, const TagEntry_t *entry, uint32_t *size_out)
{
	BackendDriver_t *drv;

	if (size_out != NULL) {
		*size_out = 0U;
	}

	if (rt == NULL || entry == NULL || entry->kind != TAGK_IO) {
		return NULL;
	}

	if (rt->backends == NULL || entry->ref.io.backend_index >= rt->backend_count) {
		return NULL;
	}

	drv = rt->backends[entry->ref.io.backend_index];
	if (drv == NULL || drv->ops == NULL) {
		return NULL;
	}

	if (entry->ref.io.dir == TAGDIR_IN) {
		if (drv->ops->get_input_data == NULL) {
			return NULL;
		}
		return drv->ops->get_input_data(drv, entry->ref.io.device_index, size_out);
	}

	if (drv->ops->get_output_data == NULL) {
		return NULL;
	}
	return drv->ops->get_output_data(drv, entry->ref.io.device_index, size_out);
}

static uint8_t *get_io_write_base(Runtime_t *rt, const TagEntry_t *entry, uint32_t *size_out)
{
	BackendDriver_t *drv;

	if (size_out != NULL) {
		*size_out = 0U;
	}

	if (rt == NULL || entry == NULL || entry->kind != TAGK_IO) {
		return NULL;
	}

	if (entry->ref.io.dir != TAGDIR_OUT) {
		return NULL;
	}

	if (rt->backends == NULL || entry->ref.io.backend_index >= rt->backend_count) {
		return NULL;
	}

	drv = rt->backends[entry->ref.io.backend_index];
	if (drv == NULL || drv->ops == NULL || drv->ops->get_output_data == NULL) {
		return NULL;
	}

	return drv->ops->get_output_data(drv, entry->ref.io.device_index, size_out);
}

static int read_io_value(Runtime_t *rt, const TagEntry_t *entry, TagValue_u *out_value)
{
	const uint8_t *base;
	uint32_t size;

	if (out_value == NULL || entry == NULL) {
		return -EINVAL;
	}

	base = get_io_read_base(rt, entry, &size);
	if (base == NULL) {
		return -EINVAL;
	}

	if (entry->type == TAG_T_BOOL) {
		if (entry->ref.io.bit >= 8U || entry->ref.io.byte_offset >= size) {
			return -ERANGE;
		}
		out_value->b = ((base[entry->ref.io.byte_offset] >> entry->ref.io.bit) & 0x1U) != 0U;
		return 0;
	}

	switch (entry->type) {
	case TAG_T_U8:
		if (entry->ref.io.byte_offset + sizeof(uint8_t) > size) {
			return -ERANGE;
		}
		out_value->u8 = base[entry->ref.io.byte_offset];
		return 0;
	case TAG_T_U16:
		if (entry->ref.io.byte_offset + sizeof(uint16_t) > size) {
			return -ERANGE;
		}
		memcpy(&out_value->u16, base + entry->ref.io.byte_offset, sizeof(uint16_t));
		return 0;
	case TAG_T_U32:
		if (entry->ref.io.byte_offset + sizeof(uint32_t) > size) {
			return -ERANGE;
		}
		memcpy(&out_value->u32, base + entry->ref.io.byte_offset, sizeof(uint32_t));
		return 0;
	case TAG_T_REAL:
		if (entry->ref.io.byte_offset + sizeof(float) > size) {
			return -ERANGE;
		}
		memcpy(&out_value->f, base + entry->ref.io.byte_offset, sizeof(float));
		return 0;
	case TAG_T_BOOL:
	default:
		return -EINVAL;
	}
}

static int write_io_value(Runtime_t *rt, const TagEntry_t *entry, TagValue_u value)
{
	uint8_t *base;
	uint32_t size;

	if (entry == NULL) {
		return -EINVAL;
	}

	base = get_io_write_base(rt, entry, &size);
	if (base == NULL) {
		return -EINVAL;
	}

	if (entry->type == TAG_T_BOOL) {
		uint8_t mask;
		if (entry->ref.io.bit >= 8U || entry->ref.io.byte_offset >= size) {
			return -ERANGE;
		}
		mask = (uint8_t)(1U << entry->ref.io.bit);
		if (value.b) {
			base[entry->ref.io.byte_offset] |= mask;
		} else {
			base[entry->ref.io.byte_offset] &= (uint8_t)~mask;
		}
		return 0;
	}

	switch (entry->type) {
	case TAG_T_U8:
		if (entry->ref.io.byte_offset + sizeof(uint8_t) > size) {
			return -ERANGE;
		}
		base[entry->ref.io.byte_offset] = value.u8;
		return 0;
	case TAG_T_U16:
		if (entry->ref.io.byte_offset + sizeof(uint16_t) > size) {
			return -ERANGE;
		}
		memcpy(base + entry->ref.io.byte_offset, &value.u16, sizeof(uint16_t));
		return 0;
	case TAG_T_U32:
		if (entry->ref.io.byte_offset + sizeof(uint32_t) > size) {
			return -ERANGE;
		}
		memcpy(base + entry->ref.io.byte_offset, &value.u32, sizeof(uint32_t));
		return 0;
	case TAG_T_REAL:
		if (entry->ref.io.byte_offset + sizeof(float) > size) {
			return -ERANGE;
		}
		memcpy(base + entry->ref.io.byte_offset, &value.f, sizeof(float));
		return 0;
	case TAG_T_BOOL:
	default:
		return -EINVAL;
	}
}

int tag_read(Runtime_t* rt, TagId_t id, void* out, size_t out_sz)
{
	const TagEntry_t *entry;
	TagValue_u value;
	ProcValue_t *proc;
	size_t type_size;

	if (out == NULL) {
		return -EINVAL;
	}

	entry = resolve_entry(rt, id, 0, 0);
	if (entry == NULL) {
		return -EINVAL;
	}

	type_size = tag_type_size(entry->type);
	if (type_size == 0U) {
		return -EINVAL;
	}

	if (out_sz < type_size) {
		return -ERANGE;
	}

	switch (entry->kind) {
	case TAGK_PROC:
		proc = get_proc_value(rt, entry);
		if (proc == NULL) {
			return -EINVAL;
		}
		value = proc->v;
		break;
	case TAGK_HMI_VAR:
	{
		HmiValue_t *hmi = get_hmi_value(rt, entry);
		if (hmi == NULL) {
			return -EINVAL;
		}
		value = hmi->v;
	}
		break;
	case TAGK_IO:
	{
		int rc = read_io_value(rt, entry, &value);
		if (rc != 0) {
			return rc;
		}
	}
		break;
	default:
		return -EINVAL;
	}

	switch (entry->type) {
	case TAG_T_BOOL:
		memcpy(out, &value.b, sizeof(value.b));
		return 0;
	case TAG_T_U8:
		memcpy(out, &value.u8, sizeof(value.u8));
		return 0;
	case TAG_T_U16:
		memcpy(out, &value.u16, sizeof(value.u16));
		return 0;
	case TAG_T_U32:
		memcpy(out, &value.u32, sizeof(value.u32));
		return 0;
	case TAG_T_REAL:
		memcpy(out, &value.f, sizeof(value.f));
		return 0;
	default:
		return -EINVAL;
	}
}

int tag_write(Runtime_t* rt, TagId_t id, const void* in, size_t in_sz)
{
	const TagEntry_t *entry;
	TagValue_u value;
	ProcValue_t *proc;
	size_t type_size;

	if (in == NULL) {
		return -EINVAL;
	}

	entry = resolve_entry(rt, id, 1, 0);
	if (entry == NULL) {
		return -EINVAL;
	}

	type_size = tag_type_size(entry->type);
	if (type_size == 0U) {
		return -EINVAL;
	}

	if (in_sz < type_size) {
		return -ERANGE;
	}

	switch (entry->type) {
	case TAG_T_BOOL:
		memcpy(&value.b, in, sizeof(value.b));
		break;
	case TAG_T_U8:
		memcpy(&value.u8, in, sizeof(value.u8));
		break;
	case TAG_T_U16:
		memcpy(&value.u16, in, sizeof(value.u16));
		break;
	case TAG_T_U32:
		memcpy(&value.u32, in, sizeof(value.u32));
		break;
	case TAG_T_REAL:
		memcpy(&value.f, in, sizeof(value.f));
		break;
	default:
		return -EINVAL;
	}

	switch (entry->kind) {
	case TAGK_PROC:
		proc = get_proc_value(rt, entry);
		if (proc == NULL) {
			return -EINVAL;
		}
		proc->v = value;
		return 0;
	case TAGK_HMI_VAR:
	{
		HmiValue_t *hmi = get_hmi_value(rt, entry);
		if (hmi == NULL) {
			return -EINVAL;
		}
		hmi->v = value;
		return 0;
	}
	case TAGK_IO:
		return write_io_value(rt, entry, value);
	default:
		return -EINVAL;
	}
}

int tag_read_bool(Runtime_t* rt, TagId_t id, bool *out)
{
	return tag_read(rt, id, out, sizeof(*out));
}

int tag_write_bool(Runtime_t* rt, TagId_t id, bool value)
{
	return tag_write(rt, id, &value, sizeof(value));
}

int tag_read_u8(Runtime_t* rt, TagId_t id, uint8_t *out)
{
	return tag_read(rt, id, out, sizeof(*out));
}

int tag_write_u8(Runtime_t* rt, TagId_t id, uint8_t value)
{
	return tag_write(rt, id, &value, sizeof(value));
}

int tag_read_u16(Runtime_t* rt, TagId_t id, uint16_t *out)
{
	return tag_read(rt, id, out, sizeof(*out));
}

int tag_write_u16(Runtime_t* rt, TagId_t id, uint16_t value)
{
	return tag_write(rt, id, &value, sizeof(value));
}

int tag_read_u32(Runtime_t* rt, TagId_t id, uint32_t *out)
{
	return tag_read(rt, id, out, sizeof(*out));
}

int tag_write_u32(Runtime_t* rt, TagId_t id, uint32_t value)
{
	return tag_write(rt, id, &value, sizeof(value));
}

int tag_read_real(Runtime_t* rt, TagId_t id, float *out)
{
	return tag_read(rt, id, out, sizeof(*out));
}

int tag_write_real(Runtime_t* rt, TagId_t id, float value)
{
	return tag_write(rt, id, &value, sizeof(value));
}
