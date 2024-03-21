/*
*/

	#include <limits.h>
	#include <ctype.h>
	#include "strview.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//	#include <stdio.h>
//	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	#define CASE_SENSETIVE		true
	#define NOT_CASE_SENSETIVE	false

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static bool contains_char(strview_t str, char c, bool case_sensetive);

	static strview_t split_first_delim(strview_t* strview_ptr, strview_t delims, bool case_sensetive);
	static strview_t split_last_delim(strview_t* strview_ptr, strview_t delims, bool case_sensetive);
	static strview_t split_index(strview_t* strview_ptr, int index);

	static int memcmp_nocase(const char* a, const char* b, size_t size);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strview_t cstr(const char* c_str)
{
	return c_str ? (strview_t){.data = c_str, .size = strlen(c_str)} : STRVIEW_INVALID;
}

char* strview_to_cstr(char* dst, size_t dst_size, strview_t str)
{
	size_t copy_size;
	size_t src_size = str.size < 0 ? 0:str.size;
	if(dst_size && dst)
	{
		if(str.size < 0)
			str.size = 0;
		copy_size = (dst_size-1) < src_size ? (dst_size-1):src_size;
		memcpy(dst, str.data, copy_size);
		dst[copy_size] = 0;
	};
	return dst;
}

bool strview_is_valid(strview_t str)
{
	return !!str.data;
}

void strview_swap(strview_t* a, strview_t* b)
{
	strview_t tmp = *a;
	*a = *b;
	*b = tmp;
}

bool strview_is_match_strview(strview_t str1, strview_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp(str1.data, str2.data, str1.size));
}

bool strview_is_match_cstr(strview_t str1, const char* str2)
{
	return strview_is_match_strview(str1, cstr(str2));
}

bool strview_is_match_nocase_strview(strview_t str1, strview_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp_nocase(str1.data, str2.data, str1.size));
}

bool strview_is_match_nocase_cstr(strview_t str1, const char* str2)
{
	return strview_is_match_nocase_strview(str1, cstr(str2));
}

bool strview_starts_with_strview(strview_t str1, strview_t str2)
{
	bool result;

	if(!strview_is_valid(str2))
		result = !strview_is_valid(str1);
	else
		result = (str1.size >= str2.size) && (str1.data == str2.data || !memcmp(str1.data, str2.data, str2.size));

	return result;
}

bool strview_starts_with_cstr(strview_t str1, const char* str2)
{
	return strview_starts_with_strview(str1, cstr(str2));
}

bool strview_starts_with_nocase_strview(strview_t str1, strview_t str2)
{
	bool result;

	if(!strview_is_valid(str2))
		result = !strview_is_valid(str1);
	else
		result = (str1.size >= str2.size) && (str1.data == str2.data || !memcmp_nocase(str1.data, str2.data, str2.size));

	return result;
}

bool strview_starts_with_nocase_cstr(strview_t str1, const char* str2)
{
	return strview_starts_with_nocase_strview(str1, cstr(str2));
}

int strview_compare(strview_t str1, strview_t str2)
{
	int compare_size = str1.size < str2.size ? str1.size:str2.size;
	int result = 0;

	if(compare_size)
		result = memcmp(str1.data, str2.data, compare_size);

	if(!result && str1.size != str2.size)
		result = str1.size > str2.size ? +1:-1;

	return result;
}

bool strview_contains(strview_t haystack, strview_t needle)
{
	return strview_is_valid(strview_find_first(haystack, needle));
}

strview_t strview_sub(strview_t str, int begin, int end)
{
	strview_t result = (strview_t){.size = 0, .data = str.data};

	if(str.data && str.size)
	{
		if(begin < 0)
			begin = str.size + begin;
		if(end < 0)
			end = str.size + end;
		
		if(begin <= end && begin < str.size && end >= 0)
		{
			if(begin < 0)
				begin = 0;
			if(end > str.size)
				end = str.size;

			result.size = end-begin;
			result.data = &str.data[begin];
		}
		else
			result.data = NULL;
	};

	return result;
}

strview_t strview_trim_start_cstr(strview_t str, const char* chars_to_trim)
{
	return strview_trim_start_strview(str, cstr(chars_to_trim));
}

strview_t strview_trim_start_strview(strview_t str, strview_t chars_to_trim)
{
	while(str.size && contains_char(chars_to_trim, *str.data, CASE_SENSETIVE))
	{
		str.data++;
		str.size--;
	};

	return str;
}

strview_t strview_trim_end_cstr(strview_t str, const char* chars_to_trim)
{
	return strview_trim_end_strview(str, cstr(chars_to_trim));
}

strview_t strview_trim_end_strview(strview_t str, strview_t chars_to_trim)
{
	while(str.size && contains_char(chars_to_trim, str.data[str.size-1], CASE_SENSETIVE))
		str.size--;

	return str;
}

