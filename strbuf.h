/*
 The following may be added to compiler options:

 -DSTRBUF_PROVIDE_PRINTF
	Provides functions which use vprintf() internally to assign or append formatted text to a buffer.

 -DSTRBUF_PROVIDE_PRNF
 	Similar to printf, -but uses an alternative text formatter https://github.com/mickjc750/prnf

 -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB
	If you wish to use dynamic memory allocation, but can't be bothered providing an allocator.

 -DSTRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB
 	assert() that the malloc or realloc of the default allocator actually succeeded.

 -DSTRBUF_CAPACITY_GROW_STEP=<size>
	Defaults to 16. This is the minimum size by which the buffer will be expanded when needed.

*/
#ifndef _STRBUF_H_
	#define _STRBUF_H_

	#include <stddef.h>
	#include <stdbool.h>
	#include <stdarg.h>
	#include <string.h>

	#include "strview.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

/*	The buffer capacity is rounded up to a multiple of this when:
		* creating a new buffer with strbuf_create()
		* expanding the buffer for any reason
	The buffer size can only ever be shrunk by calling strbuf_shrink(), which shrinks it to the minimum needed.
	Higher values will reduce calls to the allocator, at the expense of more memory overhead. */
	#ifndef STRBUF_CAPACITY_GROW_STEP
		#define STRBUF_CAPACITY_GROW_STEP 16
	#endif

//	This is used for counting the number of arguments to the strbuf_cat() macro below.
// 	From https://stackoverflow.com/questions/4421681/how-to-count-the-number-of-arguments-passed-to-a-function-that-accepts-a-variabl

	#define		PP_NARG(...) 	PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
	#define 	PP_NARG_(...) 	PP_128TH_ARG(__VA_ARGS__)

	#define PP_128TH_ARG( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
         _71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
         _81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
         _91,_92,_93,_94,_95,_96,_97,_98,_99,_100, \
         _101,_102,_103,_104,_105,_106,_107,_108,_109,_110, \
         _111,_112,_113,_114,_115,_116,_117,_118,_119,_120, \
         _121,_122,_123,_124,_125,_126,_127,N,...) N

	#define PP_RSEQ_N() \
         127,126,125,124,123,122,121,120, \
         119,118,117,116,115,114,113,112,111,110, \
         109,108,107,106,105,104,103,102,101,100, \
         99,98,97,96,95,94,93,92,91,90, \
         89,88,87,86,85,84,83,82,81,80, \
         79,78,77,76,75,74,73,72,71,70, \
         69,68,67,66,65,64,63,62,61,60, \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

/*	Use strbuf_cat to concatenate an arbitrary number of strings into a buffer.
	The buffers contents itself may be used as an argument, in this case a temporary buffer will be allcoated to build the output.
	This facilitates appending, prepending, or even inserting by using str_sub().
	A string representing the result is returned. The string returned is always valid providing buf_ptr is not NULL.
	Example to append to a buffer:  strbuf_cat(&mybuffer, strbuf_str(&mybuffer), str_to_append) */
	#define strbuf_cat(buf_ptr, ...) _strbuf_cat(buf_ptr, PP_NARG(__VA_ARGS__), __VA_ARGS__)


/*	Structure for providing the buffer with an allocator.
	The allocator must return an address that is suitably aligned for any kind of variable, as allocations also contain strbuf_t.
	When a new allocation is required, ptr_to_free will be NULL and size will be >0
	When a re-allocation is required,  ptr_to_free will be not be NULL and size will be >0
	When a de-allocation is required,  ptr_to_free may or may not be NULL, and size will be 0
	stdlib's realloc handles this perfectly, example:

static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)this_allocator; (void)caller_filename; (void)caller_line;
	void* result;
	result = realloc(ptr_to_free, size);
	assert(size==0 || result);	// You need to catch a failed allocation here.
	return result;
}
then:
	strbuf_allocator_t strbuf_allocator = {.allocator = allocator}; */

	typedef struct strbuf_allocator_t
	{
		void* app_data;
		void* (*allocator)(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);
	} strbuf_allocator_t;


//	Buffer structure. Allocates and owns the buffer memory.
//	Maintains null termination, so classic c style (null terminated) strings can be accessed with ->cstr
	typedef struct strbuf_t
	{
		int size;
		int capacity;
		strbuf_allocator_t allocator;
		char cstr[];
	} strbuf_t;



//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	Create a new buffer with content size = initial_capacity  and return it.
	strbuf_t* strbuf_create(size_t initial_capacity, strbuf_allocator_t* allocator);

/*	Create a new buffer with a fixed capacity from the given memory address. The address must be suitably aligned for a void*.
	size is the size of the memory available (not the desired capacity) and must be > sizeof(strbuf_t)+1
	The resulting buffer capacity will be the given memory size -sizeof(strbuf_t)-1, and can be checked with buf->capacity
	If the function fails due to bad alignment or insufficent size, a NULL will be returned */
	strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size);

/*	Concatenate one or more strview_t and assign the result to the buffer.
	Ideally should be used from the macro strbuf_cat() which performs the argument counting for you.
	Returns a strview_t of the buffer contents.
	The returned strview_t is always valid, even if the none of the arguments are valid */
	strview_t _strbuf_cat(strbuf_t** buf_ptr, int n_args, ...);

//	The non-variadic version of _strbuf_cat
	strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va);

//	Append a single character to the buffer
	strview_t strbuf_append_char(strbuf_t** buf_ptr, char c);

//	Return strview_t of buffer contents
	strview_t strbuf_str(strbuf_t** buf_ptr);

//	Shrink buffer to the minimum size required to hold it's contents
	strview_t strbuf_shrink(strbuf_t** buf_ptr);

//	Free memory allcoated to hold the buffer and it's contents
	void strbuf_destroy(strbuf_t** buf_ptr);

/*
	For the below assign/append/prepend/insert functions:
		strview_t may be sourced from destination buffer contents,
		even for static buffers, and even if the operation relocates or splits the source string.
		eg, you can insert "fred" into the middle of itself to get "frfreded"
*/

//	Assign strview_t to buffer 
	strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str);

//	Append strview_t to buffer, strview_t 
	strview_t strbuf_append(strbuf_t** buf_ptr, strview_t str);

//	Prepend strview_t to buffer, strview_t 
	strview_t strbuf_prepend(strbuf_t** buf_ptr, strview_t str);

//	Insert strview_t to buffer, strview_t 
	strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, strview_t str);

//	Insert src at the starting location of dst in the buffer. dst must reference data contained within the buffer.
	strview_t strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, strview_t src);

//	Insert src after the end of dst in the buffer. dst must reference data contained within the buffer.
	strview_t strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, strview_t src);

// 	Provide formatted printing to a strbuf_t (uses vsnprintf() from stdio.h)
#ifdef STRBUF_PROVIDE_PRINTF
	strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);
	strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);
	strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...);
	strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);
#endif

// 	Provide formatted printing to a strbuf_t using the alternative text formatter prnf.h (https://github.com/mickjc750/prnf)
#ifdef STRBUF_PROVIDE_PRNF
	strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);
	strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);
	strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...);
	strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);
#endif

#endif
