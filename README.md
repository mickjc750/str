# STR - C string handling library

C String handling library inspired by Luca Sas. https://www.youtube.com/watch?v=QpAhX-gsHMs&t=3009s

# Table of Contents
1.  [Introduction](#introduction)
2.  [Understanding the separate purposes of str.h and strbuf.h](#understanding-the-separate-purposes-of-strh-and-strbufh)
3.  [str.h](#strh)
4.  [Passing a str_t to printf()](#passing-a-strt-to-printf)
5.  [str.h functions](#strh-functions)
6.  [strbuf.h](#strbufh)
7.  [Providing an allocator for strbuf_create().](#providing-an-allocator-for-strbufcreate)
8.  [Allocator examples](#allocator-examples)
9.  [Buffer re-sizing](#buffer-re-sizing)
10. [strbuf.h functions](#strbufh-functions)

&nbsp;
&nbsp;
## Introduction

 This project aims to implement a convenient and intuitive approach to string handling, described in a talk by Luca Sas in the above mentioned YouTube video.

 The core ideas are:
 * Separating the ownership of a string (which can modify/build strings) from the access or view of strings (navigating/splitting/trimming).
 * Returning strings by value, to avoid pointers.
 * Ditching the requirement for null termination.

&nbsp;
# Understanding the separate purposes of str.h and strbuf.h
This project is provided in two main parts, **str.h** which provides a **str_t** type, and **strbuf.c** which provides a **strbuf_t** type.

To understand this approach to string handling, and the purpose of each, it helps to think in terms of string ownership.

## str_t
**str_t** doesn't own the string. It's just a view into a string, and can't be used to free it, and shouldn't be used to change it's characters. You can only change the range of the view, or split it into multiple views, or interpret a view as a number etc. **str_t** is intended for reading and parsing strings, not building them. As there is no null terminator requirement, binary strings including the full ascii set 0-255 can safely be worked with.
**str.h** does not depend on **strbuf.h**, and can be useful on it’s own.

&nbsp;
## strbuf_t
**strbuf_t** DOES own the string, and contains the information needed to resize it, change it's contents, or free it. The allocator used by **strbuf_t** is provided by the application, and dynamic memory allocation is not mandatory.

Whether you pass a **str_t** or a **str_buf_t** to your functions depends on the use case.

If you wish to pass a string to a function which frees it, then you need to pass ownership along with it, so in that case a strbuf_t needs to be passed.

If you only wish to provide a view into an existing string (read only), then a **str_t** can be passed.

&nbsp;
## Standard used
 GCC - GNU99

&nbsp;
## Usage
 Copy the source files __str.h__/__str.c__ and optionally __strbuf.h__/__strbuf.c__ into your project.


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
# Passing a str_t to printf()
 There are two macros defined for this **PRIstr** and **PRIstrarg()**, which make use if printf's dynamic precision to limit the number of characters read.
 
 Example usage:

	printf("The string is {%"PRIstr"}\n", PRIstrarg(mystring));

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

	#define str_to_int(str)         ((int)str_to_ll(str))
	#define str_to_int8_t(str)      ((int8_t)str_to_ll(str))
	#define str_to_int16_t(str)     ((int16_t)str_to_ll(str))
	#define str_to_int32_t(str)     ((int32_t)str_to_ll(str))
	#define str_to_int64_t(str)     ((int64_t)str_to_ll(str))
	#define str_to_unsigned(str)    ((unsigned)str_to_ull(str))
	#define str_to_uint8_t(str)     ((uint8_t)str_to_ull(str))
	#define str_to_uint16_t(str)    ((uint16_t)str_to_ull(str))
	#define str_to_uint32_t(str)    ((uint32_t)str_to_ull(str))
	#define str_to_uint64_t(str)    ((uint64_t)str_to_ull(str))

&nbsp;
## double str_to_float(str_t str);
 Convert the ascii representation of a floating point value in **str**, into a double.

The default precision of this function is double, but **str.h** accepts the following defined symbols:
* STR_NO_FLOAT - Do not provide floating point conversions. 
* STR_SUPPORT_FLOAT - Use float instead of double.
* STR_SUPPORT_LONG_DOUBLE - Use long double instead of double.

&nbsp;
# strbuf.h
 strbuf.h provides functions for allocating, building and storing strings.
 Unlike the str_t type, a strbuf_t owns the string data, and contains all the information needed to modify it, resize it, or free it.
 
 While dynamic memory allocation is very useful, it is not mandatory (with one exception regarding strbuf_cat()). The allocator can be as simple as something which returns the address of a static buffer. The size of the buffer must be the expected capacity (chars) + sizeof(strbuf_t) + 1 for a null terminator.

 All strbuf functions maintain a null terminator at the end of the buffer, and the buffer may be accessed as a regular c string using mybuffer->cstr.

 str.h defines the following strbuf_t type :

	typedef struct strbuf_t
	{
		size_t size;
		size_t capacity;
		str_allocator_t allocator;
		char cstr[];
	} strbuf_t;

&nbsp;
# Providing an allocator for strbuf_create().

 **strbuf_create()** requires an allocator to be passed.

 The following str_allocator_t type is defined by strbuf.h

	typedef struct str_allocator_t
	{
		void* app_data;
		void* (*allocator)(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);
	} str_allocator_t;

## Explanation:
	void* app_data;
 The address of the str_allocator_t is passed to the allocator. If the allocator requires access to some implementation specific data to work (such as in the case of a temporary allocator), then *app_data may be used to pass this.

&nbsp;

	void* (*allocator)(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

A pointer to the allocator function.

&nbsp;
&nbsp;

The parameters to this function are:

	struct str_allocator_t* this_allocator <-- A pointer to the str_allocator_t which may be used to access ->app_data.
	void* ptr_to_free                      <-- Memory address to free OR reallocate.
	size_t size                            <-- Size of allocation, or new size of the reallocation, or 0 if memory is to be freed.
	const char* caller_filename            <-- usually /path/strbuf.c, this is to support allocators which track caller ID.
	int caller_line                        <-- The line within strbuf.c which called the allocator, this is also to support allocators which track caller ID.

&nbsp;
# Allocator examples
Three of these provided under allocator_examples/

&nbsp;
# Buffer re-sizing
The initial capacity of the buffer will be exactly as provided to strbuf_create(). If an operation needs to extend the buffer, the size will be rounded up by STR_CAPACITY_GROW_STEP. The default value of this is 16, but this can be changed by defining it in a compiler flag ie. -DSTR_CAPACITY_GROW_STEP=32

The buffer capacity is never shrunk, unless strbuf_shrink() is called. In which case it will be reduced to the minimum possible.

&nbsp;
&nbsp;
# strbuf.h functions:

&nbsp;
##	strbuf_t* strbuf_create(size_t initial_capacity, str_allocator_t allocator);
 Create and return the address of a strbuf_t.

&nbsp;
##	str_t strbuf_cat(strbuf_t** buf_ptr, ...);
 This is a macro, which concatenates one or more str_t into a buffer, and returns the str_t of the buffer. The returned str_t is always valid.

 After performing some argument counting wizardry, it calls _strbuf_cat(strbuf_t** buf_ptr, int n_args, ...)

 If the allocator is dynamic, input arguments may be from the output buffer itself. In this case a temporary buffer is allocated to build the output.

&nbsp;
##  str_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va);
 The non-variadic version of _strbuf_cat.

&nbsp;
## str_t strbuf_append_char(strbuf_t** buf_ptr, char c);
 Append a single character to the buffer.

&nbsp;
## str_t strbuf_str(strbuf_t** buf_ptr);
 Return str_t of buffer contents.

&nbsp;
## str_t strbuf_shrink(strbuf_t** buf_ptr);
 Shrink buffer to the minimum size required to hold it's contents.
	
&nbsp;
## void strbuf_destroy(strbuf_t** buf_ptr);
 Free memory allocated to hold the buffer and it's contents.

&nbsp;
## str_t strbuf_append(strbuf_t** buf_ptr, str_t str);
 Append str_t to buffer.
	
&nbsp;
## str_t strbuf_prepend(strbuf_t** buf_ptr, str_t str);
 Prepend str_t to buffer.
	
&nbsp;
## str_t strbuf_insert(strbuf_t** buf_ptr, int index, str_t str);
 Insert str_t to buffer at index.
	
