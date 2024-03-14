/*
*/
	#include <limits.h>
	#include <unistd.h>
	#include "strbuf_io.h"
	#include "strview.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int strbuf_read(strbuf_t** buf_ptr, int fd)
{
	int retval = 0;
	strbuf_t *buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;

		retval = read(fd, buf->cstr, buf->capacity - buf->size);
		if(retval > 0)
		{
			buf->size += retval;
			buf->cstr[buf->size] = 0;
		};
	};

	return retval;
}

int strbuf_write(int fd, strbuf_t** buf_ptr)
{
	int retval = 0;
	strbuf_t *buf;
	strview_t buf_view;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;

		retval = write(fd, buf->cstr, buf->size);
		if(retval > 0)
		{
			buf_view = strbuf_view(&buf);
			buf_view = strview_sub(buf_view, retval, INT_MAX);
			strbuf_assign(&buf, buf_view);
		};

		*buf_ptr = buf;
	};

	return retval;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

