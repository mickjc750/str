/**
 * @file strview.h
 * @brief A string view API for navigating and parsing string data.
 * @author Michael Clift
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
 * @brief (macro) Provides a view of a string literal, without needing to measure it's length at runtime.
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


/**
 * @def  strview_is_match(strview_t str1, str2);
 * @brief (macro) Test if the contents of str1 matches the contents of str2.
 * @param str1 A view to compare against str2.
 * @param str2 Either a strview_t or a C string to compare against str1.
 * @return true if the contents match.
   **********************************************************************************/
	#define strview_is_match(str1, str2) _Generic((str2),\
		const char*:	strview_is_match_cstr,\
		char*:			strview_is_match_cstr,\
		strview_t:		strview_is_match_strview\
		)(str1, str2)


/**
 * @def  strview_is_match_nocase(strview_t str1, str2);
 * @brief (macro) Test if the contents of str1 matches the contents of str2.
 * @param str1 A view to compare against str2.
 * @param str2 Either a C string or a view to compare against str1.
 * @return true if the contents match.
   **********************************************************************************/
	#define strview_is_match_nocase(str1, str2) _Generic((str2),\
		const char*:	strview_is_match_nocase_cstr,\
		char*:			strview_is_match_nocase_cstr,\
		strview_t:		strview_is_match_nocase_strview\
		)(str1, str2)


/**
 * @def  strview_starts_with(strview_t str1, str2)
 * @brief (macro) Test the starting contents of a view.
 * @param str1 The view to test.
 * @param str2 Either a C string, or a view of the content we are testing for.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Example:
 * @code{.c}
 * strview_t target_view = cstr_SL("Hello World");
 * bool target_starts_with_greeting = strview_starts_with(target_view, cstr("Hello"));	//returns true
 * OR
 * bool target_starts_with_greeting = strview_starts_with(target_view, "Hello");		//returns true
 * @endcode
  **********************************************************************************/
	#define strview_starts_with(str1, str2) _Generic((str2),\
		const char*:	strview_starts_with_cstr,\
		char*:			strview_starts_with_cstr,\
		strview_t:		strview_starts_with_strview\
		)(str1, str2)


/**
 * @def  strview_starts_with_nocase(strview_t str1, str2)
 * @brief (macro) Test the starting contents of a view, ignoring case.
 * @param str1 The view to test.
 * @param str2 Either a C string, or a view of the content we are testing for.
 * @return true if the contents of str2 is found at the start of str1 ignoring case, or if both views are invalid.
 * @note Example:
 * @code{.c}
 * strview_t target_view = cstr_SL("Hello World");
 * bool target_starts_with_greeting = strview_starts_with_nocase(target_view, cstr("HELLO"));	//returns true
 * OR
 * bool target_starts_with_greeting = strview_starts_with_nocase(target_view, "HELLO");		//returns true
 * @endcode
  **********************************************************************************/
	#define strview_starts_with_nocase(str1, str2) _Generic((str2),\
		const char*:	strview_starts_with_nocase_cstr,\
		char*:			strview_starts_with_nocase_cstr,\
		strview_t:		strview_starts_with_nocase_strview\
		)(str1, str2)


/**
 * @def strview_trim(strview_t str, chars_to_trim);
 * @brief (macro) Trim both ends of a view.
 * @param str The source view.
 * @param chars_to_trim A C string OR a view of all the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Example:
 * @code{.c}
 * strview_t source_view = cstr_SL(" ._THIS. _");
 * strview_t trimmed_view = strview_trim(source_view, cstr(" ._")); // return view "THIS"
 * OR
 * strview_t trimmed_view = strview_trim(source_view, " ._"); // return view "THIS"
 * @endcode
 * **********************************************************************************/
	#define strview_trim(str, chars_to_trim) _Generic((chars_to_trim),\
		const char*:	strview_trim_cstr,\
		char*:			strview_trim_cstr,\
		strview_t:		strview_trim_strview\
		)(str, chars_to_trim)


