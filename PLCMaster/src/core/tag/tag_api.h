/* Tag API declarations for managing tag interactions. TODO: define actual interface. */

#ifndef TAG_API_H
#define TAG_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/runtime/runtime.h"

int tag_read(Runtime_t* rt, TagId_t id, void* out, size_t out_sz);
int tag_write(Runtime_t* rt, TagId_t id, const void* in, size_t in_sz);
int tag_read_bool(Runtime_t* rt, TagId_t id, bool *out);
int tag_write_bool(Runtime_t* rt, TagId_t id, bool v);
int tag_read_u8(Runtime_t* rt, TagId_t id, uint8_t *out);
int tag_write_u8(Runtime_t* rt, TagId_t id, uint8_t v);
int tag_read_u16(Runtime_t* rt, TagId_t id, uint16_t *out);
int tag_write_u16(Runtime_t* rt, TagId_t id, uint16_t v);
int tag_read_u32(Runtime_t* rt, TagId_t id, uint32_t *out);
int tag_write_u32(Runtime_t* rt, TagId_t id, uint32_t v);
int tag_read_real(Runtime_t* rt, TagId_t id, float *out);
int tag_write_real(Runtime_t* rt, TagId_t id, float v);

#endif /* TAG_API_H */
