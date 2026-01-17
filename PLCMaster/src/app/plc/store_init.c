/* PLC runtime store initialization helpers. */

#include "app/plc/store_init.h"

#include <stdio.h>

static int proc_apply_initial(const ProcessVarDesc_t *desc, ProcValue_t *value_slot)
{
	if (desc == NULL || value_slot == NULL) {
		return -1;
	}

	value_slot->type = desc->type;
	switch (desc->type) {
	case TAG_T_BOOL:
		value_slot->v.b = desc->initial.b;
		return 0;
	case TAG_T_U8:
		value_slot->v.u8 = desc->initial.u8;
		return 0;
	case TAG_T_U16:
		value_slot->v.u16 = desc->initial.u16;
		return 0;
	case TAG_T_U32:
		value_slot->v.u32 = desc->initial.u32;
		return 0;
	case TAG_T_REAL:
		value_slot->v.f = desc->initial.f;
		return 0;
	default:
		return -1;
	}
}

static int hmi_apply_initial(const HmiTagDesc_t *desc, HmiValue_t *value_slot)
{
	if (desc == NULL || value_slot == NULL) {
		return -1;
	}

	value_slot->type = desc->var.type;
	switch (desc->var.type) {
	case TAG_T_BOOL:
		value_slot->v.b = desc->var.initial.b;
		return 0;
	case TAG_T_U8:
		value_slot->v.u8 = desc->var.initial.u8;
		return 0;
	case TAG_T_U16:
		value_slot->v.u16 = desc->var.initial.u16;
		return 0;
	case TAG_T_U32:
		value_slot->v.u32 = desc->var.initial.u32;
		return 0;
	case TAG_T_REAL:
		value_slot->v.f = desc->var.initial.f;
		return 0;
	default:
		return -1;
	}
}

int proc_load_retained(Runtime_t *rt, const ProcessVarDesc_t *descs, size_t count)
{
	(void)rt;
	(void)descs;
	(void)count;
	return 0;
}

int proc_save_retained(Runtime_t *rt, const ProcessVarDesc_t *descs, size_t count)
{
	(void)rt;
	(void)descs;
	(void)count;
	return 0;
}

int proc_erase_retained_storage(void)
{
	return 0;
}

int hmi_load_retained(Runtime_t *rt, const HmiTagDesc_t *descs, size_t count)
{
	(void)rt;
	(void)descs;
	(void)count;
	return 0;
}

int hmi_save_retained(Runtime_t *rt, const HmiTagDesc_t *descs, size_t count)
{
	(void)rt;
	(void)descs;
	(void)count;
	return 0;
}

int hmi_erase_retained_storage(void)
{
	return 0;
}

int proc_init(Runtime_t *rt, const ProcessVarDesc_t *descs, size_t count, StartMode_t mode)
{
	size_t index;

	if (rt == NULL) {
		return -1;
	}

	if (count > 0 && descs == NULL) {
		return -1;
	}

	if (mode == START_HMI_INIT) {
		return 0;
	}

	(void)proc_load_retained(rt, descs, count);

	for (index = 0; index < count; ++index) {
		const ProcessVarDesc_t *desc = &descs[index];
		char full_name[128];
		TagId_t id;
		const TagEntry_t *entry;
		ProcValue_t *value_slot;
		int rc;

		rc = snprintf(full_name, sizeof(full_name), "proc.%s", desc->name);
		if (rc < 0 || (size_t)rc >= sizeof(full_name)) {
			printf("[INIT] proc tag name too long: %s\n", desc->name);
			continue;
		}

		id = tag_table_find_id(&rt->tag_table, full_name);
		if (id == 0) {
			printf("[INIT] proc tag not found: %s\n", full_name);
			continue;
		}

		entry = tag_table_get(&rt->tag_table, id);
		if (entry == NULL || entry->kind != TAGK_PROC) {
			printf("[INIT] proc tag mismatch: %s\n", full_name);
			continue;
		}

		if (entry->ref.proc.index >= rt->proc_store.count) {
			printf("[INIT] proc store index out of range: %s\n", full_name);
			continue;
		}

		value_slot = &rt->proc_store.values[entry->ref.proc.index];
		if (proc_apply_initial(desc, value_slot) != 0) {
			printf("[INIT] proc tag invalid type: %s\n", full_name);
		}
	}

	return 0;
}

int hmi_init(Runtime_t *rt, const HmiTagDesc_t *descs, size_t count, StartMode_t mode)
{
	size_t index;

	if (rt == NULL) {
		return -1;
	}

	if (count > 0 && descs == NULL) {
		return -1;
	}

	if (mode == START_COLD) {
		return 0;
	}

	(void)hmi_load_retained(rt, descs, count);

	for (index = 0; index < count; ++index) {
		const HmiTagDesc_t *desc = &descs[index];
		char full_name[128];
		TagId_t id;
		const TagEntry_t *entry;
		HmiValue_t *value_slot;
		int rc;

		if (desc->kind != HMI_TAG_VAR) {
			continue;
		}

		rc = snprintf(full_name, sizeof(full_name), "hmi.%s", desc->name);
		if (rc < 0 || (size_t)rc >= sizeof(full_name)) {
			printf("[INIT] hmi tag name too long: %s\n", desc->name);
			continue;
		}

		id = tag_table_find_id(&rt->tag_table, full_name);
		if (id == 0) {
			printf("[INIT] hmi tag not found: %s\n", full_name);
			continue;
		}

		entry = tag_table_get(&rt->tag_table, id);
		if (entry == NULL || entry->kind != TAGK_HMI_VAR) {
			printf("[INIT] hmi tag mismatch: %s\n", full_name);
			continue;
		}

		if (entry->type != desc->var.type) {
			printf("[INIT] hmi tag type mismatch: %s\n", full_name);
			continue;
		}

		if (entry->ref.hmi_var.index >= rt->hmi_store.count) {
			printf("[INIT] hmi store index out of range: %s\n", full_name);
			continue;
		}

		value_slot = &rt->hmi_store.values[entry->ref.hmi_var.index];
		if (hmi_apply_initial(desc, value_slot) != 0) {
			printf("[INIT] hmi tag invalid type: %s\n", full_name);
		}
	}

	return 0;
}
