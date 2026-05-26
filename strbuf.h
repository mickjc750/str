/**
 * @file strbuf.h
 * @brief A buffer API complementing strview.h
 * @author Michael Clift
 * 
 * * Provides functions for allocating dynamic buffers.
 * * Provides functions for building and modifying string data.
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
 * 
 * 
 * ## Build options
 *  -DSTRBUF_PROVIDE_PRINTF
 * 	Provides functions which use vsnprintf() internally to assign or append formatted text to a buffer.
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
 * strbuf_cat(&my_buf, cstr("Hello"), cstr(" World"));
 * @endcode
 **********************************************************************************/ 
 	#define strbuf_cat(buf_ptr, ...) strbuf_cat_n(buf_ptr, PP_NARG(__VA_ARGS__), __VA_ARGS__)


/**
 * @def strbuf_create(init)
 * @brief (macro) Create a new buffer.
 * @param init A size_t for an empty buffer, or strview_t of initial content.
 * @return A pointer to the newly created buffer.
 * @note Arguments may be views within the destination.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create(0);
 * strbuf_t* my_buf = strbuf_create(cstr("Hello"));
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
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create_empty(0);
 * @endcode
  **********************************************************************************/
	strbuf_t* strbuf_create_empty(size_t initial_capacity);

/**
 * @brief Create a new buffer initialised by a strview_t
 * @param initial_content The initial content of the buffer.
 * @return A pointer to the newly created buffer.
 * @note Example:
 * @code{.c}
 * strbuf_t* my_buf = strbuf_create_init(cstr("Hello"));
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
	strview_t strbuf_cat_n(strbuf_t** buf_ptr, int n_args, ...);

/**	
 * 	@brief	The non-variadic version of strbuf_cat_n
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
 * strbuf_t* my_buf = strbuf_create(0);
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
 * @brief Reduce buffer capacity to the minimum size required to hold its contents.
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
 * @brief Free memory allocated to hold the buffer and its contents.
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

/**
 * @brief Append to a buffer, attempting to fill the remaining space using read_fptr().
 * @param buf_ptr The address of a pointer to the buffer.
 * @param read_fptr A pointer to a function that will be called to read data into the buffer, the function should return the number of bytes read, or -1.
 * @param ctx The context to be passed to read_fptr.
 * @return The return value of read_fptr()
 * @note read_fptr() will always be called even if remaining space in the buffer is 0.
 * @note Does not increase the buffers capacity. Use strbuf_grow() to suitably size the buffer first.
   **********************************************************************************/
	int strbuf_stream_in(strbuf_t **buf_ptr, int (*read_fptr)(void *ctx, char *buf, int count), void *ctx);

/**
 * @brief Attempt to write the contents of the buffer using write_fptr() and remove the number of bytes written.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param write_fptr A pointer to a function that will be called to write data from the buffer, the function should return the number of bytes written, or -1.
 * @param ctx The context to be passed to write_fptr.
 * @return The return value of write_fptr()
 * @note write_fptr() will always be called even if the buffer is empty.
 * @note if write_fptr() only accepts part of the data, the remaining data will be moved in memory.
   **********************************************************************************/
	int strbuf_stream_out(strbuf_t **buf_ptr, int (*write_fptr)(const void *ctx, const char *buf, int count), void *ctx);

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




