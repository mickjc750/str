/**
 * @file strbuf_io.h
 * @brief An additional layer to strbuf.h to provide read/write to/from a linux file descriptor.
 * @author Michael Clift
 * 
 */

#ifndef _STRBUF_STREAM_H_
	#define _STRBUF_STREAM_H_

	#include "strbuf.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Append to a buffer, attempting to fill the remaining space using read_fptr().
 * @param buf_ptr The address of a pointer to the buffer.
 * @param read_fptr A pointer to a function that will be called to read data into the buffer, the function should return the number of bytes read, or -1.
 * @param ctx The context to be passed to read_fptr.
 * @return The return value of read_fptr()
 * @note read_fptr() will always be called even if remaining space in the buffer is 0.
 * @note Does not increase the buffers capacity. Use strbuf_grow() to suitably size the buffer first.
   **********************************************************************************/
	int strbuf_fill_from(strbuf_t **buf_ptr, int (*read_fptr)(void *ctx, char *buf, int count), void *ctx);

/**
 * @brief Attempt to write the contents of the buffer using write_fptr() and remove the number of bytes written.
 * @param buf_ptr The address of a pointer to the buffer.
 * @param write_fptr A pointer to a function that will be called to write data from the buffer, the function should return the number of bytes written, or -1.
 * @param ctx The context to be passed to write_fptr.
 * @return The return value of write_fptr()
 * @note write_fptr() will always be called even if the buffer is empty.
   **********************************************************************************/
	int strbuf_empty_to(strbuf_t **buf_ptr, int (*write_fptr)(const void *ctx, const char *buf, int count), void *ctx);


#endif
