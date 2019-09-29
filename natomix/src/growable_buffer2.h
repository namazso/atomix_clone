/// @file growable_buffer2.h
/// @author namazso
/// @date 2017-10-10
/// @brief A buffer that can be grown.
///
/// Contains header only implementation of a buffer that can be grown,
/// and automatically reallocates and grows memory.

#pragma once

#define DEFINE_GROWABLE_BUFFER(Type, Name) \
\
/*! @addtogroup growable_buffer */\
/*! @{ */\
\
/*! A buffer that can be grown. */\
struct Name\
{\
	/*! Allocated memory.*/\
	Type* mem;\
\
	/*! Allocated element count.*/\
	/*!*/\
	/*! Grows by the factor of two every time it*/\
	/*! needs more space.*/\
	int alloc; \
\
	/*! Amount of actual elements in the buffer.*/\
	int size; \
};\
\
/*! Initialize a growable_buffer.*/\
/*!*/\
/*! Initializes a buffer with zero allocated items and no allocated memory.*/\
/*!*/\
/*! @param[out] ctx Context to initialize */\
/*! @param[in] allocsize Size of one element*/\
inline void Name ## _init(struct Name* ctx)\
{\
	ctx->mem = NULL;\
	ctx->alloc = 0;\
	ctx->size = 0;\
}\
\
/*! Resizes a growable_buffer.*/\
/*!*/\
/*! Resizes the buffer, allocates and copies memory if needed.*/\
/*! Can only be used to grow the buffer.*/\
/*!*/\
/*! @param[in,out] ctx The buffer to resize.*/\
/*! @param[in] new_size New size.*/\
/*! @return The starting index of the newly available space.*/\
inline int Name ## _resize(struct Name* ctx,\
	int new_size)\
{\
	assert(new_size >= ctx->size);\
	int alloc = ctx->alloc;\
	if(alloc < new_size)\
	{\
		if(alloc == 0)\
			alloc = 1;\
\
		while(alloc < new_size)\
			alloc *= 2;\
\
		void* new_mem = malloc(alloc * sizeof(Type));\
		assert(new_mem);\
		void* old_mem = ctx->mem;\
		memcpy(new_mem, old_mem, sizeof(Type) * ctx->size);\
		ctx->mem = (Type*)new_mem;\
		if(old_mem)\
			free(old_mem);\
		ctx->alloc = alloc;\
	}\
	const int oldsize = ctx->size;\
	ctx->size = new_size;\
	return oldsize;\
}\
\
/*! Grow the buffer by \p plus count of elements*/\
/*!*/\
/*! Resizes the buffer, allocates and copies memory if needed.*/\
/*!*/\
/*! @param[in,out] ctx Buffer to grow.*/\
/*! @param[in] plus Count of elements to grow the buffer with.*/\
/*! @return The starting index of the newly available space.*/\
inline int Name ## _grow(struct Name* ctx, int plus)\
{\
	return Name ## _resize(ctx, ctx->size + plus);\
}\
\
/*! Push a new element to the back of the buffer.*/\
/*!*/\
/*! Grows the buffer by one, and copies the given object to the newly*/\
/*! available space.*/\
/*!*/\
/*! @param[in,out] ctx Buffer to push the element to.*/\
/*! @param[in] elem Element to push on to back of the buffer.*/\
/*! @return Index of the newly added element.*/\
inline int Name ## _push(struct Name* ctx,\
	const Type* elem)\
{\
	const int new_index = Name ## _grow(ctx, 1);\
	ctx->mem[new_index] = *elem;\
	return new_index;\
}\
\
/*! Free the buffer.*/\
/*!*/\
/*! Frees the allocated memory, and optionally runs a destructor on*/\
/*! each element of the buffer.*/\
/*!*/\
/*! @param[in,out] ctx Buffer to free.*/\
/*! @param[in] destructor Function to run on each element of the*/\
/*!                       buffer, NULL if disabled.*/\
inline int Name ## _free(struct Name* ctx,\
	void(*destructor)(Type*))\
{\
	if(destructor)\
		for(int i = 0; i < ctx->size; ++i)\
			destructor(&ctx->mem[i]);\
	free(ctx->mem);\
	ctx->mem = NULL;\
	ctx->alloc = 0;\
	ctx->size = 0;\
}\
\
/*! @} */
