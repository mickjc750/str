/*
*/
	#include <limits.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include "strbuf_io.h"
	#include "strbuf.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int strbuf_append_read(strbuf_t** buf_ptr, int fd)
{
	int retval = 0;
	strbuf_t *buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;

		retval = read(fd, &buf->cstr[buf->size], buf->capacity - buf->size);
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

strview_t strbuf_append_file(strbuf_t **dst, const char* file_name)
{
	int fd = -1;
	int err;
	bool failed;
	bool eof = false;
	int resize;
	strview_t retval = STRVIEW_INVALID;

	failed = (dst == NULL || *dst == NULL);

	if(!failed)
	{
		fd = open(file_name, O_RDONLY);
		failed = (fd == -1);
	};

	while(!failed && !eof)
	{
		err = strbuf_append_read(dst, fd);
		failed = (err == -1);
		eof = (err == 0);
		if(!failed && !eof)
		{
			resize = (*dst)->capacity;
			if(resize < (INT_MAX-2)/2)
				resize *= 2;
			else
				resize = INT_MAX-1;
			failed = !(resize > (*dst)->capacity);
		};
		if(!failed && !eof)
			failed = !strview_is_valid(strbuf_grow(dst, resize));
	};
	
	if(fd != -1)
		close(fd);

	if(eof)
		retval = strbuf_shrink(dst);
	else
		retval = strbuf_assign(dst, cstr(""));

	return retval;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