//*************************************************************************************************
#ifdef STRBUF_IMPLEMENTATION
//*************************************************************************************************

	#include <stdint.h>
	#include <ctype.h>
	#include <limits.h>

	#ifdef STRBUF_PROVIDE_PRINTF
		#include <stdio.h>
	#endif

	#ifdef STRBUF_PROVIDE_PRNF
		#include "prnf.h"
	#endif

	#ifdef STRBUF_CAPACITY_GROW_STEP
		#warning "Deprecated build option STRBUF_CAPACITY_GROW_STEP.\
 Buffer size now increases by 1/2^(STRBUF_CAPACITY_GROW_RATIO), which defaults to 1/2^1 or a 50% increase."
	#endif

	#ifdef STRBUF_DEFAULT_ALLOCATOR_STDLIB
		#warning "Deprecated build option STRBUF_DEFAULT_ALLOCATOR_STDLIB.\
 An allocator must be provided prior to including strbuf.h with STRBUF_IMPLEMENTATION defined."
	#endif

	#ifdef STRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB
		#warning "Deprecated build option STRBUF_ASSERT_DEFAULT_ALLOCATOR_STDLIB.\
 If you wish to handle allocator failure, you must do so in the applications provided allocator."
	#endif

	#ifndef STRBUF_CAPACITY_GROW_RATIO
		#define STRBUF_CAPACITY_GROW_RATIO 1
	#endif

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//	#include <stdio.h>
//	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static strbuf_t* create_buf(int initial_capacity);
	static strview_t buffer_vcat(strbuf_t** buf_ptr, int n_args, va_list va);
	static void insert_strview_into_buf(strbuf_t** buf_ptr, int index, strview_t str);
	static void destroy_buf(strbuf_t** buf_ptr);
	static void change_buf_capacity(strbuf_t** buf_ptr, int new_capacity);
	static void assign_strview_to_buf(strbuf_t** buf_ptr, strview_t str);
	static void append_char_to_buf(strbuf_t** strbuf, char c);
	static int  round_up_capacity(int current_capacity, int capacity_needed);
	static strview_t strview_of_buf(strbuf_t* buf);
	static bool buf_contains_str(strbuf_t* buf, strview_t str);
	static void empty_buf(strbuf_t* buf);
	static bool add_will_overflow_int(int a, int b);
	static bool view_contains_char(strview_t view, char c);

#ifdef STRBUF_PROVIDE_PRNF
	static void char_handler_for_prnf(void* dst, char c);
#endif

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strbuf_t* strbuf_create_empty(size_t initial_capacity)
{
	strbuf_t* result;
	
	if(initial_capacity <= INT_MAX)
		result = create_buf((int)initial_capacity);
	else
		result = NULL;
	return result;
}

strbuf_t* strbuf_create_init(strview_t initial_content)
{
	strbuf_t* result;
	
	result = create_buf(initial_content.size);
	insert_strview_into_buf(&result, 0, initial_content);

	return result;
}

// concatenate a number of str's this can include the buffer itself, strbuf.str for appending
strview_t strbuf_cat_n(strbuf_t** buf_ptr, int n_args, ...)
{
	va_list va;
	va_start(va, n_args);
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
		str = buffer_vcat(buf_ptr, n_args, va);
	va_end(va);
	return str;
}

strview_t strbuf_vcat(strbuf_t** buf_ptr, int n_args, va_list va)
{
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
		str = buffer_vcat(buf_ptr, n_args, va);
	return str;
}

#ifdef STRBUF_PROVIDE_PRINTF
strview_t strbuf_printf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_vprintf(buf_ptr, format, va);
		va_end(va);
	};
	return str;
}

strview_t strbuf_vprintf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		empty_buf(*buf_ptr);
		str = strbuf_append_vprintf(buf_ptr, format, va);
	};
	return str;
}

strview_t strbuf_append_printf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_append_vprintf(buf_ptr, format, va);
		va_end(va);
	};
	return str;
}

	
strview_t strbuf_append_vprintf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	int size;
	int append_size;
	bool failed;
	strbuf_t* buf;
	strview_t str = STRVIEW_INVALID;
	va_list vb;
	if(buf_ptr && *buf_ptr)
	{
		va_copy(vb, va);
		buf = *buf_ptr;
		size = buf->size;
		append_size = vsnprintf(NULL, 0, format, va);

		failed = add_will_overflow_int(size, append_size);
		if(!failed)
		{
			size += append_size;
			if(size > buf->capacity)
				change_buf_capacity(&buf, round_up_capacity(buf->size, size));

			failed = size > buf->capacity;
		};

		if(!failed)
			buf->size += vsnprintf(&buf->cstr[buf->size], buf->capacity - buf->size + 1, format, vb);
		else
			empty_buf(buf);

		str = strbuf_view(&buf);
		*buf_ptr = buf;
		va_end(vb);
	};
	return str;
}
#endif

#ifdef STRBUF_PROVIDE_PRNF
strview_t strbuf_prnf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_vprnf(buf_ptr, format, va);
		va_end(va);;
	};
	return str;
}

