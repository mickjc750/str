/*
*/

	#include "str.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#include <stdio.h>
	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static str_buf_t create_buf(size_t initial_capacity, str_allocator_t allocator);
	static str_t buffer_vcat(str_buf_t* buf_ptr, int n_args, va_list va);
	static void append_str_to_buf(str_buf_t* buf_ptr, str_t str);
	static void destroy_buf(str_buf_t* buf_ptr);
	static void change_buf_capacity(str_buf_t* buf_ptr, size_t new_capacity);
	static void assign_str_to_buf(str_buf_t* buf_ptr, str_t str);
	static void append_char_to_buf(str_buf_t* str_buf, char c);
	static str_search_result_t find_first_needle(str_t haystack, str_t needle);
	static bool str_contains_char(str_t str, char c);
	static size_t round_up_capacity(size_t capacity);
	static str_t str_of_buf(str_buf_t* buf);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

str_buf_t str_buf_create(size_t initial_capacity, str_allocator_t allocator)
{
	return create_buf(initial_capacity, allocator);
}

// concatenate a number of str's this can include the buffer itself, str_buf.str for appending
str_t _str_buf_cat(str_buf_t* buf_ptr, int n_args, ...)
{
	va_list va;
	va_start(va, n_args);
	str_t str = str_buf_vcat(buf_ptr, n_args, va);
	va_end(va);
	return str;
}

str_t str_buf_vcat(str_buf_t* buf_ptr, int n_args, va_list va)
{
	return buffer_vcat(buf_ptr, n_args, va);
}

str_t str_buf_str(str_buf_t* buf_ptr)
{
	return str_of_buf(buf_ptr);
}

str_t str_buf_append_char(str_buf_t* buf_ptr, char c)
{
	append_char_to_buf(buf_ptr, c);
	return str_of_buf(buf_ptr);
}

// reduce allocation size to minimum possible
str_t str_buf_shrink(str_buf_t* buf_ptr)
{
	change_buf_capacity(buf_ptr, buf_ptr->size);
	return str_of_buf(buf_ptr);
}

void str_buf_destroy(str_buf_t* buf_ptr)
{
	destroy_buf(buf_ptr);
}

str_t cstr(const char* c_str)
{
	return (str_t){.data = c_str, .size = strlen(c_str)};
}

bool str_is_valid(str_t str)
{
	return !!str.data;
}

bool str_is_match(str_t str1, str_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp(str1.data, str2.data, str1.size));
}

bool str_contains(str_t haystack, str_t needle)
{
	return find_first_needle(haystack, needle).found;
}

str_t str_sub(str_t str, int begin, int end)
{
	str_t result = (str_t){.size = 0, .data = str.data};

	if(str.data && str.size)
	{
		if(begin < 0)
			begin = str.size + begin;
		if(end < 0)
			end = str.size + end;

		if(begin >= str.size)		//begin is inclusive, so must be < size
			begin = str.size-1;
		if(end > str.size)			//end is non-inclusive, so must be <= size
			end = str.size;

		result.data = &str.data[begin];
		result.size = end-begin;
	};

	return result;
}

str_t str_trim(str_t str, str_t chars_to_trim)
{
	while(str_contains_char(chars_to_trim, *str.data) && str.size)
	{
		str.data++;
		str.size--;
	};
	while(str_contains_char(chars_to_trim, str.data[str.size]) && str.size)
	{
		str.data--;
		str.size--;
	};
	return str;
}

str_search_result_t str_find_first(str_t haystack, str_t needle)
{
	return find_first_needle(haystack, needle);
}

str_search_result_t str_find_last(str_t haystack, str_t needle)
{
	str_search_result_t result = (str_search_result_t){.found=false, .index=0};

	const char* remaining_hay = &haystack.data[haystack.size - needle.size];

	if(haystack.data && needle.data)
	{
		while((remaining_hay >= haystack.data) && !result.found)
		{
			result.found = !memcmp(remaining_hay, needle.data, needle.size);
			remaining_hay -= !result.found;
		};
	};

	if(result.found)
		result.index = remaining_hay - haystack.data;

	return result;
}

str_t str_pop_first_split(str_t* str_ptr, str_t delimiters)
{
		str_t result = (str_t){.data = NULL, .size = 0};
	bool found = false;
	const char* ptr = str_ptr->data;

	if(str_ptr->data && delimiters.data)
	{
		// trt to find the delimiter
		while(ptr != &str_ptr->data[str_ptr->size] && !found)
		{
			found = str_contains_char(delimiters, *ptr);
			ptr += !found;
		};
	};

	if(found)
	{
		result.data = str_ptr->data;
		result.size = ptr - str_ptr->data;
		str_ptr->data = ptr;
		str_ptr->size -= result.size;

		// at this stage, the remainder still includes the delimiter
		str_ptr->size--;
		if(str_ptr->size)	//only point the the character after the delimiter if there is one
			str_ptr->data++;
	};

	return result;
}

