/*
*/

	#ifndef STR_NO_FLOAT
		#include <math.h>
	#endif
	#include <limits.h>
	#include <ctype.h>
	#include "str.h"

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

	static bool contains_char(str_t str, char c, bool case_sensetive);

	static str_t pop_first_split(str_t* str_ptr, str_t delimiters, bool case_sensetive);
	static str_t pop_last_split(str_t* str_ptr, str_t delimiters, bool case_sensetive);
	static str_t pop_split(str_t* str_ptr, int index);

	static unsigned long long interpret_hex(str_t str);
	static unsigned long long interpret_bin(str_t str);
	static unsigned long long interpret_dec(str_t str);

	#ifndef STR_NO_FLOAT
	static str_float_t interpret_float(str_t str);
	#endif

	static int memcmp_nocase(const char* a, const char* b, size_t size);

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

bool str_is_valid(str_t str)
{
	return !!str.data;
}

void str_swap(str_t* a, str_t* b)
{
	str_t tmp = *a;
	*a = *b;
	*b = tmp;
}

bool str_is_match(str_t str1, str_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp(str1.data, str2.data, str1.size));
}

bool str_is_match_nocase(str_t str1, str_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp_nocase(str1.data, str2.data, str1.size));
}

int str_compare(str_t str1, str_t str2)
{
	int compare_size = str1.size < str2.size ? str1.size:str2.size;
	int result = 0;

	if(compare_size)
		result = memcmp(str1.data, str2.data, compare_size);

	if(!result && str1.size != str2.size)
		result = str1.size > str2.size ? +1:-1;

	return result;
}

bool str_contains(str_t haystack, str_t needle)
{
	return str_is_valid(str_find_first(haystack, needle));
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

str_t str_trim_start(str_t str, str_t chars_to_trim)
{
	while(str.size && contains_char(chars_to_trim, *str.data, CASE_SENSETIVE))
	{
		str.data++;
		str.size--;
	};

	return str;
}

str_t str_trim_end(str_t str, str_t chars_to_trim)
{
	while(str.size && contains_char(chars_to_trim, str.data[str.size-1], CASE_SENSETIVE))
		str.size--;

	return str;
}

str_t str_trim(str_t str, str_t chars_to_trim)
{
	str = str_trim_start(str, chars_to_trim);
	str = str_trim_end(str, chars_to_trim);
	return str;
}

str_t str_find_first(str_t haystack, str_t needle)
{
	str_t result = STR_INVALID;

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

str_t str_find_last(str_t haystack, str_t needle)
{
	str_t result = STR_INVALID;

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

str_t str_pop_first_split(str_t* str_ptr, str_t delimiters)
{
	str_t result = STR_INVALID;
	
	if(str_ptr)
		result = pop_first_split(str_ptr, delimiters, CASE_SENSETIVE);

	return result;
}

str_t str_pop_first_split_nocase(str_t* str_ptr, str_t delimiters)
{
	str_t result = STR_INVALID;
	
	if(str_ptr)
		result = pop_first_split(str_ptr, delimiters, NOT_CASE_SENSETIVE);

	return result;
}

str_t str_pop_last_split(str_t* str_ptr, str_t delimiters)
{
	str_t result = STR_INVALID;
	
	if(str_ptr)
		result = pop_last_split(str_ptr, delimiters, CASE_SENSETIVE);

	return result;
}

str_t str_pop_last_split_nocase(str_t* str_ptr, str_t delimiters)
{
	str_t result = STR_INVALID;
	
	if(str_ptr)
		result = pop_last_split(str_ptr, delimiters, NOT_CASE_SENSETIVE);

	return result;
}

str_t str_pop_split(str_t* str_ptr, int index)
{
	str_t result = STR_INVALID;

	if(str_ptr)
		result = pop_split(str_ptr, index);

	return result;
}

char str_pop_first_char(str_t* str_ptr)
{
	char result = 0;
	if(str_ptr && str_ptr->size)
		result = pop_split(str_ptr, 1).data[0];
	return result;
}

str_t str_pop_line(str_t* str_ptr, char* eol)
{
	str_t result = STR_INVALID;
	str_t src;
	char e = 0;

	if(str_ptr && str_ptr->size)
	{
		src = *str_ptr;
		if(eol && *eol)
		{
			if(*eol + src.data[0] == '\r'+'\n')
				str_pop_first_char(&src);
		};

		result = str_pop_first_split(&src, cstr("\r\n"));

		if(str_is_valid(src))	//a line ending was found
		{
			e = result.data[result.size];
			if(e + src.data[0] == '\r'+'\n')
			{
				str_pop_first_char(&src);
				e = 0;
			};
			if(eol)
				*eol = e;
			*str_ptr = src;
		}
		else	//a line ending was not found, restore the source, and return an invalid str_t
		{
			*str_ptr = result;
			result = STR_INVALID;
		};
	};
	return result;
}

long long str_to_ll(str_t str)
{
	unsigned long long magnitude = 0;
	bool is_neg = false;

	if(str.data)
	{
		str = str_trim(str, cstr(" "));
		
		if(str.size)
		{
			if(str.data[0] == '+')
				str = str_sub(str, 1, INT_MAX);
			else if(str.data[0] == '-')
			{
				is_neg = true;
				str = str_sub(str, 1, INT_MAX);
			};
		};

		magnitude = str_to_ull(str);
	};

	return is_neg ? magnitude*-1 : magnitude;
}

unsigned long long str_to_ull(str_t str)
{
	long long result = 0;
	str_t base_str;

	if(str.data)
	{
		str = str_trim(str, cstr(" "));

		base_str = str_sub(str, 0, 2);
		if( str_is_match(base_str, cstr("0x"))
		||	str_is_match(base_str, cstr("0X")))
			result = interpret_hex(str_sub(str, 2,INT_MAX));

		else if(str_is_match(base_str, cstr("0b")))
			result = interpret_bin(str_sub(str, 2,INT_MAX));

		else
			result = interpret_dec(str);
	};

	return result;
}

str_float_t str_to_float(str_t str)
{
	str_float_t result = 0;
	bool is_neg = false;
	if(str_is_valid(str))
	{
		str = str_trim(str, cstr(" "));

		if(str_is_match_nocase(str, cstr("nan")))
			result = NAN;
		else if(str.data[0] == '+')
			str = str_sub(str, 1, INT_MAX);
		else if(str.data[0] == '-')
		{
			is_neg = true;
			str = str_sub(str, 1, INT_MAX);
		};
		if(str_is_match_nocase(str, cstr("inf")))
			result = INFINITY;
		else if(!result)
			result = interpret_float(str);
	};
	return is_neg ? result*-1.0:result;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static unsigned long long interpret_hex(str_t str)
{
	unsigned long long result = 0;

	while(str.size && isxdigit(str.data[0]))
	{
		result <<= 4;
		if(isalpha(str.data[0]))
			result += 10 + (str.data[0] & 0x4F) - 'A';
		else
			result += str.data[0] & 0x0F;
		str = str_sub(str, 1, INT_MAX);
	};

	return result;
}

static unsigned long long interpret_bin(str_t str)
{
	unsigned long long result = 0;

	while(str.size && (str.data[0]=='0' || str.data[0]=='1'))
	{
		result <<= 1;
		result |= str.data[0] == '1';
		str = str_sub(str, 1, INT_MAX);
	};

	return result;
}

static unsigned long long interpret_dec(str_t str)
{
	unsigned long long result = 0;
	
	while(str.size && isdigit(str.data[0]))
	{
		result *= 10;
		result += str.data[0] & 0x0F;
		str = str_sub(str, 1, INT_MAX);
	};

	return result;
}

#ifndef STR_NO_FLOAT
static str_float_t interpret_float(str_t str)
{
	str_float_t result = 0;
	str_float_t fractional_digit_weight = 1;
	int exponent;

	while(str.size && isdigit(str.data[0]))
	{
		result *= 10;
		result += str.data[0] & 0x0F;
		str = str_sub(str, 1, INT_MAX);
	};

	if(str.size && str.data[0]=='.')
	{
		str = str_sub(str, 1, INT_MAX);

		while(str.size && isdigit(str.data[0]))
		{
			fractional_digit_weight /= 10.0;
			result += (str.data[0] & 0x0F) * fractional_digit_weight;
			str = str_sub(str, 1, INT_MAX);
		};
	};

	if(str.size && toupper(str.data[0])=='E')
	{
		str = str_sub(str, 1, INT_MAX);
		exponent = (int)str_to_ll(str);
		#ifdef STR_SUPPORT_FLOAT
			result *= powf(10, exponent);
		#elif defined STR_SUPPORT_LONG_DOUBLE
			result *= powl(10, exponent);
		#else
			result *= pow(10, exponent);
		#endif
	};

	return result;
}
#endif

static bool contains_char(str_t str, char c, bool case_sensetive)
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

static str_t pop_first_split(str_t* str_ptr, str_t delimiters, bool case_sensetive)
{
	str_t result;
	bool found = false;
	const char* ptr;
	
	ptr = str_ptr->data;

	if(str_ptr->data && delimiters.data)
	{
		// try to find the delimiter
		while(ptr != &str_ptr->data[str_ptr->size] && !found)
		{
			found = contains_char(delimiters, *ptr, case_sensetive);
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
		if(str_ptr->size)	//only point to the character after the delimiter if there is one
			str_ptr->data++;
	}
	else
	{
		result = *str_ptr;
		*str_ptr = STR_INVALID;
	};

	return result;
}

static str_t pop_last_split(str_t* str_ptr, str_t delimiters, bool case_sensetive)
{
	str_t result;
	bool found = false;
	const char* ptr;

	if(str_ptr->data && str_ptr->size && delimiters.data)
	{
		// starting from the last character, try to find the delimiter backwards
		ptr = &str_ptr->data[str_ptr->size-1];
		while(ptr != str_ptr->data-1 && !found)
		{
			found = contains_char(delimiters, *ptr, case_sensetive);
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
			result.data++; //only point to the the character after the delimiter if there is one
	}
	else
	{
		result = *str_ptr;
		*str_ptr = STR_INVALID;
	};

	return result;
}

static str_t pop_split(str_t* str_ptr, int index)
{
	str_t result = (str_t){0};
	str_t remainder = *str_ptr;
	bool neg = index < 0;

	if(neg)
		index = str_ptr->size + index;
		
	if(index < 0)
		index = 0;
	if(index > str_ptr->size)
		index = str_ptr->size;

	result.data = remainder.data;
	result.size = index;
	remainder.data += index;
	remainder.size -= index;

	if(!neg)
		*str_ptr = remainder;
	else
	{
		*str_ptr = result;
		result = remainder;
	};

	return result;
}
