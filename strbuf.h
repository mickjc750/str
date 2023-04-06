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

//	Used for buffers with stack or static storage. See examples/stack-buf and examples/static-buf

/**
 * @def
 * @brief (macro) A data structure large enough to hold a strbuf_t with a fixed capacity.
 * @param cap The capacity of the buffer.
 * @note If instantiated on the stack, the capacity may be determined at runtime using an integer variable.
  **********************************************************************************/ 
	#define strbuf_space_t(cap)		struct {strbuf_t buf; char bdy[(cap)+1];}

/**
 * @def
 * @brief (macro) An initializer for the type strbuf_space_t
 * @param cap The capacity of the buffer, this must match the value passed to the strbuf_space_t() macro.
  **********************************************************************************/ 
	#define STRBUF_STATIC_INIT(cap)		{.buf.capacity=(cap), .buf.size=0, .buf.allocator.allocator=NULL, .buf.allocator.app_data=NULL, .bdy[0]=0}

/**
 * @def
 * @brief (macro) Instantiate and provide the address of an initialized strbuf_t with a fixed capacity.
 * @param cap The capacity of the buffer.
 * @note When used within a function, the capacity may be determined at runtime by providing an integer variable for cap.
 * @note When used within a function the buffer will be on the stack.
 * @note When used outside of any function, the capacity must be a literal value.
 * @note When used outside of any function, the buffer will have static storage duration.
  **********************************************************************************/ 
	#define STRBUF_FIXED_CAP(cap)	((strbuf_t*)&((strbuf_space_t(cap)){.buf.capacity=(cap), .buf.size=0, .buf.allocator.allocator=NULL, .buf.allocator.app_data=NULL, .bdy[0]=0}))

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
	Example to append to a buffer:  strbuf_cat(&mybuffer, strbuf_view(&mybuffer), str_to_append) */

/**
 * @def
 * @brief (macro) Concatenate an arbitrary number of string views into a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param ... One or more strview_t to be concatenated.
 * @return A view of the resulting buffer contents.
 * @note If the destination buffer is of a fixed capacity (has no allocator), then ... may not contain views of the destination itself.
 * @note If a buffer of fixed capacity is unable to store the output, it will be emptied.
 **********************************************************************************/ 
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

/**
 * @brief Create a new buffer.
 * @param initial_capacity The initial capacity of the buffer. This must be <= INT_MAX.
 * @param allocator A pointer to a strbuf_allocator_t which provides tha allocator to use, or NULL to use the default allocator.
 * @return A pointer to the newly created buffer.
 * @note Using the default allocator (malloc/free) requires building with -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB
 * @note Building with -DSTRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB will also assert that the default allocator succeeded using assert.h
  **********************************************************************************/
	strbuf_t* strbuf_create(size_t initial_capacity, strbuf_allocator_t* allocator);

/**
 * @brief Create a buffer with a fixed capacity from the memory address and size provided.
 * @param addr The address of the memory space to use.
 * @param addr_size The size of the memory space to use.
 * @return A pointer to the newly created buffer.
 * @note The capacity of the buffer will be less than the memory space provided, by sizeof(strbuf_t)+1.
 * @note The memory must be suitably aligned for a void* using __attribute__ ((aligned)), or by using macro strbuf_space_t().
 * @note The maximum capacity of a buffer is INT_MAX.
  **********************************************************************************/
	strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size);

/**
 * @brief Concatenate one or more string views (strview_t) and assign the result to the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param n_args The number of strview_t in the variable argument list.
 * @param ... One or more strview_t to be concatenated.
 * @return A view of the resulting buffer contents.
 * @note This function should be used via the macro strbuf_cat(strbuf_t** buf_ptr, ...) which counts the argument list for you to provide n_args.
 * @note If the destination buffer is of a fixed capacity (has no allocator), then ... may not contain views of the destination itself.
 * @note If a buffer of fixed capacity is unable to store the output, it will be emptied.
 **********************************************************************************/
	strview_t _strbuf_cat(strbuf_t** buf_ptr, int n_args, ...);

/**	
 * 	@brief	The non-variadic version of _strbuf_cat
 **********************************************************************************/
	strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va);

/**
 * @brief Append a single character to the buffer.
 * 
 * @param buf_ptr The address of a pointer to the buffer.
 * @param c The character to be appended
 * @return A view of the resulting buffer contents.
 **********************************************************************************/
	strview_t strbuf_append_char(strbuf_t** buf_ptr, char c);

/**
 * @brief Get a view of the buffer contents.
 * @param buf_ptr The address of a pointer to the buffer.
 **********************************************************************************/
	strview_t strbuf_view(strbuf_t** buf_ptr);

/**
 * @brief Reduce buffer capacity to the minimum size required to hold it's contents.
 * @param buf_ptr The address of a pointer to the buffer.
 * @return A view of the buffer contents.
 * @note This will have no effect on a buffer with fixed capacity.
 **********************************************************************************/
	strview_t strbuf_shrink(strbuf_t** buf_ptr);

/**
 * @brief Increase the capacity of the buffer to be at least the size specified.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param min_size The buffer capacity required.
 * @return A view of the buffer contents, or STRVIEW_INVALID if the operation fails.
 * @note This can only increase the buffers capacity, to reduce it use strbuf_shrink().
 * @note The operation will fail if attempted on a buffer with fixed capacity.
 **********************************************************************************/
	strview_t strbuf_grow(strbuf_t** buf_ptr, int min_size);

