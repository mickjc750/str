/*
*/

	#include "str.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//	#include <stdio.h>
//	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static bool contains_char(str_t str, char c);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

str_t cstr(const char* c_str)
{
	return (str_t){.data = c_str, .size = strlen(c_str)};
}

char* str_to_cstr(char* dst, size_t dst_size, str_t str)
{
	size_t copy_size;
	if(dst_size && *dst)
	{
		copy_size = (dst_size-1) < str.size ? dst_size:str.size;
		memcpy(dst, str.data, copy_size);
		dst[copy_size] = 0;
	};
	return dst;
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
	return str_find_first(haystack, needle).found;
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
	while(str.size && contains_char(chars_to_trim, *str.data))
	{
		str.data++;
		str.size--;
	};
	while(str.size && contains_char(chars_to_trim, str.data[str.size-1]))
		str.size--;

	return str;
}

str_search_result_t str_find_first(str_t haystack, str_t needle)
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
			found = contains_char(delimiters, *ptr);
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
			found = contains_char(delimiters, *ptr);
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

static bool contains_char(str_t str, char c)
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
