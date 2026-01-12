#include "tag_rpc_plc.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/tag/tag_api.h"
#include "core/tag/tag_table.h"

static const char *tag_type_to_string(TagType_t type)
{
	switch (type)
	{
	case TAG_T_BOOL:
		return "BOOL";
	case TAG_T_U8:
		return "U8";
	case TAG_T_U16:
		return "U16";
	case TAG_T_U32:
		return "U32";
	case TAG_T_REAL:
		return "REAL";
	default:
		return "UNKNOWN";
	}
}

static bool str_eq_nocase(const char *a, const char *b)
{
	while (*a != '\0' && *b != '\0')
	{
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
		{
			return false;
		}
		++a;
		++b;
	}
	return *a == '\0' && *b == '\0';
}

static int parse_bool_value(const char *value, bool *out)
{
	if (value == NULL || out == NULL)
	{
		return -EINVAL;
	}

	if (strcmp(value, "0") == 0)
	{
		*out = false;
		return 0;
	}
	if (strcmp(value, "1") == 0)
	{
		*out = true;
		return 0;
	}
	if (str_eq_nocase(value, "true"))
	{
		*out = true;
		return 0;
	}
	if (str_eq_nocase(value, "false"))
	{
		*out = false;
		return 0;
	}

	return -EINVAL;
}

static void set_error_response(TagRpcResponse_t *rsp, uint32_t id, int status, const char *msg)
{
	memset(rsp, 0, sizeof(*rsp));
	rsp->id = id;
	rsp->status = status;
	if (msg != NULL)
	{
		snprintf(rsp->msg, sizeof(rsp->msg), "%s", msg);
	}
}

static void handle_read(TagRpcResponse_t *rsp, Runtime_t *rt, const TagEntry_t *entry, uint32_t req_id, TagId_t tag_id)
{
	int rc = -1;
	memset(rsp, 0, sizeof(*rsp));
	rsp->id = req_id;

	switch (entry->type)
	{
	case TAG_T_BOOL:
	{
		bool v = false;
		rc = tag_read_bool(rt, tag_id, &v);
		if (rc == 0)
		{
			snprintf(rsp->type, sizeof(rsp->type), "%s", tag_type_to_string(entry->type));
			snprintf(rsp->value, sizeof(rsp->value), "%s", v ? "1" : "0");
		}
		break;
	}
	case TAG_T_U8:
	{
		uint8_t v = 0;
		rc = tag_read_u8(rt, tag_id, &v);
		if (rc == 0)
		{
			snprintf(rsp->type, sizeof(rsp->type), "%s", tag_type_to_string(entry->type));
			snprintf(rsp->value, sizeof(rsp->value), "%u", (unsigned)v);
		}
		break;
	}
	case TAG_T_U16:
	{
		uint16_t v = 0;
		rc = tag_read_u16(rt, tag_id, &v);
		if (rc == 0)
		{
			snprintf(rsp->type, sizeof(rsp->type), "%s", tag_type_to_string(entry->type));
			snprintf(rsp->value, sizeof(rsp->value), "%u", (unsigned)v);
		}
		break;
	}
	case TAG_T_U32:
	{
		uint32_t v = 0;
		rc = tag_read_u32(rt, tag_id, &v);
		if (rc == 0)
		{
			snprintf(rsp->type, sizeof(rsp->type), "%s", tag_type_to_string(entry->type));
			snprintf(rsp->value, sizeof(rsp->value), "%u", (unsigned)v);
		}
		break;
	}
	case TAG_T_REAL:
	{
		float v = 0.0f;
		rc = tag_read_real(rt, tag_id, &v);
		if (rc == 0)
		{
			snprintf(rsp->type, sizeof(rsp->type), "%s", tag_type_to_string(entry->type));
			snprintf(rsp->value, sizeof(rsp->value), "%f", v);
		}
		break;
	}
	default:
		rc = -EINVAL;
		break;
	}

	if (rc != 0)
	{
		set_error_response(rsp, req_id, rc, "read_failed");
	}
}

static void handle_write(TagRpcResponse_t *rsp, Runtime_t *rt, const TagEntry_t *entry, uint32_t req_id, TagId_t tag_id, const char *value)
{
	int rc = -1;
	memset(rsp, 0, sizeof(*rsp));
	rsp->id = req_id;

	switch (entry->type)
	{
	case TAG_T_BOOL:
	{
		bool v = false;
		rc = parse_bool_value(value, &v);
		if (rc == 0)
		{
			rc = tag_write_bool(rt, tag_id, v);
		}
		break;
	}
	case TAG_T_U8:
	{
		char *endp = NULL;
		unsigned long v = strtoul(value, &endp, 0);
		if (endp == NULL || *endp != '\0')
		{
			rc = -EINVAL;
		}
		else if (v > 0xFFUL)
		{
			rc = -ERANGE;
		}
		else
		{
			rc = tag_write_u8(rt, tag_id, (uint8_t)v);
		}
		break;
	}
	case TAG_T_U16:
	{
		char *endp = NULL;
		unsigned long v = strtoul(value, &endp, 0);
		if (endp == NULL || *endp != '\0')
		{
			rc = -EINVAL;
		}
		else if (v > 0xFFFFUL)
		{
			rc = -ERANGE;
		}
		else
		{
			rc = tag_write_u16(rt, tag_id, (uint16_t)v);
		}
		break;
	}
	case TAG_T_U32:
	{
		char *endp = NULL;
		unsigned long v = strtoul(value, &endp, 0);
		if (endp == NULL || *endp != '\0')
		{
			rc = -EINVAL;
		}
		else
		{
			rc = tag_write_u32(rt, tag_id, (uint32_t)v);
		}
		break;
	}
	case TAG_T_REAL:
	{
		char *endp = NULL;
		float v = strtof(value, &endp);
		if (endp == NULL || *endp != '\0')
		{
			rc = -EINVAL;
		}
		else
		{
			rc = tag_write_real(rt, tag_id, v);
		}
		break;
	}
	default:
		rc = -EINVAL;
		break;
	}

	if (rc != 0)
	{
		set_error_response(rsp, req_id, rc, "write_failed");
	}
}

void tag_rpc_plc_poll(TagRpcChannel_t* ch, Runtime_t* rt, uint32_t max_per_cycle)
{
	uint32_t handled = 0U;
	TagRpcRequest_t req;

	if (ch == NULL || rt == NULL)
	{
		return;
	}

	while (handled < max_per_cycle)
	{
		TagRpcResponse_t rsp;
		TagId_t id;
		const TagEntry_t *entry;

		if (!tag_rpc_recv_request(ch, &req))
		{
			break;
		}

		id = tag_table_find_id(&rt->tag_table, req.name);
		if (id == 0U)
		{
			set_error_response(&rsp, req.id, -ENOENT, "tag_not_found");
			(void)tag_rpc_send_response(ch, &rsp);
			handled++;
			continue;
		}

		entry = tag_table_get(&rt->tag_table, id);
		if (entry == NULL)
		{
			set_error_response(&rsp, req.id, -ENOENT, "tag_not_found");
			(void)tag_rpc_send_response(ch, &rsp);
			handled++;
			continue;
		}

		if (req.op == TAGRPC_READ)
		{
			handle_read(&rsp, rt, entry, req.id, id);
		}
		else if (req.op == TAGRPC_WRITE)
		{
			handle_write(&rsp, rt, entry, req.id, id, req.value);
		}
		else
		{
			set_error_response(&rsp, req.id, -EINVAL, "bad_op");
		}

		(void)tag_rpc_send_response(ch, &rsp);
		handled++;
	}
}