/**
 * @brief Free memory allcoated to hold the buffer and it's contents.
 * @param buf_ptr The address of a pointer to the buffer. This pointer will be NULL after the operation.
 * @note Calling this with a buffer of fixed capacity is unnecessary, but harmless. It will only NULL the passed pointer.
 **********************************************************************************/
	void strbuf_destroy(strbuf_t** buf_ptr);


/*
	For the below assign/append/prepend/insert functions:
		strview_t may be sourced from destination buffer contents,
		even for static buffers, and even if the operation relocates or splits the source string.
		eg, you can insert "fred" into the middle of itself to get "frfreded"
*/

/**
 * @brief Assign the contents of a view to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A view of the data to be assigned.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note If the source view is invalid, the buffer will be emptied.
 * @note If the destination is of fixed capacity, and insufficient, the buffer will be emptied.
 **********************************************************************************/
	strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str);

/**
 * @brief Append the contents of a view to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A view of the data to be appended.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note If the destination is of fixed capacity, and insufficient, the buffer will be emptied.
 **********************************************************************************/
	strview_t strbuf_append(strbuf_t** buf_ptr, strview_t str);


/**
 * @private
 *	The fetch function must have the following signature and behaviour:
 *	int fetch(void* dst, int dst_size, void* fetcher_vars);
 *	Where:
 *		dst is the address to write data
 *		dst_size is the maximum number of bytes to write, this will be passed the amount of free space in the buffer (which may be 0)
 *		fetch_vars points to application specific data needed by the fetch function (usually a struct)
 *		Return value must be the number of bytes fetched, which may be 0 to dst_size (inclusive).
 *
 *	If you wish to fetch more bytes than the available space in the buffer, use strbuf_grow() first
 *	If the return value of the fetch indicates bad behaviour (<0 or >dst_size) then the buffer is emptied and an invalid strview_t is returned.
 **********************************************************************************/
	strview_t strbuf_append_using(strbuf_t** buf_ptr, int (*strbuf_fetcher)(void* dst, int dst_size, void* fetcher_vars), void* fetch_vars);

/**
 * @brief Prepend the contents of a view to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A view of the data to be prepended.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note If the destination is of fixed capacity, and insufficient, the buffer will be emptied.
 **********************************************************************************/
	strview_t strbuf_prepend(strbuf_t** buf_ptr, strview_t str);

/**
 * @brief Insert the contents of a view into a buffer, at a location specified by index.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param index The position within the buffer to insert at.
 * @param str A view of the data to be inserted.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note A negative index may be used to reference the end of the buffer backwards.
 **********************************************************************************/
	strview_t strbuf_insert_at_index(strbuf_t** buf_ptr, int index, strview_t str);

/**
 * @brief Insert the contents of a view into a buffer to the left of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert before.
 * @param src A view of the data to be inserted.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 **********************************************************************************/
	strview_t strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, strview_t src);

/**
 * @brief Insert the contents of a view into a buffer to the right of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert after.
 * @param src A view of the data to be inserted.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 **********************************************************************************/
	strview_t strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, strview_t src);

/**
 * @brief Delete all occurrences of the specified characters in the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param stripchars A view of the characters which should be deleted.
 * @return A view of the buffer contents.
 **********************************************************************************/
	strview_t strbuf_strip(strbuf_t** buf_ptr, strview_t stripchars);

#ifdef STRBUF_PROVIDE_PRINTF
/**
 * @brief printf formatted text to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param ... The arguments for the placeholders.
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRINTF
 * @note This uses vsnprintf() from stdio.h
 **********************************************************************************/
	strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...);

/**
 * @brief non-variadic version of strbuf_printf().
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param va A variable argument list as a va_list type from stdarg.h
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRINTF
 * @note This uses vsnprintf() from stdio.h
 **********************************************************************************/
	strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);

/**
 * @brief append printf formatted text to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param ... The arguments for the placeholders.
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRINTF
 * @note This uses vsnprintf() from stdio.h
 **********************************************************************************/
	strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...);

/**
 * @brief non-variadic version of strbuf_append_printf().
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param va A variable argument list as a va_list type from stdarg.h
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRINTF
 * @note This uses vsnprintf() from stdio.h
 **********************************************************************************/
	strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va);
#endif

#ifdef STRBUF_PROVIDE_PRNF
/**
 * @brief prnf formatted text to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param ... The arguments for the placeholders.
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRNF
 * @note This uses vfptrprnf() from prnf.h
 **********************************************************************************/
	strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...);

/**
 * @brief non-variadic version of strbuf_prnf().
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param va A variable argument list as a va_list type from stdarg.h
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRNF
 * @note This uses vfptrprnf() from prnf.h
 **********************************************************************************/
	strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);

/**
 * @brief Append prnf formatted text to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param ... The arguments for the placeholders.
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRNF
 * @note This uses vfptrprnf() from prnf.h
 **********************************************************************************/
	strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...);

/**
 * @brief non-variadic version of strbuf_append_prnf().
 * @param buf_ptr The address of a pointer to the buffer.
 * @param format The format string containing text and % placeholders.
 * @param va A variable argument list as a va_list type from stdarg.h
 * @return A view of the buffer contents.
 * @note Only available if build with -DSTRBUF_PROVIDE_PRNF
 * @note This uses vfptrprnf() from prnf.h
 **********************************************************************************/
	strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va);
#endif

#endif