strview_t strbuf_vprnf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	strbuf_t* buf;
	strview_t str = STRVIEW_INVALID;
	int char_count;
	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		empty_buf(buf);

		char_count = vfptrprnf(char_handler_for_prnf, &buf,  format, va);

		if(char_count > buf->size)
			empty_buf(buf);

		str = strbuf_view(&buf);
		*buf_ptr = buf;
	};
	return str;
}

strview_t strbuf_append_prnf(strbuf_t** buf_ptr, const char* format, ...)
{
	va_list va;
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		va_start(va, format);
		str = strbuf_append_vprnf(buf_ptr, format, va);
		va_end(va);;
	};
	return str;
}

strview_t strbuf_append_vprnf(strbuf_t** buf_ptr, const char* format, va_list va)
{
	strbuf_t* buf;
	strview_t str = STRVIEW_INVALID;
	int char_count;
	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;

		char_count = buf->size;
		char_count += vfptrprnf(char_handler_for_prnf, &buf,  format, va);

		if(char_count > buf->size || char_count < 0)
			empty_buf(buf);

		str = strbuf_view(&buf);
		*buf_ptr = buf;
	};
	return str;
}

#endif

strview_t strbuf_view(strbuf_t** buf_ptr)
{
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
		str = strview_of_buf(*buf_ptr);
	return str;
}

strview_t strbuf_append_char(strbuf_t** buf_ptr, char c)
{
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		append_char_to_buf(buf_ptr, c);
		str = strview_of_buf(*buf_ptr);
	};
	return str;
}

// reduce allocation size to the minimum possible
strview_t strbuf_shrink(strbuf_t** buf_ptr)
{
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		change_buf_capacity(buf_ptr, (*buf_ptr)->size);
		str = strview_of_buf(*buf_ptr);
	};
	return str;
}

// increase allocation size to support a capacity of at least min_size
strview_t strbuf_grow(strbuf_t** buf_ptr, int min_size)
{
	strview_t str = STRVIEW_INVALID;
	if(buf_ptr && *buf_ptr)
	{
		if(min_size > (*buf_ptr)->capacity)
			change_buf_capacity(buf_ptr, min_size);
		str = strview_of_buf(*buf_ptr);
	};
	return str;
}

void strbuf_destroy(strbuf_t** buf_ptr)
{
	if(buf_ptr)
	{
		if(*buf_ptr)
			destroy_buf(buf_ptr);
		*buf_ptr = NULL;
	};	
}

char* strbuf_to_cstr(strbuf_t** buf_ptr)
{
	int len;
	char* str = NULL;

	if(buf_ptr && *buf_ptr)
	{
		len = (*buf_ptr)->size;
		str = (void*)(*buf_ptr);
		memmove(str, (*buf_ptr)->cstr, len);
		str = strbuf_realloc(str, len+1);

		str[len] = 0;
		*buf_ptr = NULL;
	};
	return str;
}

strview_t strbuf_assign(strbuf_t** buf_ptr, strview_t str)
{
	strbuf_t* buf = NULL;
	bool failed;
	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		failed = !strview_is_valid(str);
		if(!failed)
		{
			if(str.size > buf->capacity)
				change_buf_capacity(&buf, round_up_capacity(buf->size, str.size));
			
			failed = str.size > buf->capacity;
		};
		if(!failed)
		{
			memmove(buf->cstr, str.data, (size_t)str.size);
			buf->size = str.size;
			buf->cstr[buf->size] = 0;
		}
		else
			empty_buf(buf);
		*buf_ptr = buf;
	};

	return strview_of_buf(buf);
}

strview_t strbuf_append_strview(strbuf_t** buf_ptr, strview_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_strview_into_buf(buf_ptr, (*buf_ptr)->size, str);
	return buf_ptr ? strview_of_buf(*buf_ptr) : STRVIEW_INVALID;
}

strview_t strbuf_append_cstr(strbuf_t** buf_ptr, const char* str)
{
	return strbuf_append_strview(buf_ptr, cstr(str));
}

strview_t strbuf_prepend_strview(strbuf_t** buf_ptr, strview_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_strview_into_buf(buf_ptr, 0, str);
	return buf_ptr ? strview_of_buf(*buf_ptr) : STRVIEW_INVALID;
}

strview_t strbuf_prepend_cstr(strbuf_t** buf_ptr, const char* str)
{
	return strbuf_prepend_strview(buf_ptr, cstr(str));
}

