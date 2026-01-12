#ifndef SPSC_RING_H
#define SPSC_RING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/platform/platform_atomic.h"

typedef struct
{
	uint8_t* storage;
	size_t elem_size;
	uint32_t capacity;
	plat_atomic_u32_t read_index;
	plat_atomic_u32_t write_index;
} SpscRing_t;

int spsc_ring_init(SpscRing_t* q, void* storage, size_t elem_size, size_t capacity);
bool spsc_ring_push(SpscRing_t* q, const void* elem);
bool spsc_ring_pop(SpscRing_t* q, void* out_elem);

#endif /* SPSC_RING_H */
