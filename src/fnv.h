/// @file fnv.h
/// @author namazso
/// @date 2017-12-04
/// @brief FNV-1a hashing algorithm

#pragma once

typedef uint64_t fnv_t;

/// Initialize a fnv hash
static inline void fnv_init(fnv_t* fnv)
{
	*fnv = 0xcbf29ce484222325;
}

/// Hash data
static inline void fnv_hash(fnv_t* fnv, const void* data,
	const int size)
{
	fnv_t hash = *fnv;
	for(int i = 0; i < size; ++i)
	{
		hash = hash ^ ((const uint8_t*)data)[i];
		hash = hash * 1099511628211;
	}
	*fnv = hash;
}