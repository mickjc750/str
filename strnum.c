/*
*/

	#ifndef STRNUM_NOFLOAT
		#include <math.h>
	#endif
	#include <limits.h>
	#include <ctype.h>
	
	#include "strnum.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//	#include <stdio.h>
//	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	#define BASE_PREFIX_LEN	2

#ifndef STRNUM_NOFLOAT
	typedef struct float_components_t
	{
		int options;
		strview_t num;
		bool is_neg;
		float special_value;	// NAN, +INF, -INF, or 0.0 if not special
		strview_t integral_view;
		strview_t fractional_view;
		bool got_exponent;
		int exp_value;
	}float_components_t;
#endif

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static int consume_signed(long long* dst, strview_t* src, int options, long long limit_min, long long limit_max);
	static int consume_unsigned(unsigned long long* dst, strview_t* src, int options, unsigned long long limit_max);

	static bool consume_sign(strview_t* num);
	static void consume_base_prefix(int* base, strview_t* src, int options);

//	pop digits from source, until a non-digit is found, or the next digit would overflow an unsigned long long
	static int consume_digits(unsigned long long* dst, strview_t *src, int base);

	static int consume_decimal_digits(unsigned long long* dst, strview_t* str);
	static int consume_hex_digits(unsigned long long* dst, strview_t* str);
	static int consume_bin_digits(unsigned long long* dst, strview_t* str);

#ifndef STRNUM_NOFLOAT
	static int process_float_components(float_components_t* fc);
	static float consume_float_special(float_components_t* fc);
	static int consume_exponent(float_components_t* fc);

	static float make_float(strview_t integral_digits, strview_t fractional_digits, int exp_value);
	static double make_double(strview_t integral_digits, strview_t fractional_digits, int exp_value);
	static long double make_ldouble(strview_t integral_digits, strview_t fractional_digits, int exp_value);
#endif

	static bool upper_nibble_ull_is_zero(unsigned long long i);
	static bool upper_bit_ull_is_zero(unsigned long long i);

	static int xdigit_value(char c);
	static bool isbdigit(char c);

	static strview_t split_digits(strview_t* src);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int strnum_uchar(unsigned char* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, UCHAR_MAX);
	if(!err && dst)
		*dst = (unsigned char)val;

	return err;
}

int strnum_ushort(unsigned short* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, USHRT_MAX);
	if(!err && dst)
		*dst = (unsigned short)val;

	return err;
}

int strnum_uint(unsigned int* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, UINT_MAX);
	if(!err && dst)
		*dst = (unsigned int)val;

	return err;
}

int strnum_ulong(unsigned long* dst, strview_t* src, int options)
{
	int err;
	unsigned long long val;

	err = consume_unsigned(&val, src, options, ULONG_MAX);
	if(!err && dst)
		*dst = (unsigned long)val;

	return err;
}

int strnum_ullong(unsigned long long* dst, strview_t* src, int options)
{
	return consume_unsigned(dst, src, options, ULLONG_MAX);
}

int strnum_char(char* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, CHAR_MIN, CHAR_MAX);
	if(!err && dst)
		*dst = (char)val;

	return err;
}

int strnum_short(short* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, SHRT_MIN, SHRT_MAX);
	if(!err && dst)
		*dst = (short)val;

	return err;
}

int strnum_int(int* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, INT_MIN, INT_MAX);
	if(!err && dst)
		*dst = (int)val;

	return err;
}

int strnum_long(long* dst, strview_t* src, int options)
{
	int err;
	long long val;

	err = consume_signed(&val, src, options, LONG_MIN, LONG_MAX);
	if(!err && dst)
		*dst = (long)val;

	return err;
}

int strnum_llong(long long* dst, strview_t* src, int options)
{
	return consume_signed(dst, src, options, LLONG_MIN, LLONG_MAX);
}

