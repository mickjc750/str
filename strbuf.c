/*
*/
	#include <stdint.h>
	#include <ctype.h>
	#include "str.h"
	#include "strbuf.h"

	#ifdef STRBUF_PROVIDE_PRINTF
		#include <stdio.h>
	#endif

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//	#include <stdio.h>
//	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static strbuf_t* create_buf(size_t initial_capacity, str_allocator_t allocator);
	static str_t buffer_vcat(strbuf_t** buf_ptr, int n_args, va_list va);
	static void insert_str_into_buf(strbuf_t** buf_ptr, int index, str_t str);
	static void destroy_buf(strbuf_t** buf_ptr);
	static void change_buf_capacity(strbuf_t** buf_ptr, size_t new_capacity);
	static void assign_str_to_buf(strbuf_t** buf_ptr, str_t str);
	static void append_char_to_buf(strbuf_t** strbuf, char c);
	static size_t round_up_capacity(size_t capacity);
	static str_t str_of_buf(strbuf_t* buf);
	static bool buf_contains_str(strbuf_t* buf, str_t str);
	static bool buf_is_dynamic(strbuf_t* buf);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strbuf_t* strbuf_create(size_t initial_capacity, str_allocator_t allocator)
{
	strbuf_t* result;
	if(allocator.allocator)
		result = create_buf(initial_capacity, allocator);
	else
		result = NULL;
	return result;
}

strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size)
{
	strbuf_t* result = NULL;
	intptr_t alignment_mask;

	if(addr_size > sizeof(strbuf_t) && addr)
	{
		//check alignment
		alignment_mask = sizeof(void*)-1;
		alignment_mask &= (intptr_t)addr;
		if(alignment_mask == 0)
		{
			result = addr;
			result->allocator.app_data = NULL;
			result->allocator.allocator = NULL;
			result->size = 0;
			result->capacity = addr_size - sizeof(strbuf_t) - 1;
			result->cstr[0] = 0;
		};
	};

	return result;
}

// concatenate a number of str's this can include the buffer itself, strbuf.str for appending
str_t _strbuf_cat(strbuf_t** buf_ptr, int n_args, ...)
{
	va_list va;
	va_start(va, n_args);
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
		str = buffer_vcat(buf_ptr, n_args, va);
	va_end(va);
	return str;
}

str_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va)
{
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
		str = buffer_vcat(buf_ptr, n_args, va);
	return str;
}

#ifdef STRBUF_PROVIDE_PRINTF
str_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_vprintf(buf_ptr, format, va);
		va_end(va);
	};
	return str;
}

str_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	int size;
	strbuf_t* buf;
	str_t str = {0};
	va_list vb;
	if(buf_ptr && *buf_ptr)
	{
		va_copy(vb, va);
		buf = *buf_ptr;
		size = vsnprintf(NULL, 0, format, va);

		if(buf_is_dynamic(buf) && size > buf->capacity)
			change_buf_capacity(&buf, round_up_capacity(size));

		if(size <= buf->capacity)
			buf->size = vsnprintf(buf->cstr, buf->capacity, format, vb);
		else
		{
			buf->size = 0;
			buf->cstr[0] = 0;
		};
		str = strbuf_str(&buf);
		*buf_ptr = buf;
		va_end(vb);
	};
	return str;
}
#endif

str_t strbuf_str(strbuf_t** buf_ptr)
{
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
		str = str_of_buf(*buf_ptr);
	return str;
}

str_t strbuf_append_char(strbuf_t** buf_ptr, char c)
{
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		append_char_to_buf(buf_ptr, c);
		str = str_of_buf(*buf_ptr);
	};
	return str;
}

// reduce allocation size to the minimum possible
str_t strbuf_shrink(strbuf_t** buf_ptr)
{
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		if(buf_is_dynamic(*buf_ptr))
			change_buf_capacity(buf_ptr, (*buf_ptr)->size);
		str = str_of_buf(*buf_ptr);
	};
	return str;
}

void strbuf_destroy(strbuf_t** buf_ptr)
{
	if(buf_ptr)
	{
		if(*buf_ptr && buf_is_dynamic(*buf_ptr))
			destroy_buf(buf_ptr);
		*buf_ptr = NULL;
	};	
}

str_t strbuf_append(strbuf_t** buf_ptr, str_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_str_into_buf(buf_ptr, (*buf_ptr)->size, str);
	return str_of_buf(*buf_ptr);
}

str_t strbuf_prepend(strbuf_t** buf_ptr, str_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_str_into_buf(buf_ptr, 0, str);
	return str_of_buf(*buf_ptr);
}