/**
 * @def strview_trim_start(strview_t str, chars_to_trim);
 * @brief Trim the start of a view.
 * @param str The source view.
 * @param chars_to_trim A C string OR a view of all the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Example:
 * @code{.c}
 * strview_t source_view = cstr_SL(" ._THIS");
 * strview_t trimmed_view = strview_trim_start(source_view, cstr(" ._")); // return view "THIS"
 * OR
 * strview_t trimmed_view = strview_trim_start(source_view, " ._"); // return view "THIS"
 * @endcode
 * **********************************************************************************/
	#define strview_trim_start(str, chars_to_trim) _Generic((chars_to_trim),\
		const char*:	strview_trim_start_cstr,\
		char*:			strview_trim_start_cstr,\
		strview_t:		strview_trim_start_strview\
		)(str, chars_to_trim)


/**
 * @def strview_trim_end(strview_t str, chars_to_trim);
 * @brief Trim the start of a view.
 * @param str The source view.
 * @param chars_to_trim A string literal OR a view of all the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Example:
 * @code{.c}
 * strview_t source_view = cstr_SL("THIS ._");
 * strview_t trimmed_view = strview_trim_end(source_view, cstr(" ._")); // return view "THIS"
 * OR
 * strview_t trimmed_view = strview_trim_end(source_view, " ._"); // return view "THIS"
 * @endcode
 * **********************************************************************************/
	#define strview_trim_end(str, chars_to_trim) _Generic((chars_to_trim),\
		const char*:	strview_trim_end_cstr,\
		char*:			strview_trim_end_cstr,\
		strview_t:		strview_trim_end_strview\
		)(str, chars_to_trim)


/**
 * @def strview_find_first(strview_t haystack, needle);
 * @brief (macro) Find first needle in haystack.
 * @param haystack The view to search within.
 * @param needle A view or a C string of the contents to search for.
 * @return A view of the needle within the haystack, or an invalid view if the needle was not found.
 * @note On success, the returned view will match the contents of needle, but will be viewing data within the haystack.
 * @note Example:
 * @code{.c}
 * strview_t haystack_view = cstr_SL("First name: FRED, Second name: SMITH");
 * strview_t needle_view = strview_find_first(haystack_view, "name: ");
 * strview_t name_view = strview_split_right(&haystack_view, needle_view); // view "FRED, Second name: SMITH"
 * @endcode
 * *********************************************************************************/
	#define strview_find_first(haystack, needle) _Generic((needle),\
		const char*:	strview_find_first_cstr,\
		char*:			strview_find_first_cstr,\
		strview_t:		strview_find_first_strview\
		)(haystack, needle)


/**
 * @def strview_find_last(strview_t haystack, needle);
 * @brief (macro) Find last needle in haystack.
 * @param haystack The view to search within.
 * @param needle A view or a C string of the contents to search for.
 * @return A view of the needle within the haystack, or an invalid view if the needle was not found.
 * @note On success, the returned view will match the contents of needle, but will be viewing data within the haystack.
 * @note Example:
 * @code{.c}
 * strview_t haystack_view = cstr_SL("First name: FRED, Second name: SMITH");
 * strview_t needle_view = strview_find_last(haystack_view, cstr_SL("name: ");
 * strview_t name_view = strview_split_right(&haystack_view, needle_view); // view "SMITH"
 * @endcode
 * *********************************************************************************/
	#define strview_find_last(haystack, needle) _Generic((needle),\
		const char*:	strview_find_last_cstr,\
		char*:			strview_find_last_cstr,\
		strview_t:		strview_find_last_strview\
		)(haystack, needle)


/**
 * @def strview_split_first_delim(strview_t* src, delims);
 * @brief (macro) Split view by delimiters.
 * @param src The address of the view to split.
 * @param delims A view or a C string of the delimiter character/s.
 * @return A view up to, but not including, the first delimiter found.
 * @note The returned view and the delimiter itself is removed from the source view.
 * @note If there are no delimiters, the entire source will be returned, and the source view becomes invalid.
 * @note Example:
 * @code{.c}
 *  strview_t date_view = cstr("2023/07/03");
 *  strview_t year_view  = strview_split_first_delim(&date_view, cstr("/"));
 *  strview_t month_view = strview_split_first_delim(&date_view, cstr("/"));
 *  strview_t day_view   = strview_split_first_delim(&date_view, cstr("/"));
 * @endcode
 * *********************************************************************************/
	#define strview_split_first_delim(src, delims) _Generic((delims),\
		const char*:	strview_split_first_delim_cstr,\
		char*:			strview_split_first_delim_cstr,\
		strview_t:		strview_split_first_delim_strview\
		)(src, delims)


