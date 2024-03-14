/**
 * @file strbuf.h
 * @brief A buffer API complementing strview.h
 * @author Michael Clift
 * 
 * * Provides functions for allocating buffers on the heap, stack, or static memory.
 * * Provides functions for building and modifying string data.
 * * Able to use custom allocators provided at runtime.
 * * Dynamic allocation is not mandatory.
 * * Maintains null termination, so buffer contents may be accessed as a regular C string.
 * * Able to assign or append formatted text from printf() or prnf()
 * 
 * __strbuf.h__ defines the following __strbuf_t__ type :
 * 
 * 	typedef struct strbuf_t
 * 	{
 * 		int size;
 * 		int capacity;
 * 		strbuf_allocator_t allocator;
 * 		char cstr[];
 * 	} strbuf_t;
 *
 * The structure is stored in the memory preceding the buffers contents, and the type is intended to be declared as a pointer to this structure.
 * If the buffer is relocated this pointer needs to change, therefore __strbuf.h__ functions take the address of this pointer as an argument. Example:
 * 
 * 	strbuf_t*	my_buf;
 * 	my_buf = strbuf_create(50, NULL);
 * 	strbuf_assign(&my_buf, cstr("Hello"));
 * 
 * All strbuf.h functions maintain a null terminator at the end of the content.
 * 
 * As my_buf is a pointer, members of the strbuf_t may be accessed using the arrow operator. Example:
 * 
 * 	printf("The buffer contains %s\n", my_buf->cstr);
 * 
 * Functions which modify a buffers contents return a view of the resulting buffer contents.
 * If an insert or append operation fails due to insufficient capacity, the buffer will be emptied.
 * 
 * 
 * ## Build options
 *  -DSTRBUF_PROVIDE_PRINTF
 * 	Provides functions which use vprintf() internally to assign or append formatted text to a buffer.
 * 
 * -DSTRBUF_PROVIDE_PRNF
 * Similar to printf, -but uses an alternative text formatter https://github.com/mickjc750/prnf
 * 
 * -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB
 * If you wish to use dynamic memory allocation, but can't be bothered providing an allocator.
 * 
 * -DSTRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB
 * assert() that the malloc or realloc of the default allocator actually succeeded.
 * 
 * -DSTRBUF_CAPACITY_GROW_STEP=[size]
 * Defaults to 16. This is the minimum size by which the buffer will be expanded when needed.
 * 
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

/**
 * @def strbuf_space_t(cap)
 * @hideinitializer
 * @brief (macro) Used to instantiate static buffers within functions. A structure large enough to hold a strbuf_t of given capacity. 
 * @param cap The capacity of the buffer.
 * @note The structure requires initialisation using STRBUF_STATIC_INIT(cap).
 * @note The address of the structure may be cast and assigned to a strbuf_t*
 * @note For a buffer on the stack, instead use STRBUF_FIXED_CAP(cap).
 * @note Example:
 * @code{.c}
 * #define BUF_CAP 50
 * static strbuf_space_t(BUF_CAP) buf_space = STRBUF_STATIC_INIT(BUF_CAP);
 * strbuf_t* buf = (strbuf_t*)&buf_space;
 * @endcode
  **********************************************************************************/ 
	#define strbuf_space_t(cap)		struct {strbuf_t buf; char bdy[(cap)+1];}

/**
 * @def STRBUF_STATIC_INIT(cap)
 * @hideinitializer
 * @brief (macro) An initializer for the type strbuf_space_t
 * @param cap The capacity of the buffer, this must match the value passed to the strbuf_space_t() macro.
 * @note Example:
 * @code{.c}
 * #define BUF_CAP 50
 * static strbuf_space_t(BUF_CAP) buf_space = STRBUF_STATIC_INIT(BUF_CAP);
 * strbuf_t* my_buf = (strbuf_t*)&buf_space;
 * @endcode
  **********************************************************************************/ 
	#define STRBUF_STATIC_INIT(cap)		{.buf.capacity=(cap), .buf.size=0, .buf.allocator.allocator=NULL, .buf.allocator.app_data=NULL, .bdy[0]=0}

