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
 * 		char cstr[];
 * 	} strbuf_t;
 *
 * The structure is stored in the memory preceding the buffers contents, and the type is intended to be declared as a pointer to this structure.
 * If the buffer is relocated this pointer needs to change, therefore __strbuf.h__ functions take the address of this pointer as an argument. Example:
 * 
 * 	strbuf_t*	my_buf;
 * 	my_buf = strbuf_create(50);
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
 *  -DSTRBUF_PROVIDE_PRNF
 *  Similar to printf, -but uses an alternative text formatter https://github.com/mickjc750/prnf
 * 
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
 * @note Arguments may be views within the destination.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create(0);
 * strbuf_cat(&mybuf, cstr("Hello"), cstr(" World"));
 * @endcode
 **********************************************************************************/ 
 	#define strbuf_cat(buf_ptr, ...) _strbuf_cat(buf_ptr, PP_NARG(__VA_ARGS__), __VA_ARGS__)


/**
 * @def strbuf_create(init)
 * @brief (macro) Create a new buffer.
 * @param init A size_t for an empty buffer, or strview_t of initial content.
 * @return A pointer to the newly created buffer.
 * @note Arguments may be views within the destination.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create(0);
 * strbuf_t* my_buf = strbuf_create(cstr("Hello"),NULL);
 * @endcode
 **********************************************************************************/ 
	#define strbuf_create(init) _Generic((init),\
		size_t:			strbuf_create_empty,\
		int:			strbuf_create_empty,\
		strview_t:		strbuf_create_init\
		)(init)

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
		char cstr[];					///< Beginning of the buffers contents.
	} strbuf_t;


/**
 * @def strbuf_append(strbuf_t** buf_ptr, str);
 * @brief (macro) Append to a buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param str A C string or a view of the data to be appended.
 * @return A view of the buffer contents.
 * @note The source view may be of data within the destination buffer.
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
 * @brief Create a new empty buffer.
 * @param initial_capacity The initial capacity of the buffer. This must be <= INT_MAX. It may  be 0.
 * @return A pointer to the newly created buffer.
 * @note Using the default allocator (malloc/free) requires building with -DSTRBUF_DEFAULT_ALLOCATOR_STDLIB
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create_empty(0,NULL);
 * @endcode
  **********************************************************************************/
	strbuf_t* strbuf_create_empty(size_t initial_capacity);

/**
 * @brief Create a new buffer initialised by a strview_t
 * @param initial_content The initial content of the buffer.
 * @return A pointer to the newly created buffer.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create_init(0);
 * @endcode
  **********************************************************************************/
	strbuf_t* strbuf_create_init(strview_t initial_content);

/**
 * @brief Concatenate one or more string views (strview_t) and assign the result to the buffer.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param n_args The number of strview_t in the variable argument list.
 * @param ... One or more strview_t to be concatenated.
 * @return A view of the resulting buffer contents.
 * @note This function should be used via the macro strbuf_cat(strbuf_t** buf_ptr, ...) which counts the argument list for you to provide n_args.
 * @note Arguments may be views within the destination.
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
 **********************************************************************************/
	strview_t strbuf_shrink(strbuf_t** buf_ptr);

/**
 * @brief Increase the capacity of the buffer to be at least the size specified.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param min_size The buffer capacity required.
 * @return A view of the buffer contents, or STRVIEW_INVALID if the operation fails.
 * @note This can only increase the buffers capacity, to reduce it use strbuf_shrink().
 **********************************************************************************/
	strview_t strbuf_grow(strbuf_t** buf_ptr, int min_size);

/**
 * @brief Free memory allcoated to hold the buffer and it's contents.
 * @param buf_ptr The address of a pointer to the buffer. This pointer will be NULL after the operation.
 **********************************************************************************/
	void strbuf_destroy(strbuf_t** buf_ptr);

/**
 * @brief Remove metadata from strbuf and reallocate as a naked 0 terminated c string.
 * @param buf_ptr The address of a pointer to the buffer. This pointer will be NULL after the operation.
 * @return A memory allocation containing a regular c string.
 * @note Used for applications where an interface expects a regular heap allocated c string.
 * @note Care should be taken to free the returned string with the same allocator that was used to create the buffer.
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

/**
 * @brief Insert a zero terminator at the end of each view.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param count The number of views to modify.
 * @param views A pointer to an array of views.
 * @return A view of the buffer contents, or NULL if the operation failed.
 * @note The buffer contents will become a concatenation of each view followed by a 0 terminator.
 * @note Invalid views will be excluded from the output.
 **********************************************************************************/
	strview_t strbuf_terminate_views(strbuf_t** buf_ptr, int count, strview_t src[count]);

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
