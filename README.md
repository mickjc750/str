# STR - C string handling library

C String handling library inspired by Luca Sas. https://www.youtube.com/watch?v=QpAhX-gsHMs&t=3009s

# Table of Contents
1.  [Introduction](#introduction)
2.  [Understanding the separate purposes of strview.h and strbuf.h](#understanding-the-separate-purposes-of-strviewh-and-strbufh)
3.  [strview.h](#strviewh)
4.  [Passing a strview_t to printf()](#passing-a-strview_t-to-printf)
5.  [strview.h functions](#strviewh-functions)
6.  [strbuf.h](#strbufh)
7.  [Providing an allocator for strbuf_create().](#providing-an-allocator-for-strbufcreate)
8.  [Allocator example](#allocator-example)
9.  [Buffer re-sizing](#buffer-re-sizing)
10. [non-dynamic buffers](#non-dynamic-buffers)
11. [printf to a strbuf_t](#printf-to-a-strbuf_t)
12. [prnf to a strbuf_t](#prnf-to-a-strbuft)
13. [strbuf.h functions](#strbufh-functions)

&nbsp;
&nbsp;
## Introduction

 This project aims to implement a convenient and intuitive approach to string handling, described in a talk by Luca Sas in the above mentioned YouTube video.

 The core ideas are:
 * Separating the ownership of a string (which can modify/build strings) from the access or view of strings (navigating/splitting/trimming).
 * Returning strings by value, to avoid pointers.
 * Ditching the requirement for null termination.

&nbsp;
# Understanding the separate purposes of strview.h and strbuf.h
This project is provided in two main parts, **strview.h** which provides a **strview_t** type, and **strbuf.c** which provides a **strbuf_t** type.

To understand this approach to string handling, and the purpose of each, it helps to think in terms of string ownership.

## strview_t
**strview_t** doesn't own the string. It's just a view into a string, and can't be used to free it, and shouldn't be used to change it's characters. You can only change the range of the view, or split it into multiple views, or interpret a view as a number etc. **strview_t** is intended for reading and parsing strings, not building them. As there is no null terminator requirement, binary strings including the full ascii set 0-255 can safely be worked with.
**strview.h** does not depend on **strbuf.h**, and can be useful on it’s own.

&nbsp;
## strbuf_t
**strbuf_t** DOES own the string, and contains the information needed to resize it, change it's contents, or free it. Dynamic memory allocation is not mandatory. The memory space can be as simple as a static buffer provided by the application. For a dynamic buffer, the application may either provide it's own allocator, or strbuf can default to using malloc/free.

&nbsp;

 Whether or not you pass a **strview_t** or a **strbuf_t** to your functions depends on the use case.

 If you wish to pass a string to a function which frees it, then you need to pass ownership along with it, so in that case a strbuf_t needs to be passed.

 If you only wish to provide a view into an existing string (read only), then a **strview_t** can be passed.

&nbsp;
## Standard used
 GCC - GNU99

&nbsp;
## Usage
 Copy the source files __strview.h__/__str.c__ and optionally __strbuf.h__/__strbuf.c__ into your project.
 Add any desired options (described below) to your compiler flags (eg. -DSTRBUF_PROVIDE_PRINTF).
 str.c requires linking against the maths library for interpreting float values. So either add -lm to your linker options, or -DSTR_NO_FLOAT to your compiler options if you don't need float conversion.
 A list and explanation of options is included at the top of each header file for convenient copy & pasting.


&nbsp;
# strview.h
 strview.h provides functions for navigating, reading, and interpreting portions of const char string data. It may be used standalone, and does not depend on **strbuf.h**.

It does not store the underlying data itself, and is not intended to be used for modifying the data. The string data may be stored anywhere, string literals (the string pool), a dynamic buffer provided by strbuf.h, or a static buffer etc...

strview.h defines the following strview_t type :

	typedef struct strview_t
	{
		const char* data;
		int size;
	} strview_t;

Note that this holds only:
 * A pointer to the beginning of the string
 * The size (in characters) of the string

Some operations may return an invalid strview_t, in this case .data=NULL and .size==0. 

Note that it is valid to have a strview_t of length 0. In this case *data should never be de-referenced (as it points to something of size 0, ie non-existent).

&nbsp;
# Passing a strview_t to printf()
 There are two macros defined for this **PRIstr** and **PRIstrarg()**, which make use if printf's dynamic precision to limit the number of characters read.
 
 Example usage:

	printf("The string is %"PRIstr"\n", PRIstrarg(mystring));

&nbsp;
# strview.h functions:

&nbsp;
## strview_t cstr(const char* c_str);
 Return a strview_t from a null terminated const char[] string. If the string provided is a string literal, then the macro **cstr_SL("mystring")** may be used instead of **cstr("mystring")** to avoid measuring the strings length at runtime (although either will work).

&nbsp;
## char* strview_to_cstr(char* dst, size_t dst_size, strview_t str);
 Write a strview_t out to a null terminated char* buffer. The buffer and space available (including the terminator) must be provided by the caller.

&nbsp;
## bool strview_is_valid(strview_t str);
 Return true if the strview_t is valid.

&nbsp;
##	void strview_swap(strview_t* a, strview_t* b);
 Swap strings a and b.

&nbsp;
## bool strview_is_match(strview_t str1, strview_t str2);
 Return true if the strings match. Also returns true if BOTH strings are invalid.

&nbsp;
## bool strview_is_match_nocase(strview_t str1, strview_t str2);
 Same as **strview_is_match()** ignoring case.

&nbsp;
## int strview_compare(strview_t str1, strview_t str2);
 A replacement for strcmp(). Used for alphabetizing strings. May also be used instead of **strview_is_match()**, although keep in mind that it will return 0 if it compares an invalid string to a valid string of length 0. (Where **strview_is_match()** would return false if only one string is invalid.)

&nbsp;
## strview_t strview_sub(strview_t str, int begin, int end);
 Return the sub string indexed by **begin** to **end**, where **end** is non-inclusive.
 Negative values may be used, and will index from the end of the string backwards.
 The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string. If the requested range is entirely outside of the input string, then an invalid **strview_t** is returned.

&nbsp;
## strview_t strview_trim(strview_t str, strview_t chars_to_trim);
 Return a strview_t with the start and end trimmed of all characters present in **chars_to_trim**.

&nbsp;
## strview_t strview_trim_start(strview_t str, strview_t chars_to_trim);
 Return a strview_t with the start trimmed of all characters present in **chars_to_trim**.

&nbsp;
## strview_t strview_trim_end(strview_t str, strview_t chars_to_trim);
 Return a strview_t with the end trimmed of all characters present in **chars_to_trim**.

&nbsp;
## strview_t strview_find_first(strview_t haystack, strview_t needle);
 Return the **strview_t** for the first occurrence of needle in haystack.
 If the needle is not found, strview_find_first() returns an invalid strview_t.
 If the needle is found, the returned strview_t will match the contents of needle, only it will reference data within the haystack, and can be used with various strbuf.h functions as a means of specifying the position within the buffer.

Some special cases to consider:
 * If **needle** is valid, and of length 0, it will always be found at the start of the string.
 * If **needle** is invalid, or if **haystack** is invalid, it will not be found.
	
&nbsp;
## strview_t strview_find_last(strview_t haystack, strview_t needle);
 Similar to strview_find_first(), but returns the LAST occurrence of **needle** in **haystack**.

Some special cases to consider:
* If **needle** is valid, and of length 0, it will always be found at the end of **haystack**.
* If **needle** is invalid, or if **haystack** is invalid, it will not be found.

&nbsp;
## strview_t strview_pop_first_split(strview_t* strview_ptr, strview_t delimiters);
 Return a **strview_t** representing the contents of the source string up to, but not including, any of characters in **delimiters**.
 Additionally, the contents of the returned **strview_t**, and the delimiter character itself is removed (popped) from the input string.
 If no delimiter is found, the returned string is the entire source string, and the source string becomes invalid.

Example usage:

    strview_t date = cstr("2022/10/03");
    strview_t year  = strview_pop_first_split(&date, cstr("/"));
    strview_t month = strview_pop_first_split(&date, cstr("/"));
    strview_t day   = strview_pop_first_split(&date, cstr("/"));

&nbsp;
## strview_t strview_pop_last_split(strview_t* strview_ptr, strview_t delimiters);
 Same as **strview_pop_first_split()** but searches from the end of the string backwards.

&nbsp;
## strview_t strview_pop_first_split_nocase(strview_t* strview_ptr, strview_t delimiters);
Same as **strview_pop_first_split()** but ignores the case of the delimiters

&nbsp;
## strview_t strview_pop_last_split_nocase(strview_t* strview_ptr, strview_t delimiters);
Same as **strview_pop_last_split()** but ignores the case of the delimiters

&nbsp;
## strview_t strview_pop_split(strview_t* strview_ptr, int index);
Split a strview_t at a specified index n.
* For n >= 0
 Return a strview_t representing the first n characters of the source string.
 Additionally the first n characters are removed (popped) from the start of the source string.

* For n < 0
 Return a strview_t representing the last -n characters of the source string.
 Additionally the last -n characters are removed (popped) from the end of the source string.

If the index is outside of the range of the source string, then an invalid strview_t is returned and the source is unmodified

&nbsp;
## char strview_pop_first_char(strview_t* strview_ptr);
Return the first char of str, and remove it from the str.
Returns 0 if there are no characters in str.
 If str is known to contain at least one character, it is the equivalent of:

	strview_pop_split(&str, 1).data[0]
Only it avoids dereferencing a NULL pointer in the case where strview_pop_split() would return an invalid str due to the str being empty.

&nbsp;
## strview_t strview_pop_line(strview_t* strview_ptr, char* eol);
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

&nbsp;
## long long strview_to_ll(strview_t str);
 Convert the ascii representation of the signed long long in **str**, into a long long value.
 The input string is first  trimmed of any leading spaces, and may start with 0x or 0X for hex numbers, or 0b for binary numbers.

&nbsp;
## unsigned long long strview_to_ull(strview_t str);
 Same as **strview_to_ull()** but converts to an unsigned value.

The above **strview_to_ull** and **strview_to_ll()** are also cast to stdint.h types using macros.

	#define strview_to_int(str)         ((int)strview_to_ll(str))
	#define strview_to_int8_t(str)      ((int8_t)strview_to_ll(str))
	#define strview_to_int16_t(str)     ((int16_t)strview_to_ll(str))
	#define strview_to_int32_t(str)     ((int32_t)strview_to_ll(str))
	#define strview_to_int64_t(str)     ((int64_t)strview_to_ll(str))
	#define strview_to_unsigned(str)    ((unsigned)strview_to_ull(str))
	#define strview_to_uint8_t(str)     ((uint8_t)strview_to_ull(str))
	#define strview_to_uint16_t(str)    ((uint16_t)strview_to_ull(str))
	#define strview_to_uint32_t(str)    ((uint32_t)strview_to_ull(str))
	#define strview_to_uint64_t(str)    ((uint64_t)strview_to_ull(str))

&nbsp;
## double strview_to_float(strview_t str);
 Convert the ascii representation of a floating point value in **str**, into a double.

The default precision of this function is double, but **strview.h** accepts the following defined symbols:
* STR_NO_FLOAT - Do not provide floating point conversions. 
* STR_SUPPORT_FLOAT - Use float instead of double.
* STR_SUPPORT_LONG_DOUBLE - Use long double instead of double.

These can be added to your compiler flags eg. -DSTR_SUPPORT_LONG_DOUBLE
This feature requires linking against the maths library, so linker options will need -lm, unless you define **STR_NO_FLOAT**

&nbsp;
# strbuf.h
 strbuf.h provides functions for allocating, building and storing strings.
 Unlike the strview_t type, a strbuf_t owns the string data, and contains all the information needed to modify it, resize it, or free it.
 
 While dynamic memory allocation is very useful, it is not mandatory (with one exception regarding strbuf_cat()). 

 All strbuf functions maintain a null terminator at the end of the content, and the content may be accessed as a regular c string using mybuffer->cstr.

 strview.h defines the following strbuf_t type :

	typedef struct strbuf_t
	{
		int size;
		int capacity;
		strbuf_allocator_t allocator;
		char cstr[];
	} strbuf_t;

 Note that the size and capacity are of type int. This limits the buffer capacity to INT_MAX, which is 2GB for 32bit int's and 32kB for 16bit int's. 
 
 This type is intended to be declared as a pointer (strbuf_t*), if the buffer is relocated in memory this pointer needs to change, therefore strbuf.h functions take the address of this pointer as an argument. While a pointer to a pointer may be confusing for some, in practice the source doesn't look too intimidating. Example:

	strbuf_t*	mybuffer;
	mybuffer = strbuf_create(50, NULL);
	strbuf_assign(&mybuffer, cstr("Hello"));

As mybuffer is a pointer, members of the strbuf_t may be accessed using the arrow operator. Example:

	printf("The buffer contains %s\n", mybuffer->cstr);



&nbsp;
# Providing an allocator for strbuf_create().

 **strbuf_create()** *may* be passed an allocator. If you just want strbuf_create() to use stdlib's malloc and free, then simply add -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB to your compiler options, and pass a NULL to the allocator parameter of strbuf_create(). If you want to check that stdlib's allocation/resize actually succeeded, you can also add -DSTRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB which uses regular assert() to check this.

 The following strbuf_allocator_t type is defined by strbuf.h

	typedef struct strbuf_allocator_t
	{
		void* app_data;
		void* (*allocator)(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);
	} strbuf_allocator_t;

## Explanation:
	void* app_data;
 The address of the strbuf_allocator_t is passed to the allocator. If the allocator requires access to some implementation specific data to work (such as in the case of a temporary allocator), then *app_data may provide the address of this.

&nbsp;

	void* (*allocator)(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

A pointer to the allocator function.

&nbsp;
&nbsp;

The parameters to this function are:

	struct strbuf_allocator_t* this_allocator <-- A pointer to the strbuf_allocator_t which may be used to access ->app_data.
	void* ptr_to_free                      <-- Memory address to free OR reallocate.
	size_t size                            <-- Size of allocation, or new size of the reallocation, or 0 if memory is to be freed.
	const char* caller_filename            <-- usually /path/strbuf.c, this is to support allocators which track caller ID.
	int caller_line                        <-- The line within strbuf.c which called the allocator, this is also to support allocators which track caller ID.

&nbsp;
# Allocator example
One for stdlib's realloc is provided under allocator_example/ Even though stdlib can be used as the default allocator in the case where the user doesn't wish to provide one, it is the simplest one to use for an example.

&nbsp;
# Buffer re-sizing
The initial capacity of the buffer will be exactly as provided to strbuf_create(). If an operation needs to extend the buffer, the size will be rounded up by STRBUF_CAPACITY_GROW_STEP. The default value of this is 16, but this can be changed by defining it in a compiler flag ie. -DSTRBUF_CAPACITY_GROW_STEP=32

The buffer capacity is never shrunk, unless strbuf_shrink() is called. In which case it will be reduced to the minimum possible.

&nbsp;
# non-dynamic buffers

 A function **strbuf_create_fixed()** is provided for initializing a strbuf_t* from a given memory space and size. In this case the capacity of the buffer will never change. If an operation is attempted on the buffer which requires more space than is available, this will result in an empty buffer. The capacity will be slightly less than the buffer size, as the memory must also hold a strbuf_t, and due to this the memory provided must also be suitably aligned with __ attribute __ ((aligned)). If the memory is not aligned, or is insufficient to hold event strbuf_t, then a NULL will be returned.

&nbsp;
# printf to a strbuf_t

 To enable this feature, you must define the symbol STRBUF_PROVIDE_PRINTF, ideally by adding -DSTRBUF_PROVIDE_PRINTF to your compiler options

 **strbuf.h** will then define __strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);__

 This uses vsnprintf() from stdio.h internally, to assign the formatted text output to the buffer provided.

&nbsp;
# prnf to a strbuf_t

 To enable this feature, you must define the symbol STRBUF_PROVIDE_PRNF.
 This is an alternative text formatter available here https://github.com/mickjc750/prnf

 **strbuf.h** will then define __strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);__

&nbsp;
&nbsp;
# strbuf.h functions:

&nbsp;
##	strbuf_t* strbuf_create(size_t initial_capacity, strbuf_allocator_t* allocator);
 Create and return the address of a strbuf_t.
 If STRBUF_DEFAULT_ALLOCATOR_STDLIB is defined, then allocator may be NULL and malloc/free will be used.

&nbsp;
## strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size);
 Create a new buffer with a fixed capacity from the given memory address. The address must be suitably aligned for a void*. This can be done in GCC by adding __ attribute __ ((aligned)) to the buffers declaration.

 addr_size is the size of the memory available **(not the desired capacity)** and must be > sizeof(strbuf_t)+1.

 The resulting buffer capacity will be the given memory size -sizeof(strbuf_t)-1, and can be checked with buf->capacity. If the function fails due to bad alignment or insufficient size, a NULL will be returned.

Example use:

	#define STATIC_BUFFER_SIZE	200

	strbuf_t* buf;
	static char static_buf[STATIC_BUFFER_SIZE] __attribute__ ((aligned));
	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);

	strbuf_cat(&buf, cstr("Hello"));	// Use buffer

	strbuf_destroy(&buf);	// In this case doesn't free anything, affect is the same as buf=NULL;

&nbsp;
##	strview_t strbuf_cat(strbuf_t** buf_ptr, ...);
 This is a macro, which concatenates one or more strview_t into a buffer, and returns the strview_t of the buffer. The returned strview_t is always valid.

 After performing some argument counting wizardry, it calls _strbuf_cat(strbuf_t** buf_ptr, int n_args, ...)

 If the allocator is dynamic, input arguments may be from the output buffer itself. In this case a temporary buffer is allocated to build the output.

&nbsp;
##  strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va);
 The non-variadic version of _strbuf_cat.

&nbsp;
## strview_t strbuf_append_char(strbuf_t** buf_ptr, char c);
 Append a single character to the buffer.

&nbsp;
## strview_t strbuf_str(strbuf_t** buf_ptr);
 Return strview_t of buffer contents.

&nbsp;
## strview_t strbuf_shrink(strbuf_t** buf_ptr);
 Shrink buffer to the minimum size required to hold it's contents.
	
&nbsp;
## void strbuf_destroy(strbuf_t** buf_ptr);
 Free memory allocated to hold the buffer and it's contents.

&nbsp;
## strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str);
 Assign strview_t to buffer. strview_t may be owned by the output buffer itself.
 This allows a buffers contents to be cropped or trimmed using the strview.h functions.
 Example to trim whitespace:	strbuf_assign(&buf, strview_trim(strbuf_str(&buf), cstr(" ")));

&nbsp;
## strview_t strbuf_append(strbuf_t** buf_ptr, strview_t str);
 Append strview_t to buffer. strview_t may be owned by the output buffer itself.
	
&nbsp;
## strview_t strbuf_prepend(strbuf_t** buf_ptr, strview_t str);
 Prepend strview_t to buffer. strview_t may be owned by the output buffer itself.
	
&nbsp;
## strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, strview_t str);
 Insert strview_t to buffer at index. strview_t may be owned by the output buffer itself. The index accepts python-style negative values to index the end of the string backwards.

&nbsp;
## strview_t strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, strview_t src);
 Insert src into the buffer at the location referenced by dst. dst must reference data contained within the destination buffer.

&nbsp;
##	strview_t strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, strview_t src);
 Insert src after the end of dst in the buffer. dst must reference data contained within the buffer.


&nbsp;
&nbsp;
## strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);
## strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);
### These functions are available if you define STRBUF_PROVIDE_PRINTF, ideally by adding -DSTRBUF_PROVIDE_PRINTF to your compiler options
 These provide the variadic and non-variadic versions of printf, which output to a strbuf_t. They use vsnprintf() from stdio.h to first measure the length of the output string, then resize the buffer to suit. If the buffer is non-dynamic, and the output string does not fit, the buffer will be emptied.

&nbsp;
&nbsp;
## strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...);
## strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);
 The output is appended to the buffer. If the buffer is non-dynamic, and the output string does not fit, the buffer will be emptied.

&nbsp;
&nbsp;
## strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);
## strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);
## strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...);
## strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);
### These functions are available if you define STRBUF_PROVIDE_PRNF, ideally by adding -DSTRBUF_PROVIDE_PRNF to your compiler options
 These behave like the printf functions, but use an alternative text formatter https://github.com/mickjc750/prnf

&nbsp;

# Contributing

Please raise issues to discuss changes or new features, or comment on existing issues.  
I'm yet to tag a release, so the API is open to changes. I'm still quite active, and I value others input.