/**
 * @def strview_split_all(int dst_size, strview_t dst[dst_size], strview_t src, const char* delims);
 * @brief Split entire view by delimiters, into an array of views.
 * @param dst The destination array to write to.
 * @param dst_size The number of elements available in the destination.
 * @param src The view to split.
 * @param delims A view or a C string of the delimiter character/s.
 * @return The number of elements written to dst[]
 * *********************************************************************************/
	#define strview_split_all(dst_size, dst, src, delims) _Generic((delims),\
		const char*:	strview_split_all_cstr,\
		char*:			strview_split_all_cstr,\
		strview_t:		strview_split_all_strview\
		)(dst_size, dst, src, delims)


/**
 * @def strview_split_first_delim_nocase(strview_t* src, delims);
 * @brief (macro) Split view by delimiters, ignoring case.
 * @param src The address of the view to split.
 * @param delims A view or a C string of the delimiter character/s, these are not case sensetive.
 * @return A view up to, but not including, the first delimiter found.
 * @note The returned view and the delimiter itself is removed from the source view.
 * @note If there are no delimiters, the entire source will be returned, and the source view becomes invalid.
 * *********************************************************************************/
	#define strview_split_first_delim_nocase(src, delims) _Generic((delims),\
		const char*:	strview_split_first_delim_nocase_cstr,\
		char*:			strview_split_first_delim_nocase_cstr,\
		strview_t:		strview_split_first_delim_nocase_strview\
		)(src, delims)


/**
 * @def strview_split_last_delim(strview_t* src, delims);
 * @brief (macro) Split view by last delimiter.
 * @param src The address of the view to split.
 * @param delims A view or a C string of the delimiter character/s.
 * @return A view from, but not including, the last delimiter found.
 * @note The returned view and the delimiter itself is removed from the source view.
 * @note If there are no delimiters, the entire source will be returned, and the source view becomes invalid.
 * @note Example:
 * @code{.c}
 *  strview_t date_view = cstr("2023/07/03");
 *  strview_t day_view   = strview_split_first_delim(&date_view, "/");
 *  strview_t month_view = strview_split_first_delim(&date_view, "/");
 *  strview_t year_view  = strview_split_first_delim(&date_view, "/");
 * @endcode
 * *********************************************************************************/
	#define strview_split_last_delim(src, delims) _Generic((delims),\
		const char*:	strview_split_last_delim_cstr,\
		char*:			strview_split_last_delim_cstr,\
		strview_t:		strview_split_last_delim_strview\
		)(src, delims)


/**
 * @def strview_split_last_delim_nocase(strview_t* src, delims);
 * @brief (macro) Split view by last delimiter, ignoring case.
 * @param src The address of the view to split.
 * @param delims A view or a C string of the delimiter character/s, these are not case sensetive.
 * @return A view from, but not including, the last delimiter found.
 * @note The returned view and the delimiter itself is removed from the source view.
 * @note If there are no delimiters, the entire source will be returned, and the source view becomes invalid.
 * *********************************************************************************/
	#define strview_split_last_delim_nocase(src, delims) _Generic((delims),\
		const char*:	strview_split_last_delim_nocase_cstr,\
		char*:			strview_split_last_delim_nocase_cstr,\
		strview_t:		strview_split_last_delim_nocase_strview\
		)(src, delims)


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
 * @note Use via macro strview_is_match()
  **********************************************************************************/
	bool strview_is_match_strview(strview_t str1, strview_t str2);

/**
 * @brief Test if the contents of a view matches the contents of a c string.
 * @return true if the contents match or if both views are invalid.
 * @note Use via macro strview_is_match()
  **********************************************************************************/
	bool strview_is_match_cstr(strview_t str1, const char* str2);

/**
 * @brief Test of the contents of two views match, ignoring case.
 * @return true if the contents match or if both views are invalid.
 * @note Use via macro strview_is_match_nocase()
  **********************************************************************************/
	bool strview_is_match_nocase_strview(strview_t str1, strview_t str2);

