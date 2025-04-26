# strbuf.h

&nbsp; 
&nbsp; 
# Contents
1. [About](#about)
2. [Providing an allocator](#providing-an-allocator-for-strbuf_create)
3. [Allocator example](#allocator-example)
4. [Buffer re-sizing](#buffer-re-sizing)
5. [Using static or stack allocated buffers](#using-static-or-stack-allocated-buffers)

# Function reference
 * [strbuf_t* strbuf_create(size_t initial_capacity, strbuf_allocator_t* allocator)](#strbuf_t-strbuf_createsize_t-initial_capacity-strbuf_allocator_t-allocator)
 * [strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size)](#strbuf_t-strbuf_create_fixedvoid-addr-size_t-addr_size)
 * [void strbuf_destroy(strbuf_t** buf_ptr)](#void-strbuf_destroystrbuf_t-buf_ptr)
 * [strview_t strbuf_view(strbuf_t** buf_ptr)](#strview_t-strbuf_viewstrbuf_t-buf_ptr)
 * [strview_t strbuf_shrink(strbuf_t** buf_ptr)](#strview_t-strbuf_shrinkstrbuf_t-buf_ptr)
 * [strview_t strbuf_grow(strbuf_t** buf_ptr, int min_size)](#strview_t-strbuf_growstrbuf_t-buf_ptr-int-min_size)
 * [strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str)](#strview_t-strbuf_assignstrbuf_t-buf_ptr-strview_t-str)
 * [strview_t strbuf_cat(strbuf_t** buf_ptr, ...)](#strview_t-strbuf_catstrbuf_t-buf_ptr)
 * [strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va)](#strview_t-strbuf_vcatstrbuf_t-buf_ptr-int-n_args-va_list-va)
 * [strview_t strbuf_append(strbuf_t** buf_ptr, strview_t str)](#strview_t-strbuf_appendstrbuf_t-buf_ptr-str)
 * [strview_t strbuf_append_char(strbuf_t** buf_ptr, char c)](#strview_t-strbuf_append_charstrbuf_t-buf_ptr-char-c)
 * [strview_t strbuf_prepend(strbuf_t** buf_ptr, strview_t str)](#strview_t-strbuf_prependstrbuf_t-buf_ptr-str)
 * [strview_t strbuf_strip(strbuf_t** buf_ptr, strview_t stripchars)](#strview_t-strbuf_stripstrbuf_t-buf_ptr-stripchars)
 * [strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, strview_t str)](#strview_t-strbuf_insert_at_indexstrbuf_t-buf_ptr-int-index-str)
 * [strview_t strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, strview_t src)](#strview_t-strbuf_insert_beforestrbuf_t-buf_ptr-strview_t-dst-src)
 * [strview_t strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, strview_t src)](#strview_t-strbuf_insert_afterstrbuf_t-buf_ptr-strview_t-dst-src)
 * [strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...)](#strview_t-strbuf_printfstrbuf_t-buf_ptr-const-char-format)
 * [strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va)](#strview_t-strbuf_vprintfstrbuf_t-buf_ptr-const-char-format-va_list-va)
 * [strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...)](#strview_t-strbuf_prnfstrbuf_t-buf_ptr-const-char-format)
 * [strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va)](#strview_t-strbuf_vprnfstrbuf_t-buf_ptr-const-char-format-va_list-va)
 * [strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...)](#strview_t-strbuf_append_printfstrbuf_t-buf_ptr-const-char-format)
 * [strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va)](#strview_t-strbuf_append_vprintfstrbuf_t-buf_ptr-const-char-format-va_list-va)
 * [strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...)](#strview_t-strbuf_append_prnfstrbuf_t-buf_ptr-const-char-format)
 * [strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va)](#strview_t-strbuf_append_vprnfstrbuf_t-buf_ptr-const-char-format-va_list-va)


## About
 strbuf.h provides functions for allocating, building and storing strings.
 Unlike the strview_t type, a strbuf_t owns the string data, and contains all the information needed to modify it, resize it, or free it.
 
 While dynamic memory allocation is very useful, it is not mandatory (with one exception regarding strbuf_cat()). 

 All strbuf functions maintain a null terminator at the end of the content, and the content may be accessed as a regular c string using mybuffer->cstr.

 __strbuf.h__ defines the following __strbuf_t__ type :

	typedef struct strbuf_t
	{
		int size;
		int capacity;
		strbuf_allocator_t allocator;
		char cstr[];
	} strbuf_t;

 Note that the size and capacity are of type int. This limits the buffer capacity to INT_MAX, which is 2GB for 32bit int's and 32kB for 16bit int's. 

&nbsp; 
 This type is intended to be declared as a pointer __(strbuf_t*)__, if the buffer is relocated in memory this pointer needs to change, therefore __strbuf.h__ functions take the address of this pointer as an argument. While a pointer to a pointer may be confusing for some, in practice the source doesn't look too intimidating. Example:

	strbuf_t*	mybuffer;
	mybuffer = strbuf_create(50, NULL);
	strbuf_assign(&mybuffer, cstr("Hello"));


As mybuffer is a pointer, members of the strbuf_t may be accessed using the arrow operator. Example:

	printf("The buffer contains %s\n", mybuffer->cstr);



&nbsp;
# Providing an allocator for strbuf_create().

 **strbuf_create()** *may* be passed an allocator. If you just want strbuf_create() to use stdlib's malloc and free, then simply add -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB to your compiler options, and pass a NULL to the allocator parameter of strbuf_create(). If you want to check that stdlib's allocation/resize actually succeeded, you can also add -DSTRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB which uses regular assert() to check this.
 If you don't want to use stdlib's malloc and free, and also don't want to pass your custom allocator to every occurrence of strbuf_create(), then you can provide a default allocator named **strbuf_default_allocator** (see the examples/ provided)

 The following __strbuf_allocator_t__ type is defined by __strbuf.h__

	typedef struct strbuf_allocator_t
	{
		void* app_data;
		void* (*allocator)(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size);
	} strbuf_allocator_t;

## Explanation:
	void* app_data;
 The address of the strbuf_allocator_t is passed to the allocator. If the allocator requires access to some implementation specific data to work (such as in the case of a temporary allocator), then *app_data may provide the address of this.

&nbsp;

	void* (*allocator)(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size);

A pointer to the allocator function.

&nbsp;
The parameters to this function are:

	struct strbuf_allocator_t* this_allocator <-- A pointer to the strbuf_allocator_t which may be used to access ->app_data.
	void* ptr_to_free                      <-- Memory address to free OR reallocate.
	size_t size                            <-- Size of allocation, or new size of the reallocation, or 0 if memory is to be freed.

&nbsp;
# Allocator example
Even though stdlib's realloc can be used as the default allocator in the case where the user doesn't wish to provide one, it is the simplest one to use for an example.

&nbsp;
This also available in [/examples/custom_allocator](/examples/custom_allocator/heap-buf.c)

	static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size)
	{
		(void)this_allocator;
		void* result;
		result = realloc(ptr_to_free, size);
		assert(size==0 || result);	// You need to catch a failed allocation here.
		return result;
	}


&nbsp;
# Buffer re-sizing
The initial capacity of the buffer will be exactly as provided to strbuf_create(). If an operation needs to extend the buffer, the size will be rounded up by STRBUF_CAPACITY_GROW_STEP. The default value of this is 16, but this can be changed by defining it in a compiler flag ie. -DSTRBUF_CAPACITY_GROW_STEP=32

The buffer capacity is never shrunk, unless strbuf_shrink() is called. In which case it will be reduced to the minimum possible.

&nbsp;
# Using static or stack allocated buffers

A function **strbuf_create_fixed()** is provided for initializing a __strbuf_t*__ from a given memory space and size. In this case the capacity of the buffer will never change. If an operation is attempted on the buffer which requires more space than available, this will result in an empty buffer. The capacity will be slightly less than the buffer size, as the memory must also hold a __strbuf_t__, and due to this the memory provided must also be suitably aligned with **__attribute__((aligned))**. If the memory is not aligned, or is of insufficient space to hold even __strbuf_t__, then a NULL will be returned.

Examples of using stack and static buffers are available in __/examples__

With the exception of strbuf_cat(), all buffer operations can source data from the destination itself. The following example which you might expect to fail, works fine, without any need to create a temporary buffer: 

	strbuf_assign(&mybuf, cstr("Fred"));
	strbuf_insert_at_index(&mybuf, 2, strbuf_view(&mybuf));	// Results in "FrFreded"

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

&nbsp;
&nbsp;
# Function reference

&nbsp;
## `strbuf_t* strbuf_create(size_t initial_capacity, strbuf_allocator_t* allocator);`
## `strbuf_t* strbuf_create(strview_t initial_content, strbuf_allocator_t* allocator);`
 A generic macro, which creates and returns the address of an empty buffer of initial_capacity, or a buffer initialized with initial_content.

 allocator may be NULL to use malloc/free (requires STRBUF_DEFAULT_ALLOCATOR_STDLIB) or a default allocator registered with strbuf_register_default_allocator()

&nbsp;
## `strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size);`
 Create a new buffer with a fixed capacity from the given memory address. The address must be suitably aligned for a void*. This can be done in GCC by adding __ attribute __ ((aligned)) to the buffers declaration.

&nbsp;
 addr_size is the size of the memory available **(not the desired capacity)** and must be > sizeof(strbuf_t)+1.

&nbsp;
 The resulting buffer capacity will be the given memory size -sizeof(strbuf_t)-1, and can be checked with buf->capacity. If the function fails due to bad alignment or insufficient size, a NULL will be returned.

Example use:

	#define STATIC_BUFFER_SIZE	200

	strbuf_t* buf;
	static char static_buf[STATIC_BUFFER_SIZE] __attribute__ ((aligned));
	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);

	strbuf_cat(&buf, cstr("Hello"));	// Use buffer

	strbuf_destroy(&buf);	// In this case doesn't free anything, affect is the same as buf=NULL;

&nbsp;
## `void strbuf_destroy(strbuf_t** buf_ptr);`
 Free memory allocated to hold the buffer and its contents. buf_ptr is nulled.

&nbsp;
## `char* strbuf_to_cstr(strbuf_t** buf_ptr);`
 Remove metadata from strbuf and reallocate as a naked 0 terminated c string. buf_ptr is nulled.
 Used for applications where an interface expects a regular heap allocated c string.
 Care should be taken to free the returned string with the same allocator that was used to create the buffer.
 If used on a static buffer, the ->cstr member is returned and *buf_ptr is still nulled.
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
 If the operation fails, due to the buffer being static, an invalid strview_t is returned.
 Otherwise a strview_t of the existing buffer *contents* (which may be smaller or greater than min_size) is returned.

&nbsp;
## `strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str);`
 Assign strview_t to buffer. strview_t may be owned by the output buffer itself.
 This allows a buffers contents to be cropped or trimmed using the strview.h functions.
 Example to trim whitespace:

	strbuf_assign(&buf, strview_trim(strbuf_view(&buf), cstr(" ")));

&nbsp;
##	`strview_t strbuf_cat(strbuf_t** buf_ptr, ...);`
 This is a macro, which concatenates one or more strview_t into a buffer, and returns the strview_t of the buffer. The returned strview_t is always valid.

&nbsp;
 After performing some argument counting wizardry, it calls **`_strbuf_cat(strbuf_t** buf_ptr, int n_args, ...)`**

&nbsp;
 If the allocator is dynamic, input arguments may be from the output buffer itself. In this case a temporary buffer is allocated to build the output.

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
 Strip buffer contents of characters in stripchars, which may either be a C string or s strview_t.

&nbsp;
## `strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, str);`
 Insert into buffer at index. str may be a C string or a strview_t. The index accepts python-style negative values to index the end of the string backwards.

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
 These provide the variadic and non-variadic versions of printf, which output to a strbuf_t. They use vsnprintf() from stdio.h to first measure the length of the output string, then resize the buffer to suit. If the buffer is non-dynamic, and the output string does not fit, the buffer will be emptied.

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
 These provide the variadic and non-variadic versions of printf, which append their output to a strbuf_t. They use vsnprintf() from stdio.h to first measure the length of the output string, then resize the buffer to suit. If the buffer is non-dynamic, and the output string does not fit, the buffer will be emptied.

&nbsp;
## `strview_t strbuf_terminate_views(strbuf_t** buf_ptr, int count, strview_t src[count]);`
 Given an array of views within a buffer, 0 terminate each view within the buffer.
 The end result is a concatenation of each 0 terminated view, and other data between the views is lost.
 The views themselves (src[]) are modified to view the now 0-terminated contents within the buffer, including the terminator. 