strview_t strview_trim_cstr(strview_t str, const char* chars_to_trim)
{
	return strview_trim_strview(str, cstr(chars_to_trim));
}

strview_t strview_trim_strview(strview_t str, strview_t chars_to_trim)
{
	str = strview_trim_start_strview(str, chars_to_trim);
	str = strview_trim_end_strview(str, chars_to_trim);
	return str;
}

strview_t strview_find_first_strview(strview_t haystack, strview_t needle)
{
	strview_t result = STRVIEW_INVALID;

	const char* remaining_hay = haystack.data;
	bool found = false;

	if(haystack.data && needle.data)
	{
		while((&haystack.data[haystack.size] - remaining_hay >= needle.size) && !found)
		{
			found = !memcmp(remaining_hay, needle.data, needle.size);
			remaining_hay += !found;
		};
	};

	if(found)
	{
		result.data = remaining_hay;
		result.size = needle.size;
	};

	return result;
}

strview_t strview_find_first_cstr(strview_t haystack, const char* needle)
{
	return strview_find_first_strview(haystack, cstr(needle));
}

strview_t strview_find_last_strview(strview_t haystack, strview_t needle)
{
	strview_t result = STRVIEW_INVALID;

	const char* remaining_hay = &haystack.data[haystack.size - needle.size];
	bool found = false;

	if(haystack.data && needle.data)
	{
		while((remaining_hay >= haystack.data) && !found)
		{
			found = !memcmp(remaining_hay, needle.data, needle.size);
			remaining_hay -= !found;
		};
	};

	if(found)
	{
		result.data = remaining_hay;
		result.size = needle.size;
	};

	return result;
}

strview_t strview_find_last_cstr(strview_t haystack, const char* needle)
{
	return strview_find_last_strview(haystack, cstr(needle));
}

strview_t strview_split_first_delim_strview(strview_t* strview_ptr, strview_t delims)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_first_delim(strview_ptr, delims, CASE_SENSETIVE);

	return result;
}

int strview_split_all_cstr(int dst_size, strview_t dst[dst_size], strview_t src, const char* delims)
{
	return strview_split_all_strview(dst_size, dst, src, cstr(delims));
}

int strview_split_all_strview(int dst_size, strview_t dst[dst_size], strview_t src, strview_t delims)
{
	int count = 0;
	while(strview_is_valid(src) && count < dst_size)
		dst[count++] = strview_split_first_delim_strview(&src, delims);
	return count;
}

strview_t strview_split_first_delim_cstr(strview_t* strview_ptr, const char* delims)
{
	return strview_split_first_delim_strview(strview_ptr, cstr(delims));
}

strview_t strview_split_first_delim_nocase_strview(strview_t* strview_ptr, strview_t delims)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_first_delim(strview_ptr, delims, NOT_CASE_SENSETIVE);

	return result;
}

strview_t strview_split_first_delim_nocase_cstr(strview_t* strview_ptr, const char* delims)
{
	return strview_split_first_delim_nocase_strview(strview_ptr, cstr(delims));
}

strview_t strview_split_last_delim_strview(strview_t* strview_ptr, strview_t delims)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_last_delim(strview_ptr, delims, CASE_SENSETIVE);

	return result;
}

strview_t strview_split_last_delim_cstr(strview_t* strview_ptr, const char* delims)
{
	return strview_split_last_delim_strview(strview_ptr, cstr(delims));
}

strview_t strview_split_last_delim_nocase_strview(strview_t* strview_ptr, strview_t delims)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_last_delim(strview_ptr, delims, NOT_CASE_SENSETIVE);

	return result;
}

strview_t strview_split_last_delim_nocase_cstr(strview_t* strview_ptr, const char* delims)
{
	return strview_split_last_delim_nocase_strview(strview_ptr, cstr(delims));
}

strview_t strview_split_index(strview_t* strview_ptr, int index)
{
	strview_t result = STRVIEW_INVALID;

	if(strview_ptr)
		result = split_index(strview_ptr, index);

	return result;
}

char strview_pop_first_char(strview_t* strview_ptr)
{
	char result = 0;
	if(strview_ptr && strview_ptr->size)
		result = split_index(strview_ptr, 1).data[0];
	return result;
}

strview_t strview_split_line(strview_t* strview_ptr, char* eol)
{
	strview_t result = STRVIEW_INVALID;
	strview_t src;
	char e = 0;

	if(strview_ptr && strview_ptr->size)
	{
		src = *strview_ptr;
		if(eol && *eol)
		{
			if(*eol + src.data[0] == '\r'+'\n')
				strview_pop_first_char(&src);
		};

		result = strview_split_first_delim(&src, cstr("\r\n"));

		if(strview_is_valid(src))	//a line ending was found
		{
			e = result.data[result.size];
			if(e + src.data[0] == '\r'+'\n')
			{
				strview_pop_first_char(&src);
				e = 0;
			};
			if(eol)
				*eol = e;
			*strview_ptr = src;
		}
		else	//a line ending was not found, restore the source, and return an invalid strview_t
		{
			*strview_ptr = result;
			result = STRVIEW_INVALID;
		};
	};
	return result;
}