/**
 * @brief Test if the contents of a view matches the contents of a c string, ignoring case.
 * @return true if the contents match or if both views are invalid.
 * @note Use via macro strview_is_match_nocase()
  **********************************************************************************/
	bool strview_is_match_nocase_cstr(strview_t str1, const char* str2);

/**
 * @brief Test the starting contents of a view.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Use via macro strview_starts_with()
  **********************************************************************************/
	bool strview_starts_with_cstr(strview_t str1, const char* str2);

/**
 * @brief Test the starting contents of a view.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Use via macro strview_starts_with()
  **********************************************************************************/
	bool strview_starts_with_strview(strview_t str1, strview_t str2);

/**
 * @brief Test the starting contents of a view, ignoring case.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Use via macro strview_starts_with_nocase()
  **********************************************************************************/
	bool strview_starts_with_nocase_cstr(strview_t str1, const char* str2);

/**
 * @brief Test the starting contents of a view, ignoring case.
 * @return true if the contents of str2 is found at the start of str1, or if both views are invalid.
 * @note Use via macro strview_starts_with_nocase()
  **********************************************************************************/
	bool strview_starts_with_nocase_strview(strview_t str1, strview_t str2);

/**
 * @brief Alphabetical comparison of two views.
 * @return >0 if the first non-matching character in str1 is greater than that of str2, or if str2 is invalid.
 * @return <0 if the first non-matching character in str1 is less than that of str2, or if str1 is invalid.
 * @return 0 if the views are equal, or if both views are invalid.
  **********************************************************************************/
	int strview_compare(strview_t str1, strview_t str2);

/**
 * @brief Sub string by index.
 * @param str The source view.
 * @param begin Starting index of the sub string within the source view.
 * @param end Ending index of the sub string within the source view, non-inclusive.
 * @return The sub string.
 * @note The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string.
 * @note Negative indexes may be used, and will index from the end of the source backwards.
 * @note Example:
 * @code{.c}
 * strview_t source_view = cstr_SL("...THIS...");
 * strview_t sub_view = strview_sub(source_view, 3, 7); // view THIS
 * @endcode
 * **********************************************************************************/
	strview_t strview_sub(strview_t str, int begin, int end);

/**
 * @brief Trim both ends of a view.
 * @param str The source view.
 * @param chars_to_trim A view containing all of the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Use via macro strview_trim()
 * **********************************************************************************/
	strview_t strview_trim_strview(strview_t str, strview_t chars_to_trim);

/**
 * @brief Trim the start of a view.
 * @param str The source view.
 * @param chars_to_trim A view containing all of the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Use via macro strview_trim_start():
 * **********************************************************************************/
	strview_t strview_trim_start_strview(strview_t str, strview_t chars_to_trim);

/**
 * @brief Trim the end of a view.
 * @param str The source view.
 * @param chars_to_trim A view containing all of the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Use via macro strview_trim_end():
 * **********************************************************************************/
	strview_t strview_trim_end_strview(strview_t str, strview_t chars_to_trim);

/**
 * @brief Trim both ends of a view.
 * @param str The source view.
 * @param chars_to_trim A C string containing all of the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Use via macro strview_trim()
 * **********************************************************************************/
	strview_t strview_trim_cstr(strview_t str, const char* chars_to_trim);

/**
 * @brief Trim the start of a view.
 * @param str The source view.
 * @param chars_to_trim A C string containing all of the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Use via macro strview_trim_start():
 * **********************************************************************************/
	strview_t strview_trim_start_cstr(strview_t str, const char* chars_to_trim);

/**
 * @brief Trim the end of a view.
 * @param str The source view.
 * @param chars_to_trim A C string containing all of the characters to be trimmed from the source.
 * @return The trimmed view.
 * @note Use via macro strview_trim_end():
 * **********************************************************************************/
	strview_t strview_trim_end_cstr(strview_t str, const char* chars_to_trim);

/**
 * @brief Find first needle in haystack.
 * @param haystack The view to search within.
 * @param needle A view of the contents to search for.
 * @return A view of the needle within the haystack, or an invalid view if the needle was not found.
 * @note Use via macro strview_find_first()
 * *********************************************************************************/
	strview_t strview_find_first_strview(strview_t haystack, strview_t needle);