/**
 * @def STRBUF_FIXED_CAP(cap)
 * @hideinitializer
 * @brief (macro) Instantiate and provide the address of an initialized strbuf_t with a fixed capacity.
 * @param cap The capacity of the buffer.
 * @note When used within a function, the capacity may be determined at runtime by providing an integer variable for cap, and the buffer will be on the stack.
 * @note When used outside of any function, the capacity must be a literal value, and the buffer will have static storage.
 * @note Example:
 * @code{.c}
 * #define BUF_CAP 50
 * strbuf_t* my_buf = STRBUF_FIXED(BUF_CAP);
 * @endcode
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

/// @cond DEV
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
/// @endcond

/**
 * @def strbuf_cat(buf_ptr, ...)
 * @brief (macro) Concatenate an arbitrary number of string views into a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param ... One or more strview_t to be concatenated.
 * @return A view of the resulting buffer contents.
 * @note If the destination buffer is dynamic, then ... arguments may be views within the destination.
 * @note If a buffer of fixed capacity is unable to store the output, it will be emptied.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create(0,NULL);
 * strbuf_cat(&mybuf, cstr("Hello"), cstr(" World"));
 * @endcode
 **********************************************************************************/ 
 	#define strbuf_cat(buf_ptr, ...) _strbuf_cat(buf_ptr, PP_NARG(__VA_ARGS__), __VA_ARGS__)


/**
 * @def strbuf_create(init, strbuf_allocator_t* allocator)
 * @brief (macro) Create a new buffer.
 * @param init A size_t for an empty buffer, or strview_t of initial content.
 * @param allocator A pointer to a strbuf_allocator_t which provides the allocator to use, or NULL to use the default allocator.
 * @return A pointer to the newly created buffer.
 * @note If the destination buffer is dynamic, then ... arguments may be views within the destination.
 * @note If a buffer of fixed capacity is unable to store the output, it will be emptied.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create(0,NULL);
 * strbuf_t* my_buf = strbuf_create(cstr("Hello"),NULL);
 * @endcode
 **********************************************************************************/ 
	#define strbuf_create(init, alloc) _Generic((init),\
		size_t:			strbuf_create_empty,\
		int:			strbuf_create_empty,\
		strview_t:		strbuf_create_init\
		)(init, alloc)


/**
 * @struct strbuf_allocator_t
 * @brief Structure for providing the buffer with an allocator.
 * @note Example allocator using realloc:
 * @code{.c}
 * static void* my_alloc_func(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
 * {
 * 	(void)this_allocator; (void)caller_filename; (void)caller_line;
 * 	void* result;
 * 	result = realloc(ptr_to_free, size);
 * 	assert(size==0 || result);	// You need to catch a failed allocation here.
 * 	return result;
 * }
 * 	strbuf_allocator_t my_alloc = {.allocator = my_alloc_func};
 * @endcode
 */
	typedef struct strbuf_allocator_t
	{
		void* app_data; ///< A pointer to some implementation specific data which may be required by the allocator.
		/**
		 * @brief Function pointer to the allocator
		 * @param this_allocator A pointer to the instance of this structure.
		 * @param ptr_to_free Memory address to free or relocate, or NULL for new allocations.
		 * @param size Size of the new or re-sized allocation, or 0 if freeing memory.
		 */
		void* (*allocator)(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size);
	} strbuf_allocator_t;

/**
 * @struct strbuf_t
 * @brief Structure for a buffer instance.
 * This structure exists in memory before the buffers contents. The buffer is handled by a pointer to this structure, and this pointer may change if the buffer is resized.
 * Functions which manipulate the buffer accept this pointer by reference.
 */
	typedef struct strbuf_t
	{
		int size;						///< Size of the buffers contents.
		int capacity;					///< Current capacity of the buffer.
		strbuf_allocator_t allocator;	///< Allocator in use, if available.
		char cstr[];					///< Beginning of the buffers contents.
	} strbuf_t;


/**
 * @def strbuf_append(strbuf_t** buf_ptr, str);
 * @brief (macro) Append to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A C string or a view of the data to be appended.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note If the destination is of fixed capacity, and insufficient, the buffer will be emptied.
 **********************************************************************************/
	#define strbuf_append(buf_ptr, str) _Generic((str),\
		const char*:	strbuf_append_cstr,\
		char*:			strbuf_append_cstr,\
		strview_t:		strbuf_append_strview\
		)(buf_ptr, str)


/**
 * @def strbuf_prepend(strbuf_t** buf_ptr, str);
 * @brief (macro) Prepend the contents of a view to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A view or a C string of the data to be prepended.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note If the destination is of fixed capacity, and insufficient, the buffer will be emptied.
 **********************************************************************************/
	#define strbuf_prepend(buf_ptr, str) _Generic((str),\
		const char*:	strbuf_prepend_cstr,\
		char*:			strbuf_prepend_cstr,\
		strview_t:		strbuf_prepend_strview\
		)(buf_ptr, str)


