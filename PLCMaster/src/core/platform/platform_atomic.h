#ifndef PLATFORM_ATOMIC_H
#define PLATFORM_ATOMIC_H

#include "platform_detect.h"

#if PLAT_WINDOWS
#include <windows.h>

typedef volatile LONG plat_atomic_i32_t;
typedef volatile LONG plat_atomic_u32_t;
typedef volatile LONGLONG plat_atomic_i64_t;
typedef volatile ULONGLONG plat_atomic_u64_t;
typedef volatile LONG plat_atomic_bool_t;

/* WinAPI Interlocked* functions act as full memory barriers. */

#define PLAT_ATOMIC_I32_INIT(x) (x)
#define PLAT_ATOMIC_U32_INIT(x) (x)
#define PLAT_ATOMIC_I64_INIT(x) (x)
#define PLAT_ATOMIC_U64_INIT(x) (x)
#define PLAT_ATOMIC_BOOL_INIT(x) (x)

PLAT_INLINE void plat_atomic_store_i32(plat_atomic_i32_t *v, int32_t x)
{
	InterlockedExchange(v, (LONG)x);
}

PLAT_INLINE int32_t plat_atomic_load_i32(const plat_atomic_i32_t *v)
{
	return (int32_t)InterlockedCompareExchange((LONG *)v, 0, 0);
}

PLAT_INLINE int32_t plat_atomic_exchange_i32(plat_atomic_i32_t *v, int32_t x)
{
	return (int32_t)InterlockedExchange(v, (LONG)x);
}

PLAT_INLINE bool plat_atomic_cas_i32(plat_atomic_i32_t *v, int32_t expected, int32_t desired)
{
	return InterlockedCompareExchange((LONG *)v, (LONG)desired, (LONG)expected) == (LONG)expected;
}

PLAT_INLINE int32_t plat_atomic_fetch_add_i32(plat_atomic_i32_t *v, int32_t x)
{
	return (int32_t)InterlockedExchangeAdd(v, (LONG)x);
}

PLAT_INLINE void plat_atomic_store_u32(plat_atomic_u32_t *v, uint32_t x)
{
	InterlockedExchange((LONG *)v, (LONG)x);
}

PLAT_INLINE uint32_t plat_atomic_load_u32(const plat_atomic_u32_t *v)
{
	return (uint32_t)InterlockedCompareExchange((LONG *)v, 0, 0);
}

PLAT_INLINE uint32_t plat_atomic_exchange_u32(plat_atomic_u32_t *v, uint32_t x)
{
	return (uint32_t)InterlockedExchange((LONG *)v, (LONG)x);
}

PLAT_INLINE bool plat_atomic_cas_u32(plat_atomic_u32_t *v, uint32_t expected, uint32_t desired)
{
	return InterlockedCompareExchange((LONG *)v, (LONG)desired, (LONG)expected) == (LONG)expected;
}

PLAT_INLINE uint32_t plat_atomic_fetch_add_u32(plat_atomic_u32_t *v, uint32_t x)
{
	return (uint32_t)InterlockedExchangeAdd((LONG *)v, (LONG)x);
}

PLAT_INLINE void plat_atomic_store_i64(plat_atomic_i64_t *v, int64_t x)
{
	InterlockedExchange64(v, (LONGLONG)x);
}

PLAT_INLINE int64_t plat_atomic_load_i64(const plat_atomic_i64_t *v)
{
	return (int64_t)InterlockedCompareExchange64(v, 0, 0);
}

PLAT_INLINE int64_t plat_atomic_exchange_i64(plat_atomic_i64_t *v, int64_t x)
{
	return (int64_t)InterlockedExchange64(v, (LONGLONG)x);
}

PLAT_INLINE bool plat_atomic_cas_i64(plat_atomic_i64_t *v, int64_t expected, int64_t desired)
{
	return InterlockedCompareExchange64(v, (LONGLONG)desired, (LONGLONG)expected) == (LONGLONG)expected;
}

PLAT_INLINE int64_t plat_atomic_fetch_add_i64(plat_atomic_i64_t *v, int64_t x)
{
	return (int64_t)InterlockedExchangeAdd64(v, (LONGLONG)x);
}

PLAT_INLINE void plat_atomic_store_u64(plat_atomic_u64_t *v, uint64_t x)
{
	InterlockedExchange64((LONGLONG *)v, (LONGLONG)x);
}

PLAT_INLINE uint64_t plat_atomic_load_u64(const plat_atomic_u64_t *v)
{
	return (uint64_t)InterlockedCompareExchange64((LONGLONG *)v, 0, 0);
}

PLAT_INLINE uint64_t plat_atomic_exchange_u64(plat_atomic_u64_t *v, uint64_t x)
{
	return (uint64_t)InterlockedExchange64((LONGLONG *)v, (LONGLONG)x);
}

PLAT_INLINE bool plat_atomic_cas_u64(plat_atomic_u64_t *v, uint64_t expected, uint64_t desired)
{
	return InterlockedCompareExchange64((LONGLONG *)v, (LONGLONG)desired, (LONGLONG)expected) == (LONGLONG)expected;
}

PLAT_INLINE uint64_t plat_atomic_fetch_add_u64(plat_atomic_u64_t *v, uint64_t x)
{
	return (uint64_t)InterlockedExchangeAdd64((LONGLONG *)v, (LONGLONG)x);
}

PLAT_INLINE void plat_atomic_store_bool(plat_atomic_bool_t *v, bool x)
{
	InterlockedExchange(v, x ? 1 : 0);
}

PLAT_INLINE bool plat_atomic_load_bool(const plat_atomic_bool_t *v)
{
	return InterlockedCompareExchange((LONG *)v, 0, 0) != 0;
}