strview_t strbuf_insert_at_index_strview(strbuf_t** buf_ptr, int index, strview_t str)
{
	if(buf_ptr && *buf_ptr)
		insert_strview_into_buf(buf_ptr, index, str);
	return buf_ptr ? strview_of_buf(*buf_ptr) : STRVIEW_INVALID;
}

strview_t strbuf_insert_at_index_cstr(strbuf_t** buf_ptr, int index, const char* str)
{
	return strbuf_insert_at_index_strview(buf_ptr, index, cstr(str));
}

strview_t strbuf_insert_before_strview(strbuf_t** buf_ptr, strview_t dst, strview_t src)
{
	strbuf_t* buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		if(buf->cstr <= dst.data && dst.data <= &buf->cstr[buf->size])
			insert_strview_into_buf(&buf, dst.data - buf->cstr, src);
		*buf_ptr = buf;
	};

	return buf_ptr ? strview_of_buf(*buf_ptr) : STRVIEW_INVALID;
}

strview_t strbuf_insert_before_cstr(strbuf_t** buf_ptr, strview_t dst, const char* src)
{
	return strbuf_insert_before_strview(buf_ptr, dst, cstr(src));
}

strview_t strbuf_insert_after_strview(strbuf_t** buf_ptr, strview_t dst, strview_t src)
{
	strbuf_t* buf;
	const char* dst_ptr;

	if(buf_ptr && *buf_ptr && strview_is_valid(dst))
	{
		buf = *buf_ptr;
		dst_ptr = &dst.data[dst.size];

		if(buf->cstr <= dst_ptr && dst_ptr <= &buf->cstr[buf->size])
			insert_strview_into_buf(&buf, dst_ptr - buf->cstr, src);
		*buf_ptr = buf;
	};

	return buf_ptr ? strview_of_buf(*buf_ptr) : STRVIEW_INVALID;
}

strview_t strbuf_insert_after_cstr(strbuf_t** buf_ptr, strview_t dst, const char* src)
{
	return strbuf_insert_after_strview(buf_ptr, dst, cstr(src));
}

strview_t strbuf_strip_strview(strbuf_t** buf_ptr, strview_t stripchars)
{
	strbuf_t* buf;
	char* ptr;
	int count;

	if(buf_ptr && *buf_ptr && strview_is_valid(stripchars))
	{
		buf = *buf_ptr;
		count = buf->size;
		ptr = buf->cstr;
		while(count)
		{
			if(view_contains_char(stripchars, *ptr))
			{
				memmove(ptr, ptr+1, count);
				buf->size--;
			}
			else
				ptr++;
			count--;
		};
		*buf_ptr = buf;
	};

	return buf_ptr ? strview_of_buf(*buf_ptr) : STRVIEW_INVALID;
}

strview_t strbuf_strip_cstr(strbuf_t** buf_ptr, const char* stripchars)
{
	return strbuf_strip_strview(buf_ptr, cstr(stripchars));
}

strview_t strbuf_terminate_views(strbuf_t** buf_ptr, int count, strview_t src[count])
{
	bool failed;
	int i = 0;
	int size_needed = 0;
	char *dst;
	strview_t view;
	strbuf_t *old_buf;
	ptrdiff_t offset = 0;;

	failed = !(buf_ptr && *buf_ptr);

//	determine size needed, and check that all valid views are within the buffer
	if(!failed)
	{
		i = 0;
		while(i != count && !failed)
		{
			size_needed += strview_is_valid(src[i]) ? src[i].size + 1 : 0;
			failed |= !(buf_contains_str(*buf_ptr, src[i]) || !strview_is_valid(src[i]));
			i++;
		};
	};

//	resize the buffer if possible, and check that the buffer is big enough
	if(!failed)
	{
		if((*buf_ptr)->capacity < size_needed)
		{
			old_buf = *buf_ptr;
			change_buf_capacity(buf_ptr, size_needed);
			offset = (uint8_t*)*buf_ptr - (uint8_t*)old_buf;
		};
		i = 0;
		while(i != count)	// move any valid views to the new buffer
		{
			if(strview_is_valid(src[i]))
				src[i].data += offset;
			i++;
		};

		failed = ((*buf_ptr)->capacity < size_needed);
		if(failed)
			empty_buf((*buf_ptr));
	};

	if(!failed)
	{
		i = 0;
		dst = (*buf_ptr)->cstr;
		while(i != count)
		{
			if(strview_is_valid(src[i]))
			{
				if(dst < src[i].data)
				{
					memmove(dst, src[i].data, src[i].size); //<-- ASAN FAULT
					src[i].data = dst;
				};
				dst += src[i].size + 1;
			};
			i++;
		};

		while(i--)
		{
			if(strview_is_valid(src[i]))
			{
				dst -= src[i].size + 1;
				if(dst > src[i].data)
				{
					memmove(dst, src[i].data, src[i].size);
					src[i].data = dst;
				};
				((char*)(src[i].data))[src[i].size] = 0;
				src[i].size++;
			};
		};

		view = strview_of_buf(*buf_ptr);
		view.size = size_needed;
		strbuf_assign(buf_ptr, view);
	};

	return failed ? STRVIEW_INVALID : view;
}

