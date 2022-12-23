/*/
 The following may be added to compiler options:

	The default precision for floating point conversions id double.
	If you with to change this the following options are available.

	-DSTR_NO_FLOAT
		Don't provide floating point conversions. str.c will not need linking against the math library (-lm)
	
	-DSTR_SUPPORT_LONG_DOUBLE
		Convert floating point values with long double precision.

	-DSTR_SUPPORT_FLOAT
		Convert floating point values with float precision.

*/

#ifndef _STRVIEW_H_
	#define _STRVIEW_H_

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
	#define PRIstrarg(arg)	((arg).size),((arg).data)

//	String structure.
//	This does not own the memory used to hold the string. It references data either in a string buffer, or const chars in the string pool.
	typedef struct strview_t
	{
		const char* data;
		int size;
	} strview_t;

//	Can be used instead of cstr, to avoid measuring the length of string literals at runtime
	#define cstr_SL(sl_arg) ((strview_t){.data=(sl_arg), .size=sizeof(sl_arg)-1})

//	Assign to a strview_t to make it invalid
	#define STR_INVALID		((strview_t){.data = NULL, .size = 0})	

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	Return a strview_t from a null terminated const char string.
//	If the argument is a string literal, cstr_SL() may be used instead, to prevent traversing the string literal to measure it's length
	strview_t cstr(const char* c_str);

//	Write a str out to a null terminated char* buffer.
//	The buffer memory and it's size must be provided by the caller
	char* strview_to_cstr(char* dst, size_t dst_size, strview_t str);
	
//	Return true if the strview_t is valid.
	bool strview_is_valid(strview_t str);

//	Swap strings a and b
	void strview_swap(strview_t* a, strview_t* b);

//	Return true if the strings match
	bool strview_is_match(strview_t str1, strview_t str2);

//	Same as strview_is_match() ignoring case
	bool strview_is_match_nocase(strview_t str1, strview_t str2);

/*	Replaces strcmp()
	returns >0 if the first non-matching character in str1 is greater (in ASCII) than that of str2.
	returns <0 if the first non-matching character in str1 is lower   (in ASCII) than that of str2.
	returns 0 if the strings are equal */
	int strview_compare(strview_t str1, strview_t str2);

/*	Return the sub string indexed by begin->end. end is non-inclusive.
	Negative values may be used, and will index from the end of the string backwards.
	The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string */
	strview_t strview_sub(strview_t str, int begin, int end);

//	Return a strview_t with the start and end trimmed of all characters present in chars_to_trim
	strview_t strview_trim(strview_t str, strview_t chars_to_trim);

//	Return a strview_t with the start trimmed of all characters present in chars_to_trim
	strview_t strview_trim_start(strview_t str, strview_t chars_to_trim);

//	Return a strview_t with the end trimmed of all characters present in chars_to_trim
	strview_t strview_trim_end(strview_t str, strview_t chars_to_trim);

/*	Return the strview_t for the first occurrence of needle in haystack.
 	If the needle is not found, strview_find_first() returns an invalid strview_t.
 	If the needle is found, the returned strview_t will match the contents of needle, only it will reference data within the haystack, and can be used with various strbuf.h functions as a means of specifying the position within the buffer. */
	strview_t strview_find_first(strview_t haystack, strview_t needle);

/*	Similar to strview_find_first(), but returns the LAST occurrence of needle in haystack.
	If needle is valid, and of length 0, it will always be found at the end of haystack.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	strview_t strview_find_last(strview_t haystack, strview_t needle);

/*	Return a strview_t representing the contents of the source string up to, but not including, any of the delimiters.
	Additionally this text, and the delimeter itself is removed (popped) from the source string.
	If no delimeter is found, the returned string is the entire source string, and the source string becomes invalid */
	strview_t strview_pop_first_split(strview_t* strview_ptr, strview_t delimiters);