str_t str_pop_last_split(str_t* str_ptr, str_t delimiters)
{
	str_t result = (str_t){.data = NULL, .size = 0};
	bool found = false;
	const char* ptr;

	if(str_ptr->data && str_ptr->size && delimiters.data)
	{
		// starting from the last character, try to find the delimiter backwards
		ptr = &str_ptr->data[str_ptr->size-1];
		while(ptr != str_ptr->data-1 && !found)
		{
			found = str_contains_char(delimiters, *ptr);
			ptr -= !found;
		};
	};

	if(found)
	{
		result.data = ptr;
		result.size = &str_ptr->data[str_ptr->size] - ptr;
		str_ptr->size -= result.size;

		// at this stage, the result still starts with the delimiter
		result.size--;
		if(result.size)
			result.data++; //only point the the character after the delimiter if there is one
	};

	return result;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static str_buf_t create_buf(size_t initial_capacity, str_allocator_t allocator)
{
	str_buf_t buf;

	buf.cstr = allocator.allocator(&allocator, NULL, initial_capacity+1,  __FILE__, __LINE__);
	buf.size = 0;
	buf.capacity = initial_capacity;
	buf.allocator = allocator;

	buf.cstr[buf.size] = 0;

	return buf;
}

static str_t str_of_buf(str_buf_t* buf)
{
	str_t str;
	str.data = buf->cstr;
	str.size = buf->size;
	return str;
}

static str_t buffer_vcat(str_buf_t* buf_ptr, int n_args, va_list va)
{
	str_buf_t result_buf = create_buf(buf_ptr->capacity, buf_ptr->allocator);

	while(n_args--)
		append_str_to_buf(&result_buf, va_arg(va, str_t));

	assign_str_to_buf(buf_ptr, str_of_buf(&result_buf));
	destroy_buf(&result_buf);

	return str_of_buf(buf_ptr);
}

static void append_str_to_buf(str_buf_t* buf_ptr, str_t str)
{
	if(buf_ptr->capacity < buf_ptr->size + str.size)
		change_buf_capacity(buf_ptr, round_up_capacity(buf_ptr->size + str.size));

	memcpy(&buf_ptr->cstr[buf_ptr->size], str.data, str.size);
	buf_ptr->size += str.size;
	buf_ptr->cstr[buf_ptr->size] = 0;
}

static void destroy_buf(str_buf_t* buf_ptr)
{
	buf_ptr->allocator.allocator(&buf_ptr->allocator, buf_ptr->cstr, 0, __FILE__, __LINE__);
	buf_ptr->cstr = NULL;
	buf_ptr->size = 0;
	buf_ptr->capacity = 0;
	buf_ptr->allocator = (str_allocator_t){.allocator=NULL, .app_data=NULL};
}

static void change_buf_capacity(str_buf_t* buf_ptr, size_t new_capacity)
{
	if(new_capacity < buf_ptr->size)
		new_capacity = buf_ptr->size;

	if(new_capacity != buf_ptr->capacity)
	{
		buf_ptr->cstr = buf_ptr->allocator.allocator(&buf_ptr->allocator, buf_ptr->cstr, new_capacity+1, __FILE__, __LINE__);
		buf_ptr->capacity = new_capacity;
	};
}

static void assign_str_to_buf(str_buf_t* buf_ptr, str_t str)
{
	buf_ptr->size = 0;
	append_str_to_buf(buf_ptr, str);
}

static str_search_result_t find_first_needle(str_t haystack, str_t needle)
{
	str_search_result_t result = (str_search_result_t){.found=false, .index=0};

	const char* remaining_hay = haystack.data;

	if(haystack.data && needle.data)
	{
		while((&haystack.data[haystack.size] - remaining_hay >= needle.size) && !result.found)
		{
			result.found = !memcmp(remaining_hay, needle.data, needle.size);
			remaining_hay += !result.found;
		};
	};

	if(result.found)
		result.index = remaining_hay - haystack.data;

	return result;
}

static bool str_contains_char(str_t str, char c)
{
	bool found = false;
	const char* ptr = str.data;
	while(!found && ptr != &str.data[str.size])
	{
		found = *ptr == c;
		ptr++;
	};

	return found;
}

static void append_char_to_buf(str_buf_t* str_buf, char c)
{
	if(str_buf->size+1 < str_buf->capacity)
		change_buf_capacity(str_buf, round_up_capacity(str_buf->size + 1));
	str_buf->cstr[str_buf->size] = c;
	str_buf->size++;
	str_buf->cstr[str_buf->size] = 0;
}

static size_t round_up_capacity(size_t capacity)
{
	return (capacity + STR_CAPACITY_GROW_STEP) - (capacity % STR_CAPACITY_GROW_STEP);
}
