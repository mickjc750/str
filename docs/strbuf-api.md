# strbuf.h

&nbsp; 
&nbsp; 
# Contents
1. [About](#about)
2. [Providing an allocator](#providing-an-allocator-for-strbuf_create)
3. [Allocator example](#allocator-example)
4. [Buffer re-sizing](#buffer-re-sizing)

# Function reference
- [strbuf.h](#strbufh)
- [Contents](#contents)
- [Function reference](#function-reference)
	- [About](#about)
- [Providing an allocator for strbuf\_create().](#providing-an-allocator-for-strbuf_create)
	- [Explanation:](#explanation)
- [Allocator example](#allocator-example)
- [Buffer re-sizing](#buffer-re-sizing)
- [Assigning buffer contents using printf](#assigning-buffer-contents-using-printf)
- [Assigning buffer contents using prnf](#assigning-buffer-contents-using-prnf)
- [Function reference](#function-reference-1)
	- [`strbuf_t* strbuf_create(size_t initial_capacity);`](#strbuf_t-strbuf_createsize_t-initial_capacity)
	- [`strbuf_t* strbuf_create(strview_t initial_content);`](#strbuf_t-strbuf_createstrview_t-initial_content-)
	- [`void strbuf_destroy(strbuf_t** buf_ptr);`](#void-strbuf_destroystrbuf_t-buf_ptr)
	- [`char* strbuf_to_cstr(strbuf_t** buf_ptr);`](#char-strbuf_to_cstrstrbuf_t-buf_ptr)
	- [`strview_t strbuf_view(strbuf_t** buf_ptr);`](#strview_t-strbuf_viewstrbuf_t-buf_ptr)
	- [`strview_t strbuf_shrink(strbuf_t** buf_ptr);`](#strview_t-strbuf_shrinkstrbuf_t-buf_ptr)
	- [`strview_t strbuf_grow(strbuf_t** buf_ptr, int min_size);`](#strview_t-strbuf_growstrbuf_t-buf_ptr-int-min_size)
	- [`strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str);`](#strview_t-strbuf_assignstrbuf_t-buf_ptr-strview_t-str)
	- [`strview_t strbuf_cat(strbuf_t** buf_ptr, ...);`](#strview_t-strbuf_catstrbuf_t-buf_ptr-)
	- [`strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va);`](#strview_t-strbuf_vcatstrbuf_t-buf_ptr-int-n_args-va_list-va)
	- [`strview_t strbuf_append(strbuf_t** buf_ptr, str);`](#strview_t-strbuf_appendstrbuf_t-buf_ptr-str)
	- [`strview_t strbuf_append_char(strbuf_t** buf_ptr, char c);`](#strview_t-strbuf_append_charstrbuf_t-buf_ptr-char-c)
	- [`strview_t strbuf_prepend(strbuf_t** buf_ptr, str);`](#strview_t-strbuf_prependstrbuf_t-buf_ptr-str)
	- [`strview_t strbuf_strip(strbuf_t** buf_ptr, stripchars);`](#strview_t-strbuf_stripstrbuf_t-buf_ptr-stripchars)
	- [`strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, str);`](#strview_t-strbuf_insert_at_indexstrbuf_t-buf_ptr-int-index-str)
	- [`strview_t strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, src);`](#strview_t-strbuf_insert_beforestrbuf_t-buf_ptr-strview_t-dst-src)
	- [`strview_t strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, src);`](#strview_t-strbuf_insert_afterstrbuf_t-buf_ptr-strview_t-dst-src)
	- [`strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);`](#strview_t-strbuf_printfstrbuf_t-buf_ptr-const-char-format-)
	- [`strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);`](#strview_t-strbuf_vprintfstrbuf_t-buf_ptr-const-char-format-va_list-va)
		- [These functions are available if you define STRBUF\_PROVIDE\_PRINTF, ideally by adding -DSTRBUF\_PROVIDE\_PRINTF to your compiler options](#these-functions-are-available-if-you-define-strbuf_provide_printf-ideally-by-adding--dstrbuf_provide_printf-to-your-compiler-options)
	- [`strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);`](#strview_t-strbuf_prnfstrbuf_t-buf_ptr-const-char-format-)
	- [`strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);`](#strview_t-strbuf_vprnfstrbuf_t-buf_ptr-const-char-format-va_list-va)
		- [These functions are available if you define STRBUF\_PROVIDE\_PRNF, ideally by adding -DSTRBUF\_PROVIDE\_PRNF to your compiler options](#these-functions-are-available-if-you-define-strbuf_provide_prnf-ideally-by-adding--dstrbuf_provide_prnf-to-your-compiler-options)
	- [`strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...);`](#strview_t-strbuf_append_prnfstrbuf_t-buf_ptr-const-char-format-)
	- [`strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);`](#strview_t-strbuf_append_vprnfstrbuf_t-buf_ptr-const-char-format-va_list-va)
		- [These functions are available if you define STRBUF\_PROVIDE\_PRNF, ideally by adding -DSTRBUF\_PROVIDE\_PRNF to your compiler options](#these-functions-are-available-if-you-define-strbuf_provide_prnf-ideally-by-adding--dstrbuf_provide_prnf-to-your-compiler-options-1)
	- [`strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...);`](#strview_t-strbuf_append_printfstrbuf_t-buf_ptr-const-char-format-)
	- [`strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);`](#strview_t-strbuf_append_vprintfstrbuf_t-buf_ptr-const-char-format-va_list-va)
		- [These functions are available if you define STRBUF\_PROVIDE\_PRINTF, ideally by adding -DSTRBUF\_PROVIDE\_PRINTF to your compiler options](#these-functions-are-available-if-you-define-strbuf_provide_printf-ideally-by-adding--dstrbuf_provide_printf-to-your-compiler-options-1)
	- [`strview_t strbuf_terminate_views(strbuf_t** buf_ptr, int count, strview_t src[count]);`](#strview_t-strbuf_terminate_viewsstrbuf_t-buf_ptr-int-count-strview_t-srccount)


## About
 strbuf.h provides functions for allocating, building and storing strings.
 Unlike the strview_t type, a strbuf_t owns the string data, and contains all the information needed to modify it, resize it, or free it.

 All strbuf functions maintain a null terminator at the end of the content, and the content may be accessed as a regular c string using mybuffer->cstr.

 __strbuf.h__ defines the following __strbuf_t__ type :

	typedef struct strbuf_t
	{
		int size;
		int capacity;
		char cstr[];
	} strbuf_t;

 Note that the size and capacity are of type int. This limits the buffer capacity to INT_MAX, which is approximately 2GB for 32bit int's and approximately 32kB for 16bit int's. 

&nbsp; 
 This type is intended to be declared as a pointer __(strbuf_t*)__, if the buffer is relocated in memory this pointer needs to change, therefore __strbuf.h__ functions take the address of this pointer as an argument. While a pointer to a pointer may be confusing for some, in practice the source doesn't look too intimidating. Example:

	strbuf_t*	mybuffer;
	mybuffer = strbuf_create(50);
	strbuf_assign(&mybuffer, cstr("Hello"));


As mybuffer is a pointer, members of the strbuf_t may be accessed using the arrow operator. Example:

	printf("The buffer contains %s\n", mybuffer->cstr);



&nbsp;
# Providing an allocator for strbuf_create().
 This is done by providing the functions or macros strbuf_alloc(), strbuf_free(), and strbuf_realloc(), with signatures matching stdlib's malloc() free() realloc(). strbuf_realloc() is never passed a size of 0.

&nbsp;
# Allocator example

	#include <stdlib.h>
	#define strbuf_alloc(sz)			malloc(sz)
	#define strbuf_realloc(ptr, sz)		realloc(ptr, sz)
	#define strbuf_free(ptr)			free(ptr)

	#define STRBUF_IMPLEMENTATION
	#include "strbuf.h"

	**Note** strbuf.h will NOT test the result of strbuf_alloc() or strbuf_realloc() for NULL. If you wish to check for allocator failure this must be done inside the applications provided strbuf_alloc() and strbuf_realloc().

&nbsp;
# Buffer re-sizing
The initial capacity of the buffer will be exactly as provided to strbuf_create(). If an operation needs to extend the buffer, the size will be increased by a ratio determined by 1/2^(STRBUF_CAPACITY_GROW_RATIO). If not defined STRBUF_CAPACITY_GROW_RATIO defaults to 1 which corresponds to an increase of 1/2^1 (or 50%). 

The buffer capacity is never shrunk, unless strbuf_shrink() is called. In which case it will be reduced to the minimum possible.

&nbsp;
# Assigning buffer contents using printf

 To enable this feature, you must define the symbol __STRBUF_PROVIDE_PRINTF__, ideally by adding __-DSTRBUF_PROVIDE_PRINTF__ to your compiler options.

 **strbuf.h** will then define __strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);__

 This uses vsnprintf() from stdio.h internally, to assign the formatted text output to the buffer provided.

&nbsp;
# Assigning buffer contents using prnf

 If you don't wish to use printf() because of its size or performance. There is a lightweight and fast alternative text formatter available.
 https://github.com/mickjc750/prnf
 To enable this feature, you must define the symbol __STRBUF_PROVIDE_PRNF__.
 
 **strbuf.h** will then define __strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);__

 prnf.h is not included in this repository. The application must provide the prnf implementation in a similar fashion to strbuf.h(stb single header style).

&nbsp;
&nbsp;
# Function reference

&nbsp;
## `strbuf_t* strbuf_create_empty(size_t initial_capacity);`
 Creates and returns the address of an empty buffer.

&nbsp;
## `strbuf_t* strbuf_create_init(strview_t initial_content);`
 Creates and returns the address of a buffer initialized with initial_content.

&nbsp;
## `strbuf_create(init)`
Macro.

Accepts:
- int / size_t → create empty buffer
- strview_t → create initialized buffer

&nbsp;
## `void strbuf_destroy(strbuf_t** buf_ptr);`
 Free memory allocated to hold the buffer and its contents. buf_ptr is nulled.

&nbsp;
## `char* strbuf_to_cstr(strbuf_t** buf_ptr);`
 Remove metadata from strbuf and reallocate as a naked 0 terminated c string. buf_ptr is nulled.
 Used for applications where an interface expects a regular heap allocated c string.
 Care should be taken to free the returned string with the same allocator that was used to create the buffer.
 To instead copy the buffer contents to a pre-existing memory space, use strview_to_cstr().

&nbsp;
## `strview_t strbuf_view(strbuf_t** buf_ptr);`
 Return strview_t of buffer contents.

&nbsp;
## `strview_t strbuf_shrink(strbuf_t** buf_ptr);`
 Shrink buffer to the minimum size required to hold its contents.

&nbsp;
## `strview_t strbuf_grow(strbuf_t** buf_ptr, int min_size);`
 Grow the capacity of the buffer to be at minimum the size specified.
 A strview_t of the existing buffer contents is returned.

&nbsp;
## `strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str);`
 Assign strview_t to buffer. strview_t may be owned by the output buffer itself.
 This allows a buffers contents to be cropped or trimmed using the strview.h functions.
 Example to trim whitespace:

	strbuf_assign(&buf, strview_trim(strbuf_view(&buf), cstr(" ")));

&nbsp;
##	`strview_t strbuf_cat(strbuf_t** buf_ptr, ...);`
 This is a macro, which concatenates one or more strview_t into a buffer, and returns the strview_t of the buffer. The returned strview_t is always valid providing buf_ptr and *buf_ptr are valid. Note that unlike strcat() this overwrites the previous buffer contents instead of appending to it. You may include the original buffer contents by passing a view of it as one of the arguments.

&nbsp;
##  `strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va);`
 The non-variadic version of _strbuf_cat.

&nbsp;
## `strview_t strbuf_append(strbuf_t** buf_ptr, str);`
 Append to the buffer. **str** may either be a C string or a strview_t.

&nbsp;
## `strview_t strbuf_append_char(strbuf_t** buf_ptr, char c);`
 Append a single character to the buffer.

&nbsp;
## `strview_t strbuf_prepend(strbuf_t** buf_ptr, str);`
 Prepend to buffer.  **str** may either be a C string or a strview_t.

&nbsp;
## `strview_t strbuf_strip(strbuf_t** buf_ptr, stripchars);`
 Strip buffer contents of characters in stripchars, which may either be a C string or strview_t.

&nbsp;
## `strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, str);`
 Insert into buffer at index. str may be a C string or a strview_t. Negative indices count backward from the end of the string.

&nbsp;
## `strview_t strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, src);`
 Insert src into the buffer at the location referenced by dst. dst must reference data contained within the destination buffer. src may be a C string or a strview_t

&nbsp;
## `strview_t strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, src);`
 Insert src after the end of dst in the buffer. dst must reference data contained within the buffer.  src may be a C string or a strview_t

&nbsp;
&nbsp;
## `strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);`
## `strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);`
### These functions are available if you define STRBUF_PROVIDE_PRINTF, ideally by adding -DSTRBUF_PROVIDE_PRINTF to your compiler options
 These provide the variadic and non-variadic versions of printf, which output to a strbuf_t. They use vsnprintf() from stdio.h to first measure the length of the output string, then resize the buffer to suit.

&nbsp;
&nbsp;
## `strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);`
## `strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);`
### These functions are available if you define STRBUF_PROVIDE_PRNF, ideally by adding -DSTRBUF_PROVIDE_PRNF to your compiler options
 These use the alternative text formatter prnf https://github.com/mickjc750/prnf to assign the output to a strbuf_t. 

&nbsp;
&nbsp;
## `strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...);`
## `strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);`
### These functions are available if you define STRBUF_PROVIDE_PRNF, ideally by adding -DSTRBUF_PROVIDE_PRNF to your compiler options
 These use the alternative text formatter prnf https://github.com/mickjc750/prnf to append the output to a strbuf_t. 

&nbsp;
&nbsp;
## `strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...);`
## `strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);`
### These functions are available if you define STRBUF_PROVIDE_PRINTF, ideally by adding -DSTRBUF_PROVIDE_PRINTF to your compiler options
 These provide the variadic and non-variadic versions of printf, which append their output to a strbuf_t. They use vsnprintf() from stdio.h to first measure the length of the output string, then resize the buffer to suit.

&nbsp;
## `strview_t strbuf_terminate_views(strbuf_t** buf_ptr, int count, strview_t src[count]);`
 Given an array of views within a buffer, 0 terminate each view within the buffer.
 The end result is a concatenation of each 0 terminated view, and other data between the views is lost.
 The views themselves (src[]) are modified to view the now 0-terminated contents within the buffer, including the terminator. 