/**
 * @brief Find first needle in haystack.
 * @param haystack The view to search within.
 * @param needle A C string of the contents to search for.
 * @return A view of the needle within the haystack, or an invalid view if the needle was not found.
 * @note Use via macro strview_find_first()
 * *********************************************************************************/
	strview_t strview_find_first_cstr(strview_t haystack, const char* needle);

/**
 * @brief Find last needle in haystack.
 * @param haystack The view to search within.
 * @param needle A view of the contents to search for.
 * @return A view of the needle within the haystack, or an invalid view if the needle was not found.
 * @note Use via macro strview_find_last()
 * *********************************************************************************/
	strview_t strview_find_last_strview(strview_t haystack, strview_t needle);

/**
 * @brief Find last needle in haystack.
 * @param haystack The view to search within.
 * @param needle A C string of the contents to search for.
 * @return A view of the needle within the haystack, or an invalid view if the needle was not found.
 * @note Use via macro strview_find_last()
 * *********************************************************************************/
	strview_t strview_find_last_cstr(strview_t haystack, const char* needle);

/**
 * @brief Split view by delimiters.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s.
 * @return A view up to, but not including, the first delimiter found.
 * @note Use via macro strview_split_first_delim()
 * *********************************************************************************/
	strview_t strview_split_first_delim_strview(strview_t* src, strview_t delims);

/**
 * @brief Split entire view by delimiters, into an array of views.
 * @param dst The destination array to write to.
 * @param dst_size The number of elements available in the destination.
 * @param src The the view to split.
 * @param delims A C string of the delimiter character/s.
 * @return The number of elements written to dst[]
 * @note Use via macro strview_split_all()
 * *********************************************************************************/
	int strview_split_all_cstr(int dst_size, strview_t dst[dst_size], strview_t src, const char* delims);

/**
 * @brief Split entire view by delimiters, into an array of views.
 * @param dst The destination array to write to.
 * @param dst_size The number of elements available in the destination.
 * @param src The the view to split.
 * @param delims A view of the delimiter character/s.
 * @return The number of elements written to dst[]
 * @note Use via macro strview_split_all()
 * *********************************************************************************/
	int strview_split_all_strview(int dst_size, strview_t dst[dst_size], strview_t src, strview_t delims);

/**
 * @brief Split view by delimiters.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s.
 * @return A view up to, but not including, the first delimiter found.
 * @note Use via macro strview_split_first_delim()
 * *********************************************************************************/
	strview_t strview_split_first_delim_cstr(strview_t* src, const char* delims);


/**
 * @brief Split view by delimiters, ignoring case.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s, these are not case sensetive.
 * @return A view up to, but not including, the first delimiter found.
 * @note Use via macro strview_split_first_delim_nocase()
 * *********************************************************************************/
	strview_t strview_split_first_delim_nocase_strview(strview_t* src, strview_t delims);


/**
 * @brief Split view by delimiters, ignoring case.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s, these are not case sensetive.
 * @return A view up to, but not including, the first delimiter found.
 * @note Use via macro strview_split_first_delim_nocase()
 * *********************************************************************************/
	strview_t strview_split_first_delim_nocase_cstr(strview_t* src, const char* delims);


/**
 * @brief Split view by last delimiter.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s.
 * @return A view from, but not including, the last delimiter found.
 * @note Use via macro strview_split_last_delim()
 * *********************************************************************************/
	strview_t strview_split_last_delim_strview(strview_t* src, strview_t delims);


/**
 * @brief Split view by last delimiter.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s.
 * @return A view from, but not including, the last delimiter found.
 * @note Use via macro strview_split_last_delim()
 * *********************************************************************************/
	strview_t strview_split_last_delim_cstr(strview_t* src, const char* delims);


/**
 * @brief Split view by last delimiter, ignoring case.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s, these are not case sensetive.
 * @return A view from, but not including, the last delimiter found.
 * @note Use via macro strview_split_last_delim_nocase()
 * *********************************************************************************/
	strview_t strview_split_last_delim_nocase_strview(strview_t* src, strview_t delims);


/**
 * @brief Split view by last delimiter, ignoring case.
 * @param src The address of the view to split.
 * @param delims A view of the delimiter character/s, these are not case sensetive.
 * @return A view from, but not including, the last delimiter found.
 * @note Use via macro strview_split_last_delim_nocase()
 * *********************************************************************************/
	strview_t strview_split_last_delim_nocase_cstr(strview_t* src, const char* delims);


