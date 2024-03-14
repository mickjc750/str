/**
 * @file strview_io.h
 * @brief An additional layer to strview.h to provide writing to a linux file descriptor.
 * @author Michael Clift
 * 
 */

#ifndef _STRVIEW_IO_H_
	#define _STRVIEW_IO_H_

	#include "strview.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Attempt to write the contents of a view using a POSIX write() and remove the number of bytes written from the view.
 * @param buf_ptr The address of the view.
 * @return The number of bytes written, or -1 for error with errno set.
 * @note The return value is that returned by write().
 * @note write() will be called even if the view is empty.
 * @note write will NOT be called if the view is invalid.
   **********************************************************************************/
	int strview_write(int fd, strview_t* src);

#endif