//	Same as strview_pop_first_split, ignoring case on the delimiters
	strview_t strview_pop_first_split_nocase(strview_t* strview_ptr, strview_t delimiters);

/*	Return a strview_t representing the contents of the source string from (but not including) the last delimiter found.
	Additionally this text, and the delimeter itself is removed (popped) from the end of the source string.
	If no delimeter is found the returned string is the entire source string, and the source string becomes invalid */
	strview_t strview_pop_last_split(strview_t* strview_ptr, strview_t delimiters);

//	Same as strview_pop_last_split, ignoring case on the delimiters
	strview_t strview_pop_last_split_nocase(strview_t* strview_ptr, strview_t delimiters);

/*	Split a strview_t at a specified index n.
	For n >= 0
		Return a strview_t representing the first n characters of the source string.
		Additionally the first n characters are removed (popped) from the start of the source string.
		If n is greater than the size of the source string ALL characters will be popped.

	For n < 0
		Return a strview_t representing the last -n characters of the source string.
		Additionally the last -n characters are removed (popped) from the end of the source string.
		If -n is greater than the size of the source string ALL characters will be popped.
*/
	strview_t strview_pop_split(strview_t* strview_ptr, int index);

/*	Return the first char of str, and remove it from the str.
	Returns 0 if there are no characters in str.
	If str is known to contain at least one character, it is the equivalent of:
		strview_pop_split(&str, 1).data[0]
	Only it avoids dereferencing a NULL pointer in the case where strview_pop_split() returns an invalid str.
*/
	char strview_pop_first_char(strview_t* strview_ptr);


/*
	Returns a strview_t representing the first line within the source string, not including the eol terminator.
	The returned line and the terminator are removed (popped) from the source string.
	If a line terminator is not found, an invalid strview_t is returned and the source string is unmodified.

	If the source string already contains one or more lines:
		Any mixture of (CR,LF,CRLF,LFCR) can be handled, a CRLF or LFCR sequence will always be interpreted as 1 line ending.
	In this case eol may be NULL.

	If the source string is being appended to one character at a time, such as when gathering user input:
		Any type of line ending can be handled by providing variable eol.
		This variable stores the state of the eol discriminator, regarding if a future CR or LF needs to be ignored.
		It's initial value should be 0.
*/
	strview_t strview_pop_line(strview_t* strview_ptr, char* eol);


//	Convert number to long long
	long long strview_to_ll(strview_t str);

//	Convert number to unsigned long long
	unsigned long long strview_to_ull(strview_t str);

/*	Convert number to float
	By default strview_to_float() works with and returns double
	To support long double define STR_SUPPORT_LONG_DOUBLE
	Or, to not supprt float conversions at all, define STR_NO_FLOAT*/
	#ifndef STR_NO_FLOAT
		#ifdef STR_SUPPORT_LONG_DOUBLE
			typedef long double strview_float_t;
		#elif defined STR_SUPPORT_FLOAT
			typedef float strview_float_t;
		#else
			typedef double strview_float_t;
		#endif
		strview_float_t strview_to_float(strview_t str);
	#endif

//	Other standard sizes just cast the return value
	#define strview_to_int(str) 		((int)strview_to_ll(str))
	#define strview_to_int8_t(str) 		((int8_t)strview_to_ll(str))
	#define strview_to_int16_t(str) 	((int16_t)strview_to_ll(str))
	#define strview_to_int32_t(str) 	((int32_t)strview_to_ll(str))
	#define strview_to_int64_t(str) 	((int64_t)strview_to_ll(str))
	#define strview_to_unsigned(str)	((unsigned)strview_to_ull(str))
	#define strview_to_uint8_t(str) 	((uint8_t)strview_to_ull(str))
	#define strview_to_uint16_t(str) 	((uint16_t)strview_to_ull(str))
	#define strview_to_uint32_t(str) 	((uint32_t)strview_to_ull(str))
	#define strview_to_uint64_t(str) 	((uint64_t)strview_to_ull(str))

#endif

