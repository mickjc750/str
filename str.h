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

//	Search result
	typedef struct str_search_result_t
	{
		bool found;
		size_t index;
	} str_search_result_t;

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

/*	Return the sub string indexed by begin->end. end is non-inclusive.
	Negative values may be used, and will index from the end of the string backwards.
	The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string */
	str_t str_sub(str_t str, int begin, int end);

//	Return a str_t with the start and end trimmed of all characters present in chars_to_trim
	str_t str_trim(str_t str, str_t chars_to_trim);

/*	Return the search result (bool found & index) for the first occurrence of needle in haystack
	If needle is valid, and of length 0, it will always be found at the start of the string.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	str_search_result_t str_find_first(str_t haystack, str_t needle);

/*	Return the search result (bool found & index) for the last occurrence of needle in haystack
	If needle is valid, and of length 0, it will always be found at the index of the last character in haystack+1.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	str_search_result_t str_find_last(str_t haystack, str_t needle);

/*	Return a str_t representing the contents of the source string up to, but not including, any of the delimiters.
	Additionally this text, and the delimeter itself is removed (popped) from the source string.
	If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid() */
	str_t str_pop_first_split(str_t* str_ptr, str_t delimiters);

/*	Return a str_t representing the contents of the source string from (but not including) the last delimiter found.
	Additionally this text, and the delimeter itself is removed (popped) from the end of the source string.
	If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid() */
	str_t str_pop_last_split(str_t* str_ptr, str_t delimiters);

//	Convert number to long long
	long long str_to_ll(str_t str);

//	Convert number to unsigned long long
	unsigned long long str_to_ull(str_t str);

#endif