str_t strbuf_insert(strbuf_t** buf_ptr, int index, str_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_str_into_buf(buf_ptr, index, str);
	return str_of_buf(*buf_ptr);
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static strbuf_t* create_buf(size_t initial_capacity, str_allocator_t allocator)
{
	strbuf_t* buf;

	initial_capacity = initial_capacity;

	buf = allocator.allocator(&allocator, NULL, sizeof(strbuf_t)+initial_capacity+1,  __FILE__, __LINE__);
	buf->size = 0;
	buf->capacity = initial_capacity;
	buf->allocator = allocator;
	buf->cstr[buf->size] = 0;

	return buf;
}

static str_t str_of_buf(strbuf_t* buf)
{
	str_t str;
	str.data = buf->cstr;
	str.size = buf->size;
	return str;
}

static str_t buffer_vcat(strbuf_t** buf_ptr, int n_args, va_list va)
{
	str_t 	str;
	size_t 	size_needed = 0;
	bool	tmp_buf_needed = false;
	int 	i = 0;
	strbuf_t* dst_buf = *buf_ptr;
	strbuf_t* build_buf;
	va_list vb;
	va_copy(vb, va);

	while(i++ != n_args)
	{
		str = va_arg(va, str_t);
		size_needed += str.size;
		tmp_buf_needed |= buf_contains_str(dst_buf, str);
	};

	if(tmp_buf_needed && buf_is_dynamic(dst_buf))
		build_buf = create_buf(size_needed, dst_buf->allocator);
	else
	{
		if(buf_is_dynamic(dst_buf) && dst_buf->capacity < size_needed)
			change_buf_capacity(&dst_buf, round_up_capacity(size_needed));
		build_buf = dst_buf;
		build_buf->size = 0;
		build_buf->cstr[0] = 0;
	};

	if(buf_is_dynamic(build_buf) || !tmp_buf_needed)
	{
		if(build_buf->capacity >= size_needed)
		{
			i = 0;
			while(i++ != n_args)
				insert_str_into_buf(&build_buf, build_buf->size, va_arg(vb, str_t));	
		};
	};

	if(tmp_buf_needed && buf_is_dynamic(build_buf))
	{
		assign_str_to_buf(&dst_buf, str_of_buf(build_buf));
		destroy_buf(&build_buf);
	};
	*buf_ptr = dst_buf;

	va_end(vb);
	return str_of_buf(dst_buf);
}

static void insert_str_into_buf(strbuf_t** buf_ptr, int index, str_t str)
{
	strbuf_t* buf = *buf_ptr;

	if(index > buf->size)
		index = buf->size;
	if(index < 0)
		index += buf->size;
	if(index < 0)
		index = 0;

	if(buf_is_dynamic(buf) && buf->capacity < buf->size + str.size)
		change_buf_capacity(&buf, round_up_capacity(buf->size + str.size));

	if(buf->capacity >= buf->size + str.size)
	{
		if(&buf->cstr[index+str.size] !=  &buf->cstr[index])
			memmove(&buf->cstr[index+str.size], &buf->cstr[index], buf->size-index);

		buf->size += str.size;
		memcpy(&buf->cstr[index], str.data, str.size);
		buf->cstr[buf->size] = 0;
	}
	else
	{
		buf->size = 0;
		buf->cstr[0] = 0;
	};
	*buf_ptr = buf;
}

static void destroy_buf(strbuf_t** buf_ptr)
{
	strbuf_t* buf = *buf_ptr;
	if(buf_is_dynamic(buf))
		buf->allocator.allocator(&buf->allocator, buf, 0, __FILE__, __LINE__);
	*buf_ptr = NULL;
}

static void change_buf_capacity(strbuf_t** buf_ptr, size_t new_capacity)
{
	strbuf_t* buf = *buf_ptr;

	if(buf_is_dynamic(buf))
	{
		if(new_capacity < buf->size)
			new_capacity = buf->size;

		if(new_capacity != buf->capacity)
		{
			buf = buf->allocator.allocator(&buf->allocator, buf, sizeof(strbuf_t)+new_capacity+1, __FILE__, __LINE__);
			buf->capacity = new_capacity;
		};
	};
	*buf_ptr = buf;
}

static void assign_str_to_buf(strbuf_t** buf_ptr, str_t str)
{
	(*buf_ptr)->size = 0;
	(*buf_ptr)->cstr[0] = 0;
	insert_str_into_buf(buf_ptr, 0, str);
}

static void append_char_to_buf(strbuf_t** buf_ptr, char c)
{
	strbuf_t* buf = *buf_ptr;

	if(buf_is_dynamic(buf) && buf->size+1 > buf->capacity)
		change_buf_capacity(&buf, round_up_capacity(buf->size + 1));

	if(buf->size+1 <= buf->capacity)
	{
		buf->cstr[buf->size] = c;
		buf->size++;
		buf->cstr[buf->size] = 0;
	}
	else
	{
		buf->size = 0;
		buf->cstr[0] = 0;
	};
	*buf_ptr = buf;
}

static size_t round_up_capacity(size_t capacity)
{
	if(capacity % STR_CAPACITY_GROW_STEP)
		capacity = (capacity + STR_CAPACITY_GROW_STEP) - (capacity % STR_CAPACITY_GROW_STEP);
	return capacity;
}

static bool buf_contains_str(strbuf_t* buf, str_t str)
{
	return &buf->cstr[0] <= str.data && str.data < &buf->cstr[buf->size];
}

static bool buf_is_dynamic(strbuf_t* buf)
{
	return !!(buf->allocator.allocator);
}