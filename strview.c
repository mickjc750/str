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

	#define BASE_PREFIX_LEN	2

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static bool contains_char(strview_t str, char c, bool case_sensetive);

	static strview_t split_first_delimeter(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive);
	static strview_t split_last_delimeter(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive);
	static strview_t split_index(strview_t* strview_ptr, int index);

	static int consume_signed(long long* dst, strview_t* src, int options, long long limit_min, long long limit_max);
	static int consume_unsigned(unsigned long long* dst, strview_t* src, int options, unsigned long long limit_max);

	void consume_base_prefix(int* base, strview_t* src, int options);
	static int consume_digits(unsigned long long* dst, strview_t *src, int base);
	static int consume_decimal_digits(unsigned long long* dst, strview_t* str);
	static int consume_hex_digits(unsigned long long* dst, strview_t* str);
	static int consume_bin_digits(unsigned long long* dst, strview_t* str);

	static bool upper_nibble_ull_is_zero(unsigned long long i);
	static bool upper_bit_ull_is_zero(unsigned long long i);

	static int xdigit_value(char c);
	static bool isbdigit(char c);

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
		result = split_first_delimeter(strview_ptr, delimiters, CASE_SENSETIVE);

	return result;
}

strview_t strview_split_first_delimiter_nocase(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_first_delimeter(strview_ptr, delimiters, NOT_CASE_SENSETIVE);

	return result;
}

strview_t strview_split_last_delimeter(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_last_delimeter(strview_ptr, delimiters, CASE_SENSETIVE);

	return result;
}

strview_t strview_split_last_delimeter_nocase(strview_t* strview_ptr, strview_t delimiters)
{
	strview_t result = STRVIEW_INVALID;
	
	if(strview_ptr)
		result = split_last_delimeter(strview_ptr, delimiters, NOT_CASE_SENSETIVE);

	return result;
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

int strview_consume_uchar(unsigned char* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, UCHAR_MAX);
	if(!err && dst)
		*dst = (unsigned char)val;

	return err;
}

int strview_consume_ushort(unsigned short* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, USHRT_MAX);
	if(!err && dst)
		*dst = (unsigned short)val;

	return err;
}

int strview_consume_uint(unsigned int* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, UINT_MAX);
	if(!err && dst)
		*dst = (unsigned int)val;

	return err;
}

int strview_consume_ulong(unsigned long* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, ULONG_MAX);
	if(!err && dst)
		*dst = (unsigned long)val;

	return err;
}

int strview_consume_ullong(unsigned long long* dst, strview_t* src, int options)
{
	return consume_unsigned(dst, src, options, ULLONG_MAX);
}

int strview_consume_char(char* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, CHAR_MIN, CHAR_MAX);
	if(!err && dst)
		*dst = (char)val;

	return err;
}

int strview_consume_short(short* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, SHRT_MIN, SHRT_MAX);
	if(!err && dst)
		*dst = (short)val;

	return err;
}

int strview_consume_int(int* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, INT_MIN, INT_MAX);
	if(!err && dst)
		*dst = (int)val;

	return err;
}

int strview_consume_long(long* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, LONG_MIN, LONG_MAX);
	if(!err && dst)
		*dst = (long)val;

	return err;
}