/**
 * @def strbuf_insert_at_index(strbuf_t** buf_ptr, int index, str);
 * @brief (macro) Insert the contents of a view into a buffer, at a location specified by index.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param index The position within the buffer to insert at.
 * @param str A view or a C string of the data to be inserted.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 * @note A negative index may be used to reference the end of the buffer backwards.
 **********************************************************************************/
	#define strbuf_insert_at_index(buf_ptr, index, str) _Generic((str),\
		const char*:	strbuf_insert_at_index_cstr,\
		char*:			strbuf_insert_at_index_cstr,\
		strview_t:		strbuf_insert_at_index_strview\
		)(buf_ptr, index, str)


/**
 * @def strbuf_insert_before(strbuf_t** buf_ptr, strview_t dst, src);
 * @brief (macro) Insert the contents of a view into a buffer to the left of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert before.
 * @param src A view or a C string of the data to be inserted.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 **********************************************************************************/
	#define strbuf_insert_before(buf_ptr, dst, src) _Generic((src),\
		const char*:	strbuf_insert_before_cstr,\
		char*:			strbuf_insert_before_cstr,\
		strview_t:		strbuf_insert_before_strview\
		)(buf_ptr, dst, src)


/**
 * @def strbuf_insert_after(strbuf_t** buf_ptr, strview_t dst, src);
 * @brief (macro) Insert the contents of a view into a buffer to the right of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert after.
 * @param src A view or a C string of the data to be inserted.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
 **********************************************************************************/
	#define strbuf_insert_after(buf_ptr, dst, src) _Generic((src),\
		const char*:	strbuf_insert_after_cstr,\
		char*:			strbuf_insert_after_cstr,\
		strview_t:		strbuf_insert_after_strview\
		)(buf_ptr, dst, src)


/**
 * @def strbuf_strip(strbuf_t** buf_ptr, stripchars);
 * @brief (macro) Delete all occurrences of the specified characters in the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param stripchars A view or a C string of the characters which should be deleted.
 * @return A view of the buffer contents.
 **********************************************************************************/
	#define strbuf_strip(buf_ptr, stripchars) _Generic((stripchars),\
		const char*:	strbuf_strip_cstr,\
		char*:			strbuf_strip_cstr,\
		strview_t:		strbuf_strip_strview\
		)(buf_ptr, stripchars)


//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Register default allocator.
 * @param allocator A pointer to a strbuf_allocator_t which provides the allocator to use.
 * @note Replaces malloc/free as the default allocator, when none is provided to strbuf_create()
 * @note The argument is copied, so the passed structure may be temporary.
 * @endcode
  **********************************************************************************/
	void strbuf_register_default_allocator(strbuf_allocator_t allocator);

/**
 * @brief Create a new empty buffer.
 * @param initial_capacity The initial capacity of the buffer. This must be <= INT_MAX. It may  be 0.
 * @param allocator A pointer to a strbuf_allocator_t which provides the allocator to use, or NULL to use the default allocator.
 * @return A pointer to the newly created buffer.
 * @note Using the default allocator (malloc/free) requires building with -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create_empty(0,NULL);
 * @endcode
  **********************************************************************************/
	strbuf_t* strbuf_create_empty(size_t initial_capacity, strbuf_allocator_t* allocator);

/**
 * @brief Create a new buffer initialised by a strview_t
 * @param initial_content The initial content of the buffer.
 * @param allocator A pointer to a strbuf_allocator_t which provides the allocator to use, or NULL to use the default allocator.
 * @return A pointer to the newly created buffer.
 * @note Using the default allocator (malloc/free) requires building with -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create_init(0,NULL);
 * @endcode
  **********************************************************************************/
	strbuf_t* strbuf_create_init(strview_t initial_content, strbuf_allocator_t* allocator);

/**
 * @brief Create a buffer with a fixed capacity from the memory address and size provided.
 * @param addr The address of the memory space to use.
 * @param addr_size The size of the memory space to use.
 * @return A pointer to the newly created buffer.
 * @note The capacity of the buffer will be less than the memory space provided, by sizeof(strbuf_t)+1.
 * @note The memory must be suitably aligned for a void* using __attribute__ ((aligned)), or by using macro strbuf_space_t().
 * @note The maximum capacity of a buffer is INT_MAX.
 * @note Example:
 * @code{.c}
 * char buf_space[100] __attribute__ ((aligned));
 * strbuf_t* my_buf = strbuf_create_fixed(buf_space, sizeof(buf_space));
 * @endcode
  **********************************************************************************/
	strbuf_t* strbuf_create_fixed(void* addr, size_t addr_size);