int strbuf_stream_in(strbuf_t **buf_ptr, int (*read_fptr)(void *ctx, char *buf, int count), void *ctx)
{
	int retval;
	strbuf_t *buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;	// (no need to assign this back, as the buffer is not resized)

		retval = read_fptr(ctx, &buf->cstr[buf->size], buf->capacity - buf->size);
		if(retval > 0)
		{
			buf->size += retval;
			buf->cstr[buf->size] = 0;
		};
	}
	else
		retval = read_fptr(ctx, NULL, 0);

	return retval;
}

int strbuf_stream_out(strbuf_t **buf_ptr, int (*write_fptr)(const void *ctx, const char *buf, int count), void *ctx)
{
	int retval;
	strbuf_t *buf;
	strview_t buf_view;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;

		retval = write_fptr(ctx, buf->cstr, buf->size);
		if(retval > 0)
		{
			buf_view = strbuf_view(&buf);
			buf_view = strview_sub(buf_view, retval, INT_MAX);
			strbuf_assign(&buf, buf_view);
		};

		*buf_ptr = buf;
	}
	else
		retval = write_fptr(ctx, NULL, 0);

	return retval;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static strbuf_t* create_buf(int initial_capacity)
{
	strbuf_t* buf = NULL;

	if(initial_capacity <= INT_MAX)
	{
		buf = strbuf_alloc(sizeof(strbuf_t)+initial_capacity+1);
		buf->capacity = initial_capacity;
		empty_buf(buf);
	};

	return buf;
}

static strview_t strview_of_buf(strbuf_t* buf)
{
	strview_t str = STRVIEW_INVALID;
	if(buf)
	{
		str.data = buf->cstr;
		str.size = buf->size;
	};
	return str;
}

static strview_t buffer_vcat(strbuf_t** buf_ptr, int n_args, va_list va)
{
	strview_t 	str;
	int 	size_needed = 0;
	int 	i = 0;
	bool 	failed = false;
	strbuf_t* dst_buf = *buf_ptr;
	strbuf_t* build_buf;
	va_list vb;
	va_copy(vb, va);

	while(i++ != n_args)
	{
		str = va_arg(va, strview_t);
		failed |= add_will_overflow_int(size_needed, str.size);
		size_needed += str.size;
	};
	
	if(!failed)
	{
		build_buf = create_buf(size_needed);
		i = 0;
		while(i++ != n_args)
			insert_strview_into_buf(&build_buf, build_buf->size, va_arg(vb, strview_t));

		assign_strview_to_buf(&dst_buf, strview_of_buf(build_buf));
		destroy_buf(&build_buf);
	}
	else
		empty_buf(dst_buf);

	*buf_ptr = dst_buf;

	va_end(vb);
	return strview_of_buf(dst_buf);
}

