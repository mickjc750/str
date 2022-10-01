/*
*/
	#include <ctype.h>
	#include "str.h"
	#include "strbuf.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#include <stdio.h>
	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

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

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strbuf_t* strbuf_create(size_t initial_capacity, str_allocator_t allocator)
{
	return create_buf(initial_capacity, allocator);
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
		change_buf_capacity(buf_ptr, (*buf_ptr)->size);
		str = str_of_buf(*buf_ptr);
	};
	return str;
}

void strbuf_destroy(strbuf_t** buf_ptr)
{
	if(buf_ptr && *buf_ptr)
		destroy_buf(buf_ptr);
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

	initial_capacity = round_up_capacity(initial_capacity);

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
	str_t 	str_array[n_args];
	size_t 	size_needed = 0;
	bool	tmp_buf_needed = false;
	int 	i = 0;
	strbuf_t* dst_buf = *buf_ptr;
	strbuf_t* build_buf;

	while(i != n_args)
	{
		str_array[i] = va_arg(va, str_t);
		size_needed += str_array[i].size;
		tmp_buf_needed |= buf_contains_str(dst_buf, str_array[i]);
		i++;
	};

	if(tmp_buf_needed)
		build_buf = create_buf(size_needed, dst_buf->allocator);
	else
	{
		if(dst_buf->capacity < size_needed)
			change_buf_capacity(&dst_buf, round_up_capacity(size_needed));
		build_buf = dst_buf;
		build_buf->size = 0;
	};

	i = 0;
	while(i != n_args)
		insert_str_into_buf(&build_buf, build_buf->size, str_array[i++]);

	if(tmp_buf_needed)
	{
		assign_str_to_buf(&dst_buf, str_of_buf(build_buf));
		destroy_buf(&build_buf);
	};
	*buf_ptr = dst_buf;

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

	if(buf->capacity < buf->size + str.size)
		change_buf_capacity(&buf, round_up_capacity(buf->size + str.size));

	if(&buf->cstr[index+str.size] !=  &buf->cstr[index])
		memmove(&buf->cstr[index+str.size], &buf->cstr[index], str.size);

	buf->size += str.size;
	memcpy(&buf->cstr[index], str.data, str.size);
	buf->cstr[buf->size] = 0;
	*buf_ptr = buf;
}

static void destroy_buf(strbuf_t** buf_ptr)
{
	strbuf_t* buf = *buf_ptr;
	buf->allocator.allocator(&buf->allocator, buf, 0, __FILE__, __LINE__);
	*buf_ptr = NULL;
}

static void change_buf_capacity(strbuf_t** buf_ptr, size_t new_capacity)
{
	strbuf_t* buf = *buf_ptr;

	if(new_capacity < buf->size)
		new_capacity = buf->size;

	if(new_capacity != buf->capacity)
	{
		buf = buf->allocator.allocator(&buf->allocator, buf, sizeof(strbuf_t)+new_capacity+1, __FILE__, __LINE__);
		buf->capacity = new_capacity;
	};
	*buf_ptr = buf;
}

static void assign_str_to_buf(strbuf_t** buf_ptr, str_t str)
{
	(*buf_ptr)->size = 0;
	insert_str_into_buf(buf_ptr, 0, str);
}

static void append_char_to_buf(strbuf_t** buf_ptr, char c)
{
	strbuf_t* buf = *buf_ptr;

	if(buf->size+1 > buf->capacity)
		change_buf_capacity(&buf, round_up_capacity(buf->size + 1));

	buf->cstr[buf->size] = c;
	buf->size++;
	buf->cstr[buf->size] = 0;
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
