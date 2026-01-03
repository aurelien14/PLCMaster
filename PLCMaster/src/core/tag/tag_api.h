/* Tag API declarations for managing tag interactions. TODO: define actual interface. */

#ifndef TAG_API_H
#define TAG_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tag_table.h"

int tag_read_bool(TagId_t id, bool *out);
int tag_write_bool(TagId_t id, bool v);
int tag_read_u8(TagId_t id, uint8_t *out);
int tag_write_u8(TagId_t id, uint8_t v);
int tag_read_u16(TagId_t id, uint16_t *out);
int tag_write_u16(TagId_t id, uint16_t v);
int tag_read_u32(TagId_t id, uint32_t *out);
int tag_write_u32(TagId_t id, uint32_t v);
int tag_read_real(TagId_t id, float *out);
int tag_write_real(TagId_t id, float v);
int tag_api_bind_proc_storage(const TagTable_t *t, uint8_t *buffer, size_t size);

#endif /* TAG_API_H */