strview_t strview_split_left(strview_t* strview_ptr, strview_t pos)
{
	strview_t result = STRVIEW_INVALID;
	if(strview_ptr && strview_is_valid(*strview_ptr) && strview_is_valid(pos))
	{
		if(strview_ptr->data <= pos.data && pos.data <= &strview_ptr->data[strview_ptr->size])
			result = split_index(strview_ptr, (int)(pos.data - strview_ptr->data));
	};
	return result;
}

strview_t strview_split_right(strview_t* strview_ptr, strview_t pos)
{
	strview_t result = STRVIEW_INVALID;
	strview_t src;
	const char* split_point;

	if(strview_ptr && strview_is_valid(*strview_ptr) && strview_is_valid(pos))
	{
		src = *strview_ptr;
		split_point = &pos.data[pos.size];
		if(src.data <= split_point && split_point <= &src.data[src.size])
		{
			result = split_index(&src, (int)(split_point - src.data));
			strview_swap(&result, &src);
		};
		*strview_ptr = src;
	};

	return result;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static bool contains_char(strview_t str, char c, bool case_sensetive)
{
	bool found = false;
	const char* ptr = str.data;

	while(!found && ptr != &str.data[str.size])
	{
		if(case_sensetive)
			found = *ptr == c;
		else
			found = toupper(*ptr) == toupper(c);
		ptr++;
	};

	return found;
}

static int memcmp_nocase(const char* a, const char* b, size_t size)
{
	int result = 0;

	while(size-- && !result)
		result = toupper((unsigned)(*a++)) - toupper((unsigned)(*b++));

	return result;
}

static strview_t split_first_delim(strview_t* strview_ptr, strview_t delims, bool case_sensetive)
{
	strview_t result;
	bool found = false;
	const char* ptr;
	
	ptr = strview_ptr->data;

	if(strview_ptr->data && delims.data)
	{
		// try to find the delim
		while(ptr != &strview_ptr->data[strview_ptr->size] && !found)
		{
			found = contains_char(delims, *ptr, case_sensetive);
			ptr += !found;
		};
	};

	if(found)
	{
		result.data = strview_ptr->data;
		result.size = ptr - strview_ptr->data;
		strview_ptr->data = ptr;
		strview_ptr->size -= result.size;

		// at this stage, the remainder still includes the delim
		strview_ptr->size--;
		if(strview_ptr->size)	//only point to the character after the delim if there is one
			strview_ptr->data++;
	}
	else
	{
		result = *strview_ptr;
		*strview_ptr = STRVIEW_INVALID;
	};

	return result;
}

static strview_t split_last_delim(strview_t* strview_ptr, strview_t delims, bool case_sensetive)
{
	strview_t result;
	bool found = false;
	const char* ptr;

	if(strview_ptr->data && strview_ptr->size && delims.data)
	{
		// starting from the last character, try to find the delim backwards
		ptr = &strview_ptr->data[strview_ptr->size-1];
		while(ptr != strview_ptr->data-1 && !found)
		{
			found = contains_char(delims, *ptr, case_sensetive);
			ptr -= !found;
		};
	};

	if(found)
	{
		result.data = ptr;
		result.size = &strview_ptr->data[strview_ptr->size] - ptr;
		strview_ptr->size -= result.size;

		// at this stage, the result still starts with the delim
		result.size--;
		if(result.size)
			result.data++; //only point to the the character after the delim if there is one
	}
	else
	{
		result = *strview_ptr;
		*strview_ptr = STRVIEW_INVALID;
	};

	return result;
}

static strview_t split_index(strview_t* strview_ptr, int index)
{
	strview_t result = STRVIEW_INVALID;
	strview_t remainder = *strview_ptr;
	bool neg = index < 0;

	if(neg)
		index = strview_ptr->size + index;
		
	if(index < 0)
		index = 0;
	if(index > strview_ptr->size)
		index = strview_ptr->size;

	result.data = remainder.data;
	result.size = index;
	remainder.data += index;
	remainder.size -= index;

	if(!neg)
		*strview_ptr = remainder;
	else
	{
		*strview_ptr = result;
		result = remainder;
	};

	return result;
}

strview_t strview_dequote(strview_t src)
{
	strview_t result = src;
	char quote_char;

	if(result.size > 1)
		quote_char = result.data[0];
	while( 	(result.size > 1)
	 && 	(result.data[0] == quote_char)
	 && 	(result.data[result.size-1] == quote_char) )
		result = strview_sub(result, 1, -1);
	return result;
}

