# STR - C string handling library

C String handling library inspired by Luca Sas. https://www.youtube.com/watch?v=QpAhX-gsHMs&t=3009s

&nbsp;
## Introduction

 This project aims to implement a convenient and intuitive approach to string handling, described in a talk by Luca Sas in the above mentioned YouTube video.

 The core ideas are:
 * Separating the ownership of a string (which can modify/build strings) from the access or view of strings (navigating/splitting/trimming).
 * Returning strings by value, to avoid pointers.
 * Ditching the requirement for null termination.

&nbsp;
## Standard used
 GCC - GNU99

&nbsp;
## Usage
 Copy the source files __str.h__/__str.c__ and optionally __strbuf.h__/__strbuf.c__ into your project.

&nbsp;
## Todo:
* Documentation for **strbuf.h**

&nbsp;

# str.h
 str.h provides functions for navigating, reading, and interpreting portions of const char string data. It may be used standalone, and does not depend on **strbuf.h**.

It does not store the underlying data itself, and is not intended to be used for modifying the data. The string data may be stored anywhere, string literals (the string pool), a dynamic buffer provided by strbuf.h, or a static buffer etc...

str.h defines the following str_t type :

	typedef struct str_t
	{
		const char* data;
		size_t size;
	} str_t;

Note that this holds only:
 * A pointer to the beginning of the string
 * The size (in characters) of the string

Some operations may return an invalid str_t, in this case .data=NULL and .size==0. Operations which can cause an invalid str_t to be returned include:
* Attempting to split a string using non-existent delimiter with **str_pop_first_split()** or **str_pop_last_split()**
* Requesting a substring range, which is entirely outside of the range of the input string.

&nbsp;
# str.h functions:

&nbsp;
## str_t cstr(const char* c_str);
 Return a str_t from a null terminated const char[] string. If the string provided is a string literal, then the macro **cstr_SL("mystring")** may be used instead of **cstr("mystring")** to avoid measuring the strings length at runtime (although either will work).

&nbsp;
## char* str_to_cstr(char* dst, size_t dst_size, str_t str);
 Write a str_t out to a null terminated char* buffer. The buffer and space available (including the terminator) must be provided by the caller.

&nbsp;
## bool str_is_valid(str_t str);
 Return true if the str_t is valid.

&nbsp;
## bool str_is_match(str_t str1, str_t str2);
 Return true if the strings match. Also returns true if BOTH strings are invalid.

&nbsp;
## bool str_is_match_nocase(str_t str1, str_t str2);
 Same as **str_is_match()** ignoring case.

&nbsp;
## int str_compare(str_t str1, str_t str2);
 A replacement for strcmp(). Used for alphabetizing strings. May also be used instead of **str_is_match()**, although keep in mind that it will return 0 if it compares an invalid string to a valid string of length 0. (Where **str_is_match()** would return false if only one string is invalid.)

&nbsp;
## str_t str_sub(str_t str, int begin, int end);
 Return the sub string indexed by **begin** to **end**, where **end** is non-inclusive.
 Negative values may be used, and will index from the end of the string backwards.
 The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string. If the requested range is entirely outside of the input string, then an invalid **str_t** is returned.

&nbsp;
## str_t str_trim(str_t str, str_t chars_to_trim);
 Return a str_t with the start and end trimmed of all characters present in **chars_to_trim**.

&nbsp;
## str_t str_trim_start(str_t str, str_t chars_to_trim);
 Return a str_t with the start trimmed of all characters present in **chars_to_trim**.

&nbsp;
## str_t str_trim_end(str_t str, str_t chars_to_trim);
 Return a str_t with the end trimmed of all characters present in **chars_to_trim**.

&nbsp;
## str_search_result_t str_find_first(str_t haystack, str_t needle);
 Return the **str_search_result_t** (bool found & index) for the first occurrence of needle in haystack.
 **str_search_result_t** is defined as the following structure:

	typedef struct str_search_result_t
	{
		bool found;
		size_t index;
	} str_search_result_t;

Some special cases to consider:
 * If **needle** is valid, and of length 0, it will always be found at the start of the string.
 * If **needle** is invalid, or if **haystack** is invalid, it will not be found.
	
&nbsp;
## str_search_result_t str_find_last(str_t haystack, str_t needle);
 Return the **str_search_result_t** (bool found & index) for the last occurrence of **needle** in **haystack**.

Some special cases to consider:
* If **needle** is valid, and of length 0, it will always be found at the index of the last character in **haystack**+1.
* If **needle** is invalid, or if **haystack** is invalid, it will not be found.

&nbsp;
## str_t str_pop_first_split(str_t* str_ptr, str_t delimiters);
 Return a **str_t** representing the contents of the source string up to, but not including, any of characters in **delimiters**.
 Additionally, the contents of the returned **str_t**, and the delimeter character itself is removed (popped) from the input string.
 If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid().

Example usage:

    str_t date = cstr("2022/10/03");
    str_t year  = str_pop_first_split(&date, cstr("/"));
    str_t month = str_pop_first_split(&date, cstr("/"));
    str_t day   = str_pop_first_split(&date, cstr("/"));

&nbsp;
## str_t str_pop_last_split(str_t* str_ptr, str_t delimiters);
 Same as **str_pop_first_split()** but searches from the end of the string backwards.

&nbsp;
## long long str_to_ll(str_t str);
 Convert the ascii representation of the signed long long in **str**, into a long long value.
 The input string is first  trimmed of any leading spaces, and may start with 0x or 0X for hex numbers, or 0b for binary numbers.

&nbsp;
## unsigned long long str_to_ull(str_t str);
 Same as **str_to_ull()** but converts to an unsigned value.

The above **str_to_ull** and **str_to_ll()** are also cast to stdint.h types using macros.

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

&nbsp;
## double str_to_float(str_t str);
 Convert the ascii representation of a floating point value in **str**, into a double.

The default precision of this function is double, but **str.h** accepts the following defined symbols:
* STR_NO_FLOAT - Do not provide floating point conversions. 
* STR_SUPPORT_FLOAT - Use float instead of double.
* STR_SUPPORT_LONG_DOUBLE - Use long double instead of double.