/**
 * @brief Split view by index.
 * @param src The address of the view to split.
 * @param index The number of characters to split. Negative values split from the end.
 * @return A view of the split characters.
 * @note The returned view is removed from the source view.
 * @note If all remaining characters are split, the source view becomes empty, but remains valid.
 * @note If index is larger than the source, all characters will be split.
 * @note Example:
 * @code{.c}
 *  strview_t src_view = cstr("ABCDE........FGHIJ");
 *  strview_t atoe_view  = strview_split_index(&src_view, 5);	//view "ABCDE"
 *  strview_t ftoj_view  = strview_split_index(&src_view, -5);	//view "FGHIJ"
 * @endcode
 * *********************************************************************************/
	strview_t strview_split_index(strview_t* src, int index);

/**
 * @brief Split left of a view.
 * @param src The address of the view to split.
 * @param pos A view of the location within source to split at.
 * @return A view of the source up to the location of pos.
 * @note The returned view is removed from the source view.
 * @note Edge cases:
 * @note If pos references the upper limit of src, the entire src is returned. src becomes empty but remains valid.
 * @note If pos references the start of src, an empty but valid view is returned, and src remains unmodified.
 * @note If pos does not reference data within src, an invalid view is returned, and src remains unmodified.
 * @note Example:
 * @code{.c}
 * strview_t haystack_view = cstr_SL("Activity cancelled 2023-07-01");
 * strview_t needle_view = strview_find_first(haystack_view, cstr_SL("cancelled");
 * strview_t activity_view = strview_split_left(&haystack_view, needle_view);   // view "Activity "
 * @endcode
 * *********************************************************************************/
	strview_t strview_split_left(strview_t* src, strview_t pos);

/**
 * @brief Split right of a view.
 * @param src The address of the view to split.
 * @param pos A view of the location within source to split at.
 * @return A view of the source from the end of pos.
 * @note The returned view is removed from the source view.
 * @note Edge cases:
 * @note If the last character of pos is the last character of src, an empty but valid view is returned, and src remains unmodified.
 * @note If pos references the start of src, an empty but valid view is returned, and src remains unmodified.
 * @note If pos does not reference data within src, an invalid view is returned, and src remains unmodified.
 * @note Example:
 * @code{.c}
 * strview_t haystack_view = cstr_SL("Activity cancelled 2023-07-01");
 * strview_t needle_view = strview_find_first(haystack_view, cstr_SL("cancelled");
 * strview_t activity_view = strview_split_right(&haystack_view, needle_view);   // view " 2023-07-01"
 * @endcode
 * *********************************************************************************/
	strview_t strview_split_right(strview_t* src, strview_t pos);

/**
 * @brief Pop first character from view.
 * @param src The address of the view to pop a character from.
 * @return The first character from the source view, or 0 if the source is empty or invalid.
 * *********************************************************************************/
	char strview_pop_first_char(strview_t* src);
 
/**
 * @brief Split by line.
 * @param src The address of the view to split.
 * @param eol Optional. The address of a char representing the state of the eol discriminator, or NULL.
 * @return A view of the first line within the source, not including the end of line terminator.
 * @note The returned line and the terminator are removed from the source.
 * @note If a line terminator is not found, an invalid view is returned and the source remains unmodified.
 * @note Any mixture of (CR,LF,CRLF,LFCR) can be handled. A CRLF or LFCR sequence will always be interpreted as 1 line ending.
 * @note If the source may end with a partial CRLF or LFCR, the state of the EOL discriminator can be saved in *eol.
 * *********************************************************************************/
	strview_t strview_split_line(strview_t* src, char* eol);

/**
 * @brief Remove quotation.
 * @param src The view to de-quote.
 * @return A view of the src without quotes.
 * @note Multiple levels of quotation will be removed providing it is of the same type eg.
 * @note ""Fred"" yields Fred. "'Fred'" yields 'Fred'. '"Fred"' yields "Fred"
 * @note Does not trim whitespace first, use strview_trim() for that.
 * *********************************************************************************/
	strview_t strview_dequote(strview_t src);


#endif

