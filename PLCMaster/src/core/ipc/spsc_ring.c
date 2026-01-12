#include "spsc_ring.h"

#include <string.h>

int spsc_ring_init(SpscRing_t* q, void* storage, size_t elem_size, size_t capacity)
{
	if (q == NULL || storage == NULL || elem_size == 0U || capacity == 0U)
	{
		return -1;
	}

	q->storage = (uint8_t *)storage;
	q->elem_size = elem_size;
	q->capacity = (uint32_t)capacity;
	plat_atomic_store_u32(&q->read_index, 0U);
	plat_atomic_store_u32(&q->write_index, 0U);
	return 0;
}

bool spsc_ring_push(SpscRing_t* q, const void* elem)
{
	uint32_t read_index;
	uint32_t write_index;
	uint32_t slot;

	if (q == NULL || elem == NULL)
	{
		return false;
	}

	read_index = plat_atomic_load_u32(&q->read_index);
	write_index = plat_atomic_load_u32(&q->write_index);

	if ((write_index - read_index) >= q->capacity)
	{
		return false;
	}

	slot = write_index % q->capacity;
	memcpy(q->storage + (size_t)slot * q->elem_size, elem, q->elem_size);
	plat_atomic_store_u32(&q->write_index, write_index + 1U);
	return true;
}

bool spsc_ring_pop(SpscRing_t* q, void* out_elem)
{
	uint32_t read_index;
	uint32_t write_index;
	uint32_t slot;

	if (q == NULL || out_elem == NULL)
	{
		return false;
	}

	read_index = plat_atomic_load_u32(&q->read_index);
	write_index = plat_atomic_load_u32(&q->write_index);

	if (read_index == write_index)
	{
		return false;
	}

	slot = read_index % q->capacity;
	memcpy(out_elem, q->storage + (size_t)slot * q->elem_size, q->elem_size);
	plat_atomic_store_u32(&q->read_index, read_index + 1U);
	return true;
}
