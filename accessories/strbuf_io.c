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

//	When appending a file to a buffer, start with a buffer of at least this size
//	This must be >0 
	#define MIN_STARTING_SIZE	4096

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static int shim_read_fd(void *ctx, char *buf, int count);
	static int shim_write_fd(void *ctx, const char *buf, int count);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int strbuf_append_read(strbuf_t** buf_ptr, int fd)
{
	return strbuf_stream_in((int*)&fd, shim_read_fd, &fd);
}

int strbuf_write(int fd, strbuf_t** buf_ptr)
{
	return strbuf_stream_out((int*)&fd, shim_write_fd, &fd);
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
		failed = !strview_is_valid(strbuf_grow(dst, MIN_STARTING_SIZE));

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
	{
		strbuf_assign(dst, cstr(""));
		retval = STRVIEW_INVALID;
	};

	return retval;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static int shim_read_fd(void *ctx, char *buf, int count)
{
	int fd = *(int*)ctx;
	return read(fd, buf, count);
}

static int shim_write_fd(void *ctx, const char *buf, int count)
{
	int fd = *(int*)ctx;
	return write(fd, buf, count);
}