#ifndef STRNUM_NOFLOAT
int strnum_float(float* dst, strview_t* src, int options)
{
	int err = 0;
	float value = 0.0;
	float_components_t fc =
	{
		.options = options,
		.num = STRVIEW_INVALID,
		.is_neg = false,
		.special_value = 0.0,
		.integral_view = STRVIEW_INVALID,
		.fractional_view = STRVIEW_INVALID,
		.exp_value = 0,
		.got_exponent=false
	};

	if(src)
	{
		fc.num = *src;
		err = process_float_components(&fc);
	};

	// calculate/determine output value
	if(!err && fc.special_value)
		value = fc.special_value;
	else if(!err)
	{
		value = make_float(fc.integral_view, fc.fractional_view, fc.exp_value);
		if(value == INFINITY)
			err = ERANGE;
	};

	if(!err && dst)
	{
		if(value != value)
			*dst = NAN;
		else
			*dst = fc.is_neg ? -value:value;
	};

	if(!err)
		*src = fc.num;

	return err;
}

int strnum_double(double* dst, strview_t* src, int options)
{
	int err = 0;
	double value;
	float_components_t fc =
	{
		.options = options,
		.num = STRVIEW_INVALID,
		.is_neg = false,
		.special_value = 0.0,
		.integral_view = STRVIEW_INVALID,
		.fractional_view = STRVIEW_INVALID,
		.exp_value = 0,
		.got_exponent=false
	};

	if(src)
		fc.num = *src;

	err = process_float_components(&fc);
	value = fc.special_value;

	// calculate/determine output value
	if(!err && fc.special_value)
		value = fc.special_value;
	else if(!err)
	{
		value = make_double(fc.integral_view, fc.fractional_view, fc.exp_value);
		if(value == INFINITY)
			err = ERANGE;
	};

	if(!err && dst)
	{
		if(value != value)
			*dst = NAN;
		else
			*dst = fc.is_neg ? -value:value;
	};

	if(!err)
		*src = fc.num;

	return err;
}

int strnum_ldouble(long double* dst, strview_t* src, int options)
{
	int err = 0;
	long double value;
	float_components_t fc =
	{
		.options = options,
		.num = STRVIEW_INVALID,
		.is_neg = false,
		.special_value = 0.0,
		.integral_view = STRVIEW_INVALID,
		.fractional_view = STRVIEW_INVALID,
		.exp_value = 0,
		.got_exponent=false
	};

	if(src)
		fc.num = *src;

	err = process_float_components(&fc);
	value = fc.special_value;

	// calculate/determine output value
	if(!err && fc.special_value)
		value = fc.special_value;
	else if(!err)
	{
		value = make_ldouble(fc.integral_view, fc.fractional_view, fc.exp_value);
		if(value == INFINITY)
			err = ERANGE;
	};

	if(!err && dst)
	{
		if(value != value)
			*dst = NAN;
		else
			*dst = fc.is_neg ? -value:value;
	};

	if(!err)
		*src = fc.num;

	return err;
}
#endif

//********************************************************************************************************
// Private functions
//********************************************************************************************************

#ifndef STRNUM_NOFLOAT
static int process_float_components(float_components_t* fc)
{
	int err = 0;
	fc->integral_view = STRVIEW_INVALID;
	fc->fractional_view = STRVIEW_INVALID;

	if(!strview_is_valid(fc->num))
		err = EINVAL;

	//consume whitespace
	if(!err && !(fc->options & STRNUM_NOSPACE))
		fc->num = strview_trim_start(fc->num, cstr(" "));

	//consume sign
	if(!err && !(fc->options & STRNUM_NOSIGN))
		fc->is_neg = consume_sign(&fc->num);

	//consume special cases inf and nan
	if(!err)
		fc->special_value = consume_float_special(fc);

	//consume views of integral and fractional digits (but don't convert them yet)
	if(!err && !fc->special_value)
	{
		fc->integral_view = split_digits(&fc->num);
		if(fc->num.size && fc->num.data[0]=='.')
		{
			strview_pop_first_char(&fc->num);
			fc->fractional_view = split_digits(&fc->num);
		};
		if(!strview_is_valid(fc->integral_view) && !strview_is_valid(fc->fractional_view))
			err = EINVAL;
	};

	//consume exponent
	if(!err && !fc->special_value && !(fc->options & STRNUM_NOEXP) && strview_starts_with_nocase(fc->num, cstr("E")))
		err = consume_exponent(fc);

	return err;
}

static float consume_float_special(float_components_t* fc)
{
	float value = 0.0;
	if(strview_starts_with_nocase(fc->num, cstr("infinty")))
	{
		value = INFINITY;
		fc->num = strview_sub(fc->num, strlen("infinity"), INT_MAX);
	}
	else if(strview_starts_with_nocase(fc->num, cstr("inf")))
	{
		value = INFINITY;
		fc->num = strview_sub(fc->num, strlen("inf"), INT_MAX);
	}
	else if(strview_starts_with_nocase(fc->num, cstr("nan")))
	{
		value = NAN;
		fc->num = strview_sub(fc->num, strlen("nan"), INT_MAX);
	};
	return value;
}

static int consume_exponent(float_components_t* fc)
{
	int err = 0;
	strview_t exp_view;
	exp_view = strview_sub(fc->num, 1, INT_MAX);
	err = strnum_int(&fc->exp_value, &exp_view, STRNUM_NOSPACE | STRNUM_NOBX);
	fc->got_exponent = (err == 0);
	if(fc->got_exponent)
		fc->num = exp_view;
	else if(err == EINVAL)
		err = 0;	// an invalid exponent (non-numeric) simply means we don't consume the exponent.
	return err;
}

#define make_float_type(typ, powfunc)											\
	typ result = 0.0;															\
	typ fractional_part;														\
	unsigned long long ull;														\
	int fractional_power = 0;													\
	while(integral_digits.size)													\
	{																			\
		consume_decimal_digits(&ull, &integral_digits);							\
		result += (typ)ull;														\
		result *= powfunc(10, integral_digits.size);							\
	};																			\
	fractional_digits = strview_trim_end(fractional_digits, cstr("0"));			\
	while(fractional_digits.size)												\
	{																			\
		fractional_power -= fractional_digits.size;								\
		consume_decimal_digits(&ull, &fractional_digits);						\
		fractional_power += fractional_digits.size;								\
		fractional_part = (typ)ull;												\
		fractional_part *= powfunc(10, fractional_power);						\
		result += fractional_part;												\
	};																			\
	result *= powfunc(10, exp_value);											\
	return result;

static float make_float(strview_t integral_digits, strview_t fractional_digits, int exp_value)
{
	make_float_type(float, powf)
}

static double make_double(strview_t integral_digits, strview_t fractional_digits, int exp_value)
{
	make_float_type(double, pow)
}

static long double make_ldouble(strview_t integral_digits, strview_t fractional_digits, int exp_value)
{
	make_float_type(long double, powl)
}

#endif

