/*
*/

	#include <stdlib.h>
	#define strbuf_alloc(sz)		malloc(sz)
	#define strbuf_realloc(ptr,sz)	realloc(ptr,sz)
	#define strbuf_free(ptr)		free(ptr)

	#define STRBUF_IMPLEMENTATION
	#include "strbuf.h"