static void insert_strview_into_buf(strbuf_t** buf_ptr, int index, strview_t str)
{
	strbuf_t* buf = *buf_ptr;
	bool src_in_dst = buf_contains_str(buf, str);
	size_t src_offset = str.data - buf->cstr;
	strview_t strview_part_left_behind = STRVIEW_INVALID;
	strview_t strview_part_shifted;
	char* move_src;
	char* move_dst;
	bool failed;

	if(index > buf->size)
		index = buf->size;
	if(index < 0)
		index += buf->size;
	if(index < 0)
		index = 0;

	failed = add_will_overflow_int(buf->size, str.size);

	if(!failed)
	{
		if(buf->capacity < buf->size + str.size)
			change_buf_capacity(&buf, round_up_capacity(buf->size, (buf->size + str.size)));

		if(src_in_dst && buf != *buf_ptr)
			str.data = buf->cstr + src_offset;

		failed = buf->capacity < buf->size + str.size;
	};

	if(!failed)
	{
		strview_part_shifted = str;
		move_src = &buf->cstr[index];
		move_dst = &buf->cstr[index+str.size];
		if(str.size)
		{
			memmove(move_dst, move_src, buf->size-index);
			if(src_in_dst)
			{
				if(move_src > str.data)
					strview_part_left_behind = strview_split_index(&strview_part_shifted, move_src - str.data);
				strview_part_shifted.data += move_dst-move_src;
			};
		};

		buf->size += str.size;
		if(strview_part_left_behind.size)
			memcpy(move_src, strview_part_left_behind.data, strview_part_left_behind.size);
		move_src += strview_part_left_behind.size;
		if(strview_part_shifted.size)
			memcpy(move_src, strview_part_shifted.data, strview_part_shifted.size);
		buf->cstr[buf->size] = 0;
	}
	else
		empty_buf(buf);

	*buf_ptr = buf;
}

static void destroy_buf(strbuf_t** buf_ptr)
{
	strbuf_t* buf = *buf_ptr;
	strbuf_free(buf);
	*buf_ptr = NULL;
}

static void change_buf_capacity(strbuf_t** buf_ptr, int new_capacity)
{
	strbuf_t* buf = *buf_ptr;

	if(new_capacity < buf->size)
		new_capacity = buf->size;

	if(new_capacity != buf->capacity)
	{
		buf = strbuf_realloc(buf, sizeof(strbuf_t)+new_capacity+1);
		buf->capacity = new_capacity;
	};

	*buf_ptr = buf;
}

static void assign_strview_to_buf(strbuf_t** buf_ptr, strview_t str)
{
	empty_buf(*buf_ptr);
	insert_strview_into_buf(buf_ptr, 0, str);
}

static void append_char_to_buf(strbuf_t** buf_ptr, char c)
{
	strbuf_t* buf = *buf_ptr;
	bool failed = add_will_overflow_int(buf->size, 1);

	if(!failed)
	{
		if(buf->size+1 > buf->capacity)
			change_buf_capacity(&buf, round_up_capacity(buf->size, buf->size + 1));
		failed = buf->capacity < buf->size+1;
	};

	if(!failed)
	{
		buf->cstr[buf->size] = c;
		buf->size++;
		buf->cstr[buf->size] = 0;
	}
	else
		empty_buf(buf);

	*buf_ptr = buf;
}

static int round_up_capacity(int current_capacity, int capacity_needed)
{
	int grow_size;
	int new_capacity = current_capacity;

	while(new_capacity < capacity_needed)
	{
		grow_size = new_capacity >> STRBUF_CAPACITY_GROW_RATIO;
		if(!grow_size)
			grow_size = 1;
		if(!add_will_overflow_int(new_capacity, grow_size))
			new_capacity += grow_size;
		else
			new_capacity = INT_MAX;
	};

	return new_capacity;
}

static bool buf_contains_str(strbuf_t* buf, strview_t str)
{
	return &buf->cstr[0] <= str.data && str.data < &buf->cstr[buf->size];
}

static void empty_buf(strbuf_t* buf)
{
	buf->size = 0;
	buf->cstr[0] = 0;
}

static bool add_will_overflow_int(int a, int b)
{
	int c = a;
	c += b;
	return ((a < 0) == (b < 0) && (a < 0) != (c < 0));
}

static bool view_contains_char(strview_t view, char c)
{
	bool retval = false;

	if(strview_is_valid(view))
	{
		while(!retval && view.size)
		{
			retval |= (*view.data == c);
			view.data++;
			view.size--;
		};
	};

	return retval;
}

#ifdef STRBUF_PROVIDE_PRNF
static void char_handler_for_prnf(void* dst, char c)
{
	append_char_to_buf((strbuf_t**)dst, c);
}
#endif
#endif