static int consume_signed(long long* dst, strview_t* src, int options, long long limit_min, long long limit_max)
{
	int err = 0;
	bool is_neg = false;
	strview_t num = STRVIEW_INVALID;
	int base = 10;
	unsigned long long val_ull;
	long long val_ll;

	if(options & STRNUM_BASE_BIN)
		base = 2;
	else if(options & STRNUM_BASE_HEX)
		base = 16;

	if(src)
		num = *src;

	if(!strview_is_valid(num))
		err = EINVAL;

	//consume whitespace
	if(!err && !(options & STRNUM_NOSPACE))
		num = strview_trim_start(num, cstr(" "));

	//consume sign
	if(!err && !(options & STRNUM_NOSIGN))
		is_neg = consume_sign(&num);

	//consume base prefix and digits
	if(!err)
	{
		consume_base_prefix(&base, &num, options);
		err = consume_digits(&val_ull, &num, base);
	};

	//check if over range for signed ll before applying sign
	if(!err)
	{
		if(is_neg)
		{
			if(val_ull > (unsigned long long)LLONG_MIN)
				err = ERANGE;
			else 
				val_ll = val_ull * -1;
		}
		else if(val_ull > LLONG_MAX)
			err = ERANGE;
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
	int base = 10;
	unsigned long long val_ull;

	if(options & STRNUM_BASE_BIN)
		base = 2;
	else if(options & STRNUM_BASE_HEX)
		base = 16;

	if(src)
		num = *src;

	if(!strview_is_valid(num))
		err = EINVAL;

	//consume whitespace
	if(!err && !(options & STRNUM_NOSPACE))
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

	if(!(options & STRNUM_NOBX))
	{
		if(!(options & STRNUM_BASE_HEX) && strview_is_match_nocase(base_prefix, cstr("0b")))
		{
			*src = strview_sub(*src, BASE_PREFIX_LEN, INT_MAX);
			*base = 2;
		}
		else if(!(options & STRNUM_BASE_BIN) && strview_is_match_nocase(base_prefix, cstr("0x")))
		{
			*src = strview_sub(*src, BASE_PREFIX_LEN, INT_MAX);
			*base = 16;
		};
	};
}

static bool consume_sign(strview_t* num)
{
	bool is_neg = false;
	if(num->size && num->data[0] == '-')
	{
		is_neg = true;
		strview_pop_first_char(num);
	}
	else if(num->size && num->data[0] == '+')
		strview_pop_first_char(num);
	return is_neg;
}

//	pop digits from source, until a non-digit is found, or the next digit would overflow an unsigned long long
//  return value is ERANGE if conversion stopped to prevent overflow, or EINVAL if no digits were found
//	dst is always written to with a value representing the digits converted, or 0 if no digits exist (EINVAL)
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
	unsigned long res_ul = 0;
	unsigned long long res_ull = 0;
	unsigned long long post_add;
	unsigned long long next_weight;

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
		while(str->size && isdigit(str->data[0]) && !err)
		{
			if(res_ull > ULLONG_MAX/10)	// if number is too big to multiply by 10, ERANGE
				err = ERANGE;
			else
			{
				next_weight = res_ull * 10;
				post_add = next_weight + (str->data[0] & 0x0F);
				if(post_add < next_weight)	// if digit addition results in a lower value, ERANGE
					err = ERANGE;
				else
				{	// else digit is consumed, and value is valid
					strview_pop_first_char(str);
					res_ull = post_add;
				};
			};
		};
	};

	if(dst)
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
	{
		err = 0;
		*str = strview_trim_start(*str, cstr("0"));
	}
	else
		err = EINVAL;

	while(str->size && isxdigit(str->data[0]) && !err)
	{
		if(!upper_nibble_ull_is_zero(result))
			err = ERANGE;
		else
		{
			result <<= 4;
			result += xdigit_value(strview_pop_first_char(str));
		};
	};

	if(dst)
		*dst = result;

	return err;
}

static int consume_bin_digits(unsigned long long* dst, strview_t* str)
{
	unsigned long long result = 0;
	int err;

	if(str->size && isbdigit(str->data[0]))
	{
		err = 0;
		*str = strview_trim_start(*str, cstr("0"));
	}
	else
		err = EINVAL;

	while(str->size && isbdigit(str->data[0]) && !err)
	{
		if(!upper_bit_ull_is_zero(result))
			err = ERANGE;
		else
		{
			result <<= 1;
			result |= strview_pop_first_char(str) & 1;
		};
	};

	if(dst)
		*dst = result;

	return err;
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

static strview_t split_digits(strview_t* src)
{
	strview_t result = STRVIEW_INVALID;
	if(src && src->size && isdigit(src->data[0]))
	{
		result.data = src->data;
		result.size = 0;
		while(src->size && isdigit(src->data[0]))
		{
			strview_pop_first_char(src);
			result.size++;
		};
	};
	return result;
}
