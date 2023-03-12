/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <stdbool.h>

	#include "strbuf.h"
	#include "strview.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define INITIAL_BUF_CAPACITY 16

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{

	strbuf_allocator_t str_allocator = {.allocator = allocator};
	strbuf_t* buf;

	DBG("Creating buffer with initial capacity of %i", INITIAL_BUF_CAPACITY);
	buf = strbuf_create(INITIAL_BUF_CAPACITY, &str_allocator);

	strbuf_append(&buf, cstr("This "));
	strbuf_append(&buf, cstr("buffer "));
	strbuf_append(&buf, cstr("lives "));
	strbuf_append(&buf, cstr("on "));
	strbuf_append(&buf, cstr("the "));
	strbuf_append(&buf, cstr("heap. "));

	// This can only be done with dynamic allocation, as it creates a temporary buffer
	strbuf_cat(&buf, cstr("Let's say this twice {"), strbuf_view(&buf), strbuf_view(&buf), cstr("}"));

	DBG("%s", buf->cstr);

	strbuf_destroy(&buf);

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)this_allocator; (void)caller_filename; (void)caller_line;
	void* result;
	result = realloc(ptr_to_free, size);
	assert(size==0 || result);	// You need to catch a failed allocation here.
	return result;
}