/**
 * @brief Concatenate one or more string views (strview_t) and assign the result to the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param n_args The number of strview_t in the variable argument list.
 * @param ... One or more strview_t to be concatenated.
 * @return A view of the resulting buffer contents.
 * @note This function should be used via the macro strbuf_cat(strbuf_t** buf_ptr, ...) which counts the argument list for you to provide n_args.
 * @note If the destination buffer is dynamic, then ... arguments may be views within the destination.
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
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create(0,NULL);
 * strbuf_append_char(&my_buf, 'X');
 * @endcode
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

/**
 * @brief Remove metadata from strbuf and reallocate as a naked 0 terminated c string.
 * @param buf_ptr The address of a pointer to the buffer. This pointer will be NULL after the operation.
 * @return A memory allocation containing a regular c string.
 * @note Used for applications where an interface expects a regular heap allocated c string.
 * @note Care should be taken to free the returned string with the same allocator that was used to create the buffer.
 * @note If used on a static buffer, the ->cstr member is returned and *buf_ptr is NULLed.
 * @note To instead copy the buffer contents to a pre-existing memory space, use strview_to_cstr().
 **********************************************************************************/
	char* strbuf_to_cstr(strbuf_t** buf_ptr);

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
 * @brief Append to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A view of the data to be appended.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_append()
 **********************************************************************************/
	strview_t strbuf_append_strview(strbuf_t** buf_ptr, strview_t str);

/**
 * @brief Append to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A C string of the data to be appended.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_append()
 **********************************************************************************/
	strview_t strbuf_append_cstr(strbuf_t** buf_ptr, const char* str);


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
 * @brief Prepend to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A view of the data to be prepended.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_prepend())
 **********************************************************************************/
	strview_t strbuf_prepend_strview(strbuf_t** buf_ptr, strview_t str);


/**
 * @brief Prepend to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A C string to be prepended.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_prepend())
 **********************************************************************************/
	strview_t strbuf_prepend_cstr(strbuf_t** buf_ptr, const char* str);


/**
 * @brief Insert into a buffer, at a location specified by index.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param index The position within the buffer to insert at.
 * @param str A view of the data to be inserted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_insert_at_index()
 **********************************************************************************/
	strview_t strbuf_insert_at_index_strview(strbuf_t** buf_ptr, int index, strview_t str);


/**
 * @brief Insert into a buffer, at a location specified by index.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param index The position within the buffer to insert at.
 * @param str A C string of the data to be inserted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_insert_at_index()
 **********************************************************************************/
	strview_t strbuf_insert_at_index_cstr(strbuf_t** buf_ptr, int index, const char* str);


/**
 * @brief Insert into a buffer to the left of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert before.
 * @param src A view of the data to be inserted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_insert_before()
 **********************************************************************************/
	strview_t strbuf_insert_before_strview(strbuf_t** buf_ptr, strview_t dst, strview_t src);


/**
 * @brief Insert into a buffer to the left of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert before.
 * @param src A C string of the data to be inserted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_insert_before()
 **********************************************************************************/
	strview_t strbuf_insert_before_cstr(strbuf_t** buf_ptr, strview_t dst, const char* src);


/**
 * @brief Insert into a buffer to the right of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert after.
 * @param src A view of the data to be inserted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_insert_after()
 **********************************************************************************/
	strview_t strbuf_insert_after_strview(strbuf_t** buf_ptr, strview_t dst, strview_t src);


/**
 * @brief Insert into a buffer to the right of a location specified by a view within the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param dst A view within the buffer to insert after.
 * @param src A C string of the data to be inserted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_insert_after()
 **********************************************************************************/
	strview_t strbuf_insert_after_cstr(strbuf_t** buf_ptr, strview_t dst, const char* src);


/**
 * @brief Delete all occurrences of the specified characters in the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param stripchars The characters which should be deleted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_strip()
 **********************************************************************************/
	strview_t strbuf_strip_strview(strbuf_t** buf_ptr, strview_t stripchars);


/**
 * @brief Delete all occurrences of the specified characters in the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param stripchars The characters which should be deleted.
 * @return A view of the buffer contents.
 * @note Use via macro strbuf_strip()
 **********************************************************************************/
	strview_t strbuf_strip_cstr(strbuf_t** buf_ptr, const char* stripchars);


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
	strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...) __attribute__((format(printf, 2, 3)));

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
	strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...) __attribute__((format(printf, 2, 3)));

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
	strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...) __attribute__((format(printf, 2, 3)));

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
	strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...) __attribute__((format(printf, 2, 3)));

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
