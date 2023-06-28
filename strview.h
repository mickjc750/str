/**
 * @file strview.h
 * @brief A string view API for navigating and parsing string data.
 * 
 * strview.h declares the type strview_t, which is a pointer+size pair, or a 'view' of some existing string data.
 * 
 * Functions are provided for searching, comparing, trimming, and splitting portions of const char string data.
 * 
 * strview.h may be used standalone, and does not depend on **strbuf.h**.
 * 
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

/**
 * @def PRIstr
 * @brief (macro) a printf placeholder for a string view.
 * @note In the style of <inttypes.h> PRIstr should be concatenated with the format string literal.
 * @note The corresponding strview_t argument must also be wrapped in PRIstrarg()
 * @note Example:
 * @code{.c}
 * printf("The view is %"PRIstr"\n", PRIstrarg(my_view));
 * @endcode
  **********************************************************************************/ 
	#define PRIstr	".*s"


/**
 * @def PRIstrarg(arg)
 * @brief (macro) a wrapper for passing string views as arguments to printf.
 * @param arg A strview_t argument.
 * @note This macro will expand the argument twice, so avoid side affects in the argument.
 * @note Example:
 * @code{.c}
 * printf("The view is %"PRIstr"\n", PRIstrarg(my_view));
 * @endcode
  **********************************************************************************/ 
	#define PRIstrarg(arg)	((arg).size),((arg).data)

/**
 * @struct strview_t
 * @brief A view of some string data
 **********************************************************************************/
	typedef struct strview_t
	{
		const char* data;
		int size;
	} strview_t;


/**
 * @def cstr_SL(sl_arg)
 * @brief (macro) Provides a view of a string literal, without needing to measure it's length as runtime.
 * @param sl_arg A string literal.
 * @note Only use this to wrap "string literals", it will not work with a const char* due to pointer decay.
 * @note To get a view of a const char* or a char* use instead the cstr() function.
 * @note Example:
 * @code{.c}
 * strview_t title_view = cstr_SL("My Title");
 * @endcode
 **********************************************************************************/
	#define cstr_SL(sl_arg) ((strview_t){.data=(sl_arg), .size=sizeof(sl_arg)-1})


/**
 * @def STRVIEW_INVALID
 * @brief (macro) An invalid string view.
 * @note May be used as an initializer or assignment to a strview_t to invalidate it.
 **********************************************************************************/
	#define STRVIEW_INVALID		((strview_t){.data = NULL, .size = 0})

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief View a null terminated C string.
 * @param c_str The C string to view
 * @return A view of the C string, or STRVIEW_INVALID if c_str is passed NULL.
 * @note Measures the length of the string at runtime. To avoid this when viewing string literals use instead cstr_SL().
 * @note Example:
 * @code{.c}
 * char some_string[] = "Hello World";
 * strview_t some_view = cstr(some_string);
 * @endcode
  **********************************************************************************/
	strview_t cstr(const char* c_str);

/**
 * @brief Write a view to a null terminated C string.
 * @param dst The destination address.
 * @param dst_size The number of bytes available at the destination address.
 * @param str The source view to write to the destination.
 * @return The written C string at the destination.
 * @note Example:
 * @code{.c}
 * char some_string[50];
 * strview_t some_view = cstr_SL("Hello World");
 * strview_to_cstr(some_string, sizeof(some_string), some_view);
 * @endcode
  **********************************************************************************/
	char* strview_to_cstr(char* dst, size_t dst_size, strview_t str);

/**
 * @brief Test if a view is valid.
 * @param str The view to test.
 * @return true if the view is valid, or false if it is not.
  **********************************************************************************/
	bool strview_is_valid(strview_t str);

/**
 * @brief Swap two views.
 * @note This does not move any data, only the two views are swapped.
  **********************************************************************************/
	void strview_swap(strview_t* a, strview_t* b);

/**
 * @brief Test if the contents of two views match.
 * @return true if the contents match or if both views are invalid.
  **********************************************************************************/
	bool strview_is_match(strview_t str1, strview_t str2);

/**
 * @brief Test of the contents of two views match, ignoring case.
 * @return true if the contents match or if both views are invalid.
  **********************************************************************************/
	bool strview_is_match_nocase(strview_t str1, strview_t str2);

/**
 * @brief Test the starting contents of a view.
 * @param str1 The view to test.
 * @param str2 A view of the content we are testing for.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Example:
 * @code{.c}
 * strview_t target_view = cstr_SL("Hello World");
 * strview_t keyword_view = cstr_SL("Hello");
 * bool target_starts_with_greeting = strview_starts_with(target_view, keyword_view);
 * @endcode
  **********************************************************************************/
	bool strview_starts_with(strview_t str1, strview_t str2);

/**
 * @brief Test the starting contents of a view, ignoring case.
 * @param str1 The view to test.
 * @param str2 A view of the content we are testing for.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Example:
 * @code{.c}
 * strview_t target_view = cstr_SL("Hello World");
 * strview_t keyword_view = cstr_SL("heLLo");
 * bool target_starts_with_greeting = strview_starts_with_nocase(target_view, keyword_view);
 * @endcode
  **********************************************************************************/
	bool strview_starts_with_nocase(strview_t str1, strview_t str2);

/**
 * @brief Alphabetical comparison of two views.
 * @return >0 if the first non-matching character in str1 is greater than that of str2.
 * @return <0 if the first non-matching character in str1 is less than that of str2.
 * @return 0 if the views are equal.
  **********************************************************************************/
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
	strview_t strview_split_first_delimeter(strview_t* src, strview_t delimiters);

//	Same as strview_split_first_delimeter, ignoring case on the delimiters
	strview_t strview_split_first_delimiter_nocase(strview_t* src, strview_t delimiters);

/*	Return a strview_t representing the contents of the source string from (but not including) the last delimiter found.
	Additionally this text, and the delimeter itself is removed (popped) from the end of the source string.
	If no delimeter is found the returned string is the entire source string, and the source string becomes invalid */
	strview_t strview_split_last_delimeter(strview_t* src, strview_t delimiters);

//	Same as strview_split_last_delimeter, ignoring case on the delimiters
	strview_t strview_split_last_delimeter_nocase(strview_t* src, strview_t delimiters);

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
	strview_t strview_split_index(strview_t* src, int index);

/*	Split a strview_t at a position specified by pos

	If pos references characters within *src, return a strview_t representing all characters to the left of pos.
	If pos references the upper limit of *src, the entire *src is returned.
	If pos references the start of *src, a valid strview_t of length 0 is returned.

	The returned characters are removed (popped) from *src

	If src is NULL, *src is invalid, or pos is not a valid reference, an invalid string is returned and src is unmodified.*/
	strview_t strview_split_left(strview_t* src, strview_t pos);


/*	Split a strview_t at a position specified by pos

	If pos references characters within *src, return a strview_t representing all characters to the right of pos.
	If the upper limit of pos matches the upper limit of *src, a valid strview_t of length 0 is returned.

	The returned characters are removed (popped) from *src

	If src is NULL, *src is invalid, or pos is not a valid reference, an invalid string is returned and src is unmodified.*/
	strview_t strview_split_right(strview_t* src, strview_t pos);

/*	Return the first char of str, and remove it from the str.
	Returns 0 if there are no characters in str.
	If str is known to contain at least one character, it is the equivalent of:
		strview_split_index(&str, 1).data[0]
	Only it avoids dereferencing a NULL pointer in the case where strview_split_index() returns an invalid str.
*/
	char strview_pop_first_char(strview_t* src);


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
	strview_t strview_split_line(strview_t* src, char* eol);


#endif

