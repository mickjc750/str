#ifndef _STR_H_
	#define _STR_H_

	#include <stddef.h>
	#include <stdbool.h>
	#include <stdarg.h>
	#include <string.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//	These macros can be used with printf for printing str types using dynamic precision.
//	eg.  printf("name=%"PRIstr"\n", PRIstrarg(myname));
	#define PRIstr	".*s"
	#define PRIstrarg(arg)	(int)((arg).size),((arg).data)

//	String structure.
//	This does not own the memory used to hold the string. It references data either in a string buffer, or const chars in the string pool.
	typedef struct str_t
	{
		const char* data;
		size_t size;
	} str_t;

//	Can be used instead of cstr, to avoid measuring the length of string literals at runtime
	#define cstr_SL(sl_arg) ((str_t){.data=(sl_arg), .size=sizeof(sl_arg)-1})

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	Return a str_t from a null terminated const char string.
//	If the argument is a string literal, cstr_SL() may be used instead, to prevent traversing the string literal to measure it's length
	str_t cstr(const char* c_str);

//	Write a str out to a null terminated char* buffer.
//	The buffer memory and it's size must be provided by the caller
	char* str_to_cstr(char* dst, size_t dst_size, str_t str);
	
//	Return true if the str_t is valid.
// 	A str_t may be invalid if a delimiter is not found when attempting to split a string with str_pop_first_split() or str_pop_last_split()
	bool str_is_valid(str_t str);

//	Return true if the strings match
	bool str_is_match(str_t str1, str_t str2);

//	Same as str_is_match() ignoring case
	bool str_is_match_nocase(str_t str1, str_t str2);

/*	Replaces strcmp()
	returns >0 if the first non-matching character in str1 is greater (in ASCII) than that of str2.
	returns <0 if the first non-matching character in str1 is lower   (in ASCII) than that of str2.
	returns 0 if the strings are equal */
	int str_compare(str_t str1, str_t str2);

/*	Return the sub string indexed by begin->end. end is non-inclusive.
	Negative values may be used, and will index from the end of the string backwards.
	The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string */
	str_t str_sub(str_t str, int begin, int end);

//	Return a str_t with the start and end trimmed of all characters present in chars_to_trim
	str_t str_trim(str_t str, str_t chars_to_trim);

//	Return a str_t with the start trimmed of all characters present in chars_to_trim
	str_t str_trim_start(str_t str, str_t chars_to_trim);

//	Return a str_t with the end trimmed of all characters present in chars_to_trim
	str_t str_trim_end(str_t str, str_t chars_to_trim);

/*	Return the search result (bool found & index) for the first occurrence of needle in haystack
	If needle is valid, and of length 0, it will always be found at the start of the string.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	str_t str_find_first(str_t haystack, str_t needle);

/*	Return the search result (bool found & index) for the last occurrence of needle in haystack
	If needle is valid, and of length 0, it will always be found at the index of the last character in haystack+1.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	str_t str_find_last(str_t haystack, str_t needle);

/*	Return a str_t representing the contents of the source string up to, but not including, any of the delimiters.
	Additionally this text, and the delimeter itself is removed (popped) from the source string.
	If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid() */
	str_t str_pop_first_split(str_t* str_ptr, str_t delimiters);

//	Same as str_pop_first_split, ignoring case on the delimiters
	str_t str_pop_first_split_nocase(str_t* str_ptr, str_t delimiters);

/*	Return a str_t representing the contents of the source string from (but not including) the last delimiter found.
	Additionally this text, and the delimeter itself is removed (popped) from the end of the source string.
	If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid() */
	str_t str_pop_last_split(str_t* str_ptr, str_t delimiters);

//	Same as str_pop_last_split, ignoring case on the delimiters
	str_t str_pop_last_split_nocase(str_t* str_ptr, str_t delimiters);

/*	Split a str_t at a specified index n.
	For n >= 0
		Return a str_t representing the first n characters of the source string.
		Additionally the first n characters are removed (popped) from the start of the source string.
		If n is greater than the size of the source string ALL characters will be popped.

	For n < 0
		Return a str_t representing the last -n characters of the source string.
		Additionally the last -n characters are removed (popped) from the end of the source string.
		If -n is greater than the size of the source string ALL characters will be popped.
*/
	str_t str_pop_split(str_t* str_ptr, int index);

/*	Return the first char of str, and remove it from the str.
	Returns 0 if there are no characters in str.
	If str is known to contain at least one character, it is the equivalent of:
		str_pop_split(&str, 1).data[0]
	Only it avoids dereferencing a NULL pointer in the case where str_pop_split() returns an invalid str.
*/
	char str_pop_first_char(str_t* str_ptr);


/*
	Returns a str_t representing the first line within the source string, not including the eol terminator.
	The returned line and the terminator are removed (popped) from the source string.
	If a line terminator is not found, an invalid str_t is returned and the source string is unmodified.

	If the source string already contains one or more lines:
		Any mixture of (CR,LF,CRLF,LFCR) can be handled, a CRLF or LFCR sequence will always be interpreted as 1 line ending.
	In this case eol may be NULL.

	If the source string is being appended to one character at a time, such as when gathering user input:
		Any type of line ending can be handled by providing variable eol.
		This variable stores the state of the eol discriminator, regarding if a future CR or LF needs to be ignored.
		It's initial value should be 0.
*/
	str_t str_pop_line(str_t* str_ptr, char* eol);


//	Convert number to long long
	long long str_to_ll(str_t str);

//	Convert number to unsigned long long
	unsigned long long str_to_ull(str_t str);

/*	Convert number to float
	By default str_to_float() works with and returns double
	To support long double define STR_SUPPORT_LONG_DOUBLE
	Or, to not supprt float conversions at all, define STR_NO_FLOAT*/
	#ifndef STR_NO_FLOAT
		#ifdef STR_SUPPORT_LONG_DOUBLE
			typedef long double str_float_t;
		#elif defined STR_SUPPORT_FLOAT
			typedef float str_float_t;
		#else
			typedef double str_float_t;
		#endif
		str_float_t str_to_float(str_t str);
	#endif

//	Other standard sizes just cast the return value
	#define str_to_int(str) 		((int)str_to_ll(str))
	#define str_to_int8_t(str) 		((int8_t)str_to_ll(str))
	#define str_to_int16_t(str) 	((int16_t)str_to_ll(str))
	#define str_to_int32_t(str) 	((int32_t)str_to_ll(str))
	#define str_to_int64_t(str) 	((int64_t)str_to_ll(str))
	#define str_to_unsigned(str)	((unsigned)str_to_ull(str))
	#define str_to_uint8_t(str) 	((uint8_t)str_to_ull(str))
	#define str_to_uint16_t(str) 	((uint16_t)str_to_ull(str))
	#define str_to_uint32_t(str) 	((uint32_t)str_to_ull(str))
	#define str_to_uint64_t(str) 	((uint64_t)str_to_ull(str))

#endif