PLAT_INLINE bool plat_atomic_exchange_bool(plat_atomic_bool_t *v, bool x)
{
	return InterlockedExchange(v, x ? 1 : 0) != 0;
}

PLAT_INLINE bool plat_atomic_cas_bool(plat_atomic_bool_t *v, bool expected, bool desired)
{
	LONG exp_val = expected ? 1 : 0;
	LONG desired_val = desired ? 1 : 0;
	return InterlockedCompareExchange(v, desired_val, exp_val) == exp_val;
}

PLAT_INLINE bool plat_atomic_fetch_add_bool(plat_atomic_bool_t *v, bool x)
{
	return InterlockedExchangeAdd(v, x ? 1 : 0) != 0;
}

#elif PLAT_LINUX
#include <stdatomic.h>

typedef _Atomic int32_t plat_atomic_i32_t;
typedef _Atomic uint32_t plat_atomic_u32_t;
typedef _Atomic int64_t plat_atomic_i64_t;
typedef _Atomic uint64_t plat_atomic_u64_t;
typedef _Atomic bool plat_atomic_bool_t;

#define PLAT_ATOMIC_I32_INIT(x) ATOMIC_VAR_INIT(x)
#define PLAT_ATOMIC_U32_INIT(x) ATOMIC_VAR_INIT(x)
#define PLAT_ATOMIC_I64_INIT(x) ATOMIC_VAR_INIT(x)
#define PLAT_ATOMIC_U64_INIT(x) ATOMIC_VAR_INIT(x)
#define PLAT_ATOMIC_BOOL_INIT(x) ATOMIC_VAR_INIT(x)

PLAT_INLINE void plat_atomic_store_i32(plat_atomic_i32_t *v, int32_t x)
{
	atomic_store_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE int32_t plat_atomic_load_i32(const plat_atomic_i32_t *v)
{
	return atomic_load_explicit(v, memory_order_seq_cst);
}

PLAT_INLINE int32_t plat_atomic_exchange_i32(plat_atomic_i32_t *v, int32_t x)
{
	return atomic_exchange_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_cas_i32(plat_atomic_i32_t *v, int32_t expected, int32_t desired)
{
	return atomic_compare_exchange_strong_explicit(v, &expected, desired, memory_order_seq_cst, memory_order_seq_cst);
}

PLAT_INLINE int32_t plat_atomic_fetch_add_i32(plat_atomic_i32_t *v, int32_t x)
{
	return atomic_fetch_add_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE void plat_atomic_store_u32(plat_atomic_u32_t *v, uint32_t x)
{
	atomic_store_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE uint32_t plat_atomic_load_u32(const plat_atomic_u32_t *v)
{
	return atomic_load_explicit(v, memory_order_seq_cst);
}

PLAT_INLINE uint32_t plat_atomic_exchange_u32(plat_atomic_u32_t *v, uint32_t x)
{
	return atomic_exchange_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_cas_u32(plat_atomic_u32_t *v, uint32_t expected, uint32_t desired)
{
	return atomic_compare_exchange_strong_explicit(v, &expected, desired, memory_order_seq_cst, memory_order_seq_cst);
}

PLAT_INLINE uint32_t plat_atomic_fetch_add_u32(plat_atomic_u32_t *v, uint32_t x)
{
	return atomic_fetch_add_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE void plat_atomic_store_i64(plat_atomic_i64_t *v, int64_t x)
{
	atomic_store_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE int64_t plat_atomic_load_i64(const plat_atomic_i64_t *v)
{
	return atomic_load_explicit(v, memory_order_seq_cst);
}

PLAT_INLINE int64_t plat_atomic_exchange_i64(plat_atomic_i64_t *v, int64_t x)
{
	return atomic_exchange_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_cas_i64(plat_atomic_i64_t *v, int64_t expected, int64_t desired)
{
	return atomic_compare_exchange_strong_explicit(v, &expected, desired, memory_order_seq_cst, memory_order_seq_cst);
}

PLAT_INLINE int64_t plat_atomic_fetch_add_i64(plat_atomic_i64_t *v, int64_t x)
{
	return atomic_fetch_add_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE void plat_atomic_store_u64(plat_atomic_u64_t *v, uint64_t x)
{
	atomic_store_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE uint64_t plat_atomic_load_u64(const plat_atomic_u64_t *v)
{
	return atomic_load_explicit(v, memory_order_seq_cst);
}

PLAT_INLINE uint64_t plat_atomic_exchange_u64(plat_atomic_u64_t *v, uint64_t x)
{
	return atomic_exchange_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_cas_u64(plat_atomic_u64_t *v, uint64_t expected, uint64_t desired)
{
	return atomic_compare_exchange_strong_explicit(v, &expected, desired, memory_order_seq_cst, memory_order_seq_cst);
}

PLAT_INLINE uint64_t plat_atomic_fetch_add_u64(plat_atomic_u64_t *v, uint64_t x)
{
	return atomic_fetch_add_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE void plat_atomic_store_bool(plat_atomic_bool_t *v, bool x)
{
	atomic_store_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_load_bool(const plat_atomic_bool_t *v)
{
	return atomic_load_explicit(v, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_exchange_bool(plat_atomic_bool_t *v, bool x)
{
	return atomic_exchange_explicit(v, x, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_cas_bool(plat_atomic_bool_t *v, bool expected, bool desired)
{
	return atomic_compare_exchange_strong_explicit(v, &expected, desired, memory_order_seq_cst, memory_order_seq_cst);
}

PLAT_INLINE bool plat_atomic_fetch_add_bool(plat_atomic_bool_t *v, bool x)
{
	return atomic_fetch_add_explicit(v, x, memory_order_seq_cst);
}

#else
#error "Unsupported platform for atomics"
#endif

#endif /* PLATFORM_ATOMIC_H */
