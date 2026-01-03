/* Generic device descriptor declarations. */

#ifndef DEVICE_DESC_H
#define DEVICE_DESC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum TagType_t {
    TAG_T_BOOL = 0,
    TAG_T_U8,
    TAG_T_U16,
    TAG_T_U32,
    TAG_T_REAL
} TagType_t;

typedef enum TagDir_t {
    TAG_DIR_IN = 0,
    TAG_DIR_OUT
} TagDir_t;

typedef struct TagDesc_t {
    const char *name;
    TagDir_t dir;
    TagType_t type;
    uint32_t offset_byte;
    uint8_t bit_index;
    const char *iec_alias;
    const char *comment;
} TagDesc_t;

typedef struct DeviceHooks_t {
    void (*on_init)(void *context);
    void (*on_update)(void *context);
    void (*on_shutdown)(void *context);
} DeviceHooks_t;

typedef struct DeviceDesc_t {
    const char *model;
    uint32_t vendor_id;
    uint32_t product_code;
    uint32_t rx_bytes;
    uint32_t tx_bytes;
    const TagDesc_t *tags;
    uint32_t tag_count;
    DeviceHooks_t hooks;
} DeviceDesc_t;

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_DESC_H */