int strview_consume_llong(long long* dst, strview_t* src, int options)
{
	return consume_signed(dst, src, options, LLONG_MIN, LLONG_MAX);
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
			result = split_index(strview_ptr, (int)(pos.data - strview_ptr->data));
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

static int consume_signed(long long* dst, strview_t* src, int options, long long limit_min, long long limit_max)
{
	int err = 0;
	bool is_neg = false;
	strview_t num = STRVIEW_INVALID;
	strview_t base_prefix;
	strview_t view;
	int base = 10;
	unsigned long long val_ull;
	unsigned long long val_ll;
	char c;

	if(options & STR_BASE_BIN)
		base = 2;
	else if(options & STR_BASE_HEX)
		base = 16;

	if(src)
		num = *src;

	if(!strview_is_valid(num))
		err = EINVAL;

	//consume whitespace
	if(!err && !(options & STR_NOSPACE))
		num = strview_trim_start(num, cstr(" "));

	//consume sign
	if(!err && !(options & STR_NOSIGN))
	{
		if(num.data[0] == '-')
		{
			is_neg = true;
			strview_pop_first_char(&num);
		}
		else if(num.data[0] == '+')
			strview_pop_first_char(&num);
	};

	//consume base prefix and digits
	if(!err)
	{
		consume_base_prefix(&base, &num, options);
		err = consume_digits(&val_ull, &num, base);
	};

	//check if over range for signed ll before applying sign
	if(!err)
	{
		if(val_ull > (unsigned long long)LLONG_MAX)
			err = ERANGE;
		else if(is_neg)
			val_ll = val_ull * -1;
		else
			val_ll = val_ull;
	};

	//check if within range of destination type
	if(!err && !(limit_min <= val_ll && val_ll <= limit_max))
		err = ERANGE;

	//provide output
	if(!err)
	{
		if(dst)
			*dst = val_ll;
		if(src)
			*src = num;
	};		

	return err;
}

static int consume_unsigned(unsigned long long* dst, strview_t* src, int options, unsigned long long limit_max)
{
	int err = 0;
	strview_t num = STRVIEW_INVALID;
	strview_t base_prefix;
	strview_t view;
	int base = 10;
	unsigned long long val_ull;
	char c;

	if(options & STR_BASE_BIN)
		base = 2;
	else if(options & STR_BASE_HEX)
		base = 16;

	if(src)
		num = *src;

	if(!strview_is_valid(num))
		err = EINVAL;

	//consume whitespace
	if(!err && !(options & STR_NOSPACE))
		num = strview_trim_start(num, cstr(" "));

	//consume base prefix and digits
	if(!err)
	{
		consume_base_prefix(&base, &num, options);
		err = consume_digits(&val_ull, &num, base);
	};

	//check if within range of destination type
	if(!err && val_ull > limit_max)
		err = ERANGE;

	//provide output
	if(!err)
	{
		if(dst)
			*dst = val_ull;
		if(src)
			*src = num;
	};		

	return err;
}

static void consume_base_prefix(int* base, strview_t* src, int options)
{
	strview_t base_prefix = strview_sub(*src, 0, BASE_PREFIX_LEN);

	if(!(options & STR_NOBX))
	{
		if(!(options & STR_BASE_HEX) && strview_is_match_nocase(base_prefix, cstr("0b")))
		{
			*src = strview_sub(*src, BASE_PREFIX_LEN, INT_MAX);
			base = 2;
		}
		else if(!(options & STR_BASE_BIN) && strview_is_match_nocase(base_prefix, cstr("0x")))
		{
			*src = strview_sub(*src, BASE_PREFIX_LEN, INT_MAX);
			base = 16;
		};
	};
}

static int consume_digits(unsigned long long* dst, strview_t *src, int base)
{
	int err;
	if(base == 10)
		err = consume_decimal_digits(dst, src);
	else if(base == 16)
		err = consume_hex_digits(dst, src);
	else
		err = consume_bin_digits(dst, src);
	return err;
}

static int consume_decimal_digits(unsigned long long* dst, strview_t* str)
{
	#define UI_LIMIT 	((unsigned int)(UINT_MAX/10-9))
	#define UL_LIMIT 	((unsigned long)(ULONG_MAX/10-9))
	#define ULL_LIMIT 	((unsigned long long)(ULLONG_MAX/10-9))

	int err;
	unsigned int res_ui = 0;
	unsigned long res_ul;
	unsigned long long res_ull;

	if(str->size && isdigit(str->data[0]))
		err = 0;
	else
		err = EINVAL;

	if(!err)
	{
		*str = strview_trim_start(*str, cstr("0"));
		while(str->size && isdigit(str->data[0]) && res_ui < UI_LIMIT)
		{
			res_ui *= 10;
			res_ui += strview_pop_first_char(str) & 0x0F;
		};
		res_ul = res_ui;
		while(str->size && isdigit(str->data[0]) && res_ul < UL_LIMIT)
		{
			res_ul *= 10;
			res_ul += strview_pop_first_char(str) & 0x0F;
		};
		res_ull = res_ul;
		while(str->size && isdigit(str->data[0]) && res_ull < ULL_LIMIT)
		{
			res_ull *= 10;
			res_ull += strview_pop_first_char(str) & 0x0F;
		};
		if(str->size && isdigit(str->data[0]))
			err = ERANGE;
	};

	if(!err && dst)
		*dst = res_ull;

	#undef UI_LIMIT
	#undef UL_LIMIT
	#undef ULL_LIMIT
	return err;
}

static int consume_hex_digits(unsigned long long* dst, strview_t* str)
{
	unsigned long long result = 0;
	int err;

	if(str->size && isxdigit(str->data[0]))
		err = 0;
	else
		err = EINVAL;

	if(!err)
	{
		while(str->size && isxdigit(str->data[0]))
		{
			if(!upper_nibble_ull_is_zero(result))
				err = ERANGE;
			result <<= 4;
			result += xdigit_value(strview_pop_first_char(str));
		};
	};

	if(!err && dst)
		*dst = result;

	return err;
}

static int consume_bin_digits(unsigned long long* dst, strview_t* str)
{
	unsigned long long result = 0;
	int err;

	if(str->size && isbdigit(str->data[0]))
		err = 0;
	else
		err = EINVAL;

	if(!err)
	{
		while(str->size && isbdigit(str->data[0]))
		{
			if(!upper_bit_ull_is_zero(result))
				err = ERANGE;
			result <<= 1;
			result |= strview_pop_first_char(str) & 1;
		};
	};

	if(!err && dst)
		*dst = result;

	return result;
}

static bool upper_nibble_ull_is_zero(unsigned long long i)
{
	return !(i & (0x0Full << (sizeof(unsigned long long)*8-4)));
}

static bool upper_bit_ull_is_zero(unsigned long long i)
{
	return !(i & (0x01ull << (sizeof(unsigned long long)*8-1)));
}

static int xdigit_value(char c)
{
	c &= 0x5F;
	if(c & 0x40)
		c += 9;
	return c & 0x0F;
}

static bool isbdigit(char c)
{
	return (c & 0x7E) == 0x30;
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

static strview_t split_first_delimeter(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive)
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

static strview_t split_last_delimeter(strview_t* strview_ptr, strview_t delimiters, bool case_sensetive)
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
