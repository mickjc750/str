/**
 * @file strbuf_io.h
 * @brief An additional layer to strbuf.h to provide read/write to/from a linux file descriptor.
 * @author Michael Clift
 * 
 */

#ifndef _STRBUF_IO_H_
	#define _STRBUF_IO_H_

	#include "strbuf.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Append to a buffer, attempting to fill the remaining space using a POSIX read().
 * @param buf_ptr The address of a pointer to the buffer.
 * @return The number of bytes appended. 0 for file EOF or buffer full on entry, or -1 for error with errno set.
 * @note The return value is that returned by read(), read will always be called even if remaining space in the buffer is 0.
 * @note Does not increase the buffers capacity. Use strbuf_grow() to suitably size the buffer first.
   **********************************************************************************/
	int strbuf_append_read(strbuf_t **buf_ptr, int fd);

/**
 * @brief Attempt to write the contents of the buffer using a POSIX write() and remove the number of bytes written.
 * @param buf_ptr The address of a pointer to the buffer.
 * @return The number of bytes written, or -1 for error with errno set.
 * @note The return value is that returned by write(), write() will always be called even if the buffer is empty.
   **********************************************************************************/
	int strbuf_write(int fd, strbuf_t **buf_ptr);

/**
 * @brief Append to a buffer from a file on disk.
 * @param buf_ptr The address of a pointer to the target buffer.
 * @return A view of the resulting buffer contents, or STRVIEW_INVALID if the operation failed.
 * @note The buffer will be resized up to a maximum of INT_MAX-1 to allow the entire file to be appended.
   **********************************************************************************/
	strview_t strbuf_append_file(strbuf_t **buf_ptr, const char* file_name);

#endif
