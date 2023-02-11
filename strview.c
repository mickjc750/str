/*
*/

	#ifndef STR_NO_FLOAT
		#include <math.h>
	#endif
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

	static strview_t pop_first_split(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive);
	static strview_t pop_last_split(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive);
	static strview_t pop_split(strview_t* strview_ptr, int index);

	static unsigned long long interpret_hex(strview_t str);
	static unsigned long long interpret_bin(strview_t str);
	static unsigned long long interpret_dec(strview_t str);

	#ifndef STR_NO_FLOAT
	static strview_float_t interpret_float(strview_t str);
	#endif

	static int memcmp_nocase(const char* a, const char* b, size_t size);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strview_t cstr(const char* c_str)
{
	return (strview_t){.data = c_str, .size = strlen(c_str)};
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

bool strview_is_match(strview_t str1, strview_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp(str1.data, str2.data, str1.size));
}

bool strview_is_match_nocase(strview_t str1, strview_t str2)
{
	return (str1.size == str2.size) && (str1.data == str2.data || !memcmp_nocase(str1.data, str2.data, str1.size));
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

strview_t strview_trim_start(strview_t str, strview_t chars_to_trim)
{
	while(str.size && contains_char(chars_to_trim, *str.data, CASE_SENSETIVE))
	{
		str.data++;
		str.size--;
	};

	return str;
}

strview_t strview_trim_end(strview_t str, strview_t chars_to_trim)
{
	while(str.size && contains_char(chars_to_trim, str.data[str.size-1], CASE_SENSETIVE))
		str.size--;

	return str;
}

strview_t strview_trim(strview_t str, strview_t chars_to_trim)
{
	str = strview_trim_start(str, chars_to_trim);
	str = strview_trim_end(str, chars_to_trim);
	return str;
}

strview_t strview_find_first(strview_t haystack, strview_t needle)
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

strview_t strview_find_last(strview_t haystack, strview_t needle)
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

strview_t strview_split_first_delimeter(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = pop_first_split(strview_ptr, delimiters, CASE_SENSETIVE);

	return result;
}

strview_t strview_split_first_delimiter_nocase(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = pop_first_split(strview_ptr, delimiters, NOT_CASE_SENSETIVE);

	return result;
}

strview_t strview_split_last_delimeter(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = pop_last_split(strview_ptr, delimiters, CASE_SENSETIVE);

	return result;
}

strview_t strview_split_last_delimeter_nocase(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = pop_last_split(strview_ptr, delimiters, NOT_CASE_SENSETIVE);

	return result;
}

strview_t strview_split_index(strview_t* strview_ptr, int index)
{
	strview_t result = STRVIEW_INVALID;

	if(strview_ptr)
		result = pop_split(strview_ptr, index);

	return result;
}

char strview_pop_first_char(strview_t* strview_ptr)
{
	char result = 0;
	if(strview_ptr && strview_ptr->size)
		result = pop_split(strview_ptr, 1).data[0];
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

		result = strview_split_first_delimeter(&src, cstr("\r\n"));

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

long long strview_to_ll(strview_t str)
{
	unsigned long long magnitude = 0;
	bool is_neg = false;

	if(str.data)
	{
		str = strview_trim(str, cstr(" "));
		
		if(str.size)
		{
			if(str.data[0] == '+')
				str = strview_sub(str, 1, INT_MAX);
			else if(str.data[0] == '-')
			{
				is_neg = true;
				str = strview_sub(str, 1, INT_MAX);
			};
		};

		magnitude = strview_to_ull(str);
	};

	return is_neg ? magnitude*-1 : magnitude;
}

unsigned long long strview_to_ull(strview_t str)
{
	long long result = 0;
	strview_t base_str;

	if(str.data)
	{
		str = strview_trim(str, cstr(" "));

		base_str = strview_sub(str, 0, 2);
		if( strview_is_match(base_str, cstr("0x"))
		||	strview_is_match(base_str, cstr("0X")))
			result = interpret_hex(strview_sub(str, 2,INT_MAX));

		else if(strview_is_match(base_str, cstr("0b")))
			result = interpret_bin(strview_sub(str, 2,INT_MAX));

		else
			result = interpret_dec(str);
	};

	return result;
}

strview_float_t strview_to_float(strview_t str)
{
	strview_float_t result = 0;
	bool is_neg = false;

	str = strview_trim(str, cstr(" "));

	if(!str.size)
		result = NAN;
	else if(strview_is_match_nocase(str, cstr("nan")))
		result = NAN;
	else if(str.data[0] == '+')
		str = strview_sub(str, 1, INT_MAX);
	else if(str.data[0] == '-')
	{
		is_neg = true;
		str = strview_sub(str, 1, INT_MAX);
	};
	if(strview_is_match_nocase(str, cstr("inf")))
		result = INFINITY;
	else if(result == 0)
		result = interpret_float(str);

	if((result == result) && is_neg)
		result *= -1;

	return result;
}

strview_t strview_split_left_of_view(strview_t* strview_ptr, strview_t pos)
{
	strview_t result = STRVIEW_INVALID;
	if(strview_ptr && strview_is_valid(*strview_ptr) && strview_is_valid(pos))
	{
		if(strview_ptr->data <= pos.data && pos.data <= &strview_ptr->data[strview_ptr->size])
			result = pop_split(strview_ptr, (int)(pos.data - strview_ptr->data));
	};
	return result;
}

strview_t strview_split_right_of_view(strview_t* strview_ptr, strview_t pos)
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
			result = pop_split(&src, (int)(split_point - src.data));
			strview_swap(&result, &src);
		};
		*strview_ptr = src;
	};

	return result;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static unsigned long long interpret_hex(strview_t str)
{
	unsigned long long result = 0;

	while(str.size && isxdigit(str.data[0]))
	{
		result <<= 4;
		if(isalpha(str.data[0]))
			result += 10 + (str.data[0] & 0x4F) - 'A';
		else
			result += str.data[0] & 0x0F;
		str = strview_sub(str, 1, INT_MAX);
	};

	return result;
}

static unsigned long long interpret_bin(strview_t str)
{
	unsigned long long result = 0;

	while(str.size && (str.data[0]=='0' || str.data[0]=='1'))
	{
		result <<= 1;
		result |= str.data[0] == '1';
		str = strview_sub(str, 1, INT_MAX);
	};

	return result;
}

static unsigned long long interpret_dec(strview_t str)
{
	unsigned long long result = 0;
	
	while(str.size && isdigit(str.data[0]))
	{
		result *= 10;
		result += str.data[0] & 0x0F;
		str = strview_sub(str, 1, INT_MAX);
	};

	return result;
}

#ifndef STR_NO_FLOAT
static strview_float_t interpret_float(strview_t str)
{
	strview_float_t result = 0;
	strview_float_t fractional_digit_weight = 1;
	bool digit_found = false;
	int exponent;

	digit_found |= str.size && isdigit(str.data[0]);
	while(str.size && isdigit(str.data[0]))
	{
		result *= 10;
		result += str.data[0] & 0x0F;
		str = strview_sub(str, 1, INT_MAX);
	};

	if(str.size && str.data[0]=='.')
	{
		str = strview_sub(str, 1, INT_MAX);

		digit_found |= str.size && isdigit(str.data[0]);
		while(str.size && isdigit(str.data[0]))
		{
			fractional_digit_weight /= 10.0;
			result += (str.data[0] & 0x0F) * fractional_digit_weight;
			str = strview_sub(str, 1, INT_MAX);
		};
	};

	if(str.size && toupper(str.data[0])=='E')
	{
		str = strview_sub(str, 1, INT_MAX);
		exponent = (int)strview_to_ll(str);
		#ifdef STR_SUPPORT_FLOAT
			result *= powf(10, exponent);
		#elif defined STR_SUPPORT_LONG_DOUBLE
			result *= powl(10, exponent);
		#else
			result *= pow(10, exponent);
		#endif
	};

	return digit_found ? result:NAN;
}
#endif

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

static strview_t pop_first_split(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive)
{
	strview_t result;
	bool found = false;
	const char* ptr;
	
	ptr = strview_ptr->data;

	if(strview_ptr->data && delimiters.data)
	{
		// try to find the delimiter
		while(ptr != &strview_ptr->data[strview_ptr->size] && !found)
		{
			found = contains_char(delimiters, *ptr, case_sensetive);
			ptr += !found;
		};
	};

	if(found)
	{
		result.data = strview_ptr->data;
		result.size = ptr - strview_ptr->data;
		strview_ptr->data = ptr;
		strview_ptr->size -= result.size;

		// at this stage, the remainder still includes the delimiter
		strview_ptr->size--;
		if(strview_ptr->size)	//only point to the character after the delimiter if there is one
			strview_ptr->data++;
	}
	else
	{
		result = *strview_ptr;
		*strview_ptr = STRVIEW_INVALID;
	};

	return result;
}

static strview_t pop_last_split(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive)
{
	strview_t result;
	bool found = false;
	const char* ptr;

	if(strview_ptr->data && strview_ptr->size && delimiters.data)
	{
		// starting from the last character, try to find the delimiter backwards
		ptr = &strview_ptr->data[strview_ptr->size-1];
		while(ptr != strview_ptr->data-1 && !found)
		{
			found = contains_char(delimiters, *ptr, case_sensetive);
			ptr -= !found;
		};
	};

	if(found)
	{
		result.data = ptr;
		result.size = &strview_ptr->data[strview_ptr->size] - ptr;
		strview_ptr->size -= result.size;

		// at this stage, the result still starts with the delimiter
		result.size--;
		if(result.size)
			result.data++; //only point to the the character after the delimiter if there is one
	}
	else
	{
		result = *strview_ptr;
		*strview_ptr = STRVIEW_INVALID;
	};

	return result;
}

static strview_t pop_split(strview_t* strview_ptr, int index)
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
