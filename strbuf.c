/*
*/
	#include <stdint.h>
	#include <ctype.h>
	#include "str.h"
	#include "strbuf.h"

	#ifdef STRBUF_PROVIDE_PRINTF
		#include <stdio.h>
	#endif

	#ifdef STRBUF_PROVIDE_PRNF
		#include "prnf.h"
	#endif

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//	#include <stdio.h>
//	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static strbuf_t* create_buf(size_t initial_capacity, strbuf_allocator_t allocator);
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
	static void empty_buf(strbuf_t* buf);

#ifdef STRBUF_PROVIDE_PRNF
	static void char_handler_for_prnf(void* dst, char c);
#endif

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strbuf_t* strbuf_create(size_t initial_capacity, strbuf_allocator_t allocator)
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
			result->capacity = addr_size - sizeof(strbuf_t) - 1;
			empty_buf(result);
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
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		empty_buf(*buf_ptr);
		str = strbuf_append_vprintf(buf_ptr, format, va);
	};
	return str;
}

str_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_append_vprintf(buf_ptr, format, va);
		va_end(va);
	};
	return str;
}

str_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	int size;
	strbuf_t* buf;
	str_t str = {0};
	va_list vb;
	if(buf_ptr && *buf_ptr)
	{
		va_copy(vb, va);
		buf = *buf_ptr;
		size = buf->size;
		size += vsnprintf(NULL, 0, format, va);

		if(buf_is_dynamic(buf) && size > buf->capacity)
			change_buf_capacity(&buf, round_up_capacity(size));

		if(size <= buf->capacity)
			buf->size += vsnprintf(&buf->cstr[buf->size], buf->capacity - buf->size, format, vb);
		else
			empty_buf(buf);

		str = strbuf_str(&buf);
		*buf_ptr = buf;
		va_end(vb);
	};
	return str;
}

#endif

#ifdef STRBUF_PROVIDE_PRNF
str_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_vprnf(buf_ptr, format, va);
		va_end(va);;
	};
	return str;
}

str_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	strbuf_t* buf;
	str_t str = {0};
	int char_count;
	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		empty_buf(buf);

		char_count = vfptrprnf(char_handler_for_prnf, &buf,  format, va);

		if(char_count > buf->size)
			empty_buf(buf);

		str = strbuf_str(&buf);
		*buf_ptr = buf;
	};
	return str;
}

str_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	str_t str = {0};
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_append_vprnf(buf_ptr, format, va);
		va_end(va);;
	};
	return str;
}

str_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	strbuf_t* buf;
	str_t str = {0};
	int char_count;
	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;

		char_count = buf->size;
		char_count += vfptrprnf(char_handler_for_prnf, &buf,  format, va);

		if(char_count > buf->size)
			empty_buf(buf);

		str = strbuf_str(&buf);
		*buf_ptr = buf;
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

str_t strbuf_assign(strbuf_t** buf_ptr, str_t str)
{
	strbuf_t* buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		if(str_is_valid(str))
		{
			if(str.size > buf->capacity)
				change_buf_capacity(&buf, round_up_capacity(str.size));
			memmove(buf->cstr, str.data, str.size);
			buf->size = str.size;
			buf->cstr[buf->size] = 0;
		}
		else
			empty_buf(buf);
		*buf_ptr = buf;
	};

	return str_of_buf(buf);
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

str_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, str_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_str_into_buf(buf_ptr, index, str);
	return str_of_buf(*buf_ptr);
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static strbuf_t* create_buf(size_t initial_capacity, strbuf_allocator_t allocator)
{
	strbuf_t* buf;

	initial_capacity = initial_capacity;

	buf = allocator.allocator(&allocator, NULL, sizeof(strbuf_t)+initial_capacity+1,  __FILE__, __LINE__);
	buf->capacity = initial_capacity;
	buf->allocator = allocator;
	empty_buf(buf);

	return buf;
}

static str_t str_of_buf(strbuf_t* buf)
{
	str_t str = {.data = NULL, .size = 0};
	if(buf)
	{
		str.data = buf->cstr;
		str.size = buf->size;
	};
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
		empty_buf(build_buf);
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
	bool src_in_dst = buf_contains_str(buf, str);
	size_t src_offset = str.data - buf->cstr;
	str_t str_part_left_behind = {.data=NULL, .size=0};
	str_t str_part_shifted;
	char* move_src;
	char* move_dst;

	if(index > buf->size)
		index = buf->size;
	if(index < 0)
		index += buf->size;
	if(index < 0)
		index = 0;

	if(buf_is_dynamic(buf) && buf->capacity < buf->size + str.size)
		change_buf_capacity(&buf, round_up_capacity(buf->size + str.size));

	if(src_in_dst && buf != *buf_ptr)
		str.data = buf->cstr + src_offset;

	if(buf->capacity >= buf->size + str.size)
	{
		str_part_shifted = str;
		move_src = &buf->cstr[index];
		move_dst = &buf->cstr[index+str.size];
		if(str.size)
		{
			memmove(move_dst, move_src, buf->size-index);
			if(src_in_dst)
			{
				if(move_src > str.data)
					str_part_left_behind = str_pop_split(&str_part_shifted, move_src - str.data);
				str_part_shifted.data += move_dst-move_src;
			};
		};

		buf->size += str.size;
		memcpy(move_src, str_part_left_behind.data, str_part_left_behind.size);
		move_src += str_part_left_behind.size;
		memcpy(move_src, str_part_shifted.data, str_part_shifted.size);
		buf->cstr[buf->size] = 0;
	}
	else
		empty_buf(buf);

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
	empty_buf(*buf_ptr);
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
		empty_buf(buf);

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

static void empty_buf(strbuf_t* buf)
{
	buf->size = 0;
	buf->cstr[0] = 0;
}

#ifdef STRBUF_PROVIDE_PRNF
static void char_handler_for_prnf(void* dst, char c)
{
	append_char_to_buf((strbuf_t**)dst, c);
}
#endif
