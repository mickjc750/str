/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <stdbool.h>
	#include <stdint.h>

	#include "../../strbuf.h"
	#include "../../str.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define BUFFER_CAPACITY	200

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	str_allocator_t str_allocator = {.allocator = allocator};
	strbuf_t* buf;
	size_t index;

	DBG("Creating buffer with capacity of %i", BUFFER_CAPACITY);
	buf = strbuf_create(BUFFER_CAPACITY, str_allocator);

	strbuf_append(&buf, cstr("buffer "));
	strbuf_append(&buf, cstr("static "));
	strbuf_prepend(&buf, cstr("This "));

	index = str_find_first(strbuf_str(&buf), cstr("static")).index;
	strbuf_insert(&buf, index, cstr("is "));

	DBG("%s", buf->cstr);

// 	This would break without dynamic allocation, as it will try to allocate a temporary buffer
//	strbuf_cat(&buf, cstr("Let's say this twice {"), strbuf_str(&buf), strbuf_str(&buf), cstr("}"));

//	But we can still use strbuf_cat() providing the input strings are not from the output itself.
	strbuf_cat(&buf, cstr("One "), cstr("Two "), cstr("Three"));
	DBG("%s", buf->cstr);

	strbuf_destroy(&buf);	// doesn't really do anything.. except allow us to create the buffer again from the same allocator if we want to.

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

// Declare an allocator function for returning the address of a static buffer, held by the allocator itself
static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)caller_filename; (void)caller_line;

	//Size must accommodate strbuf_t + expected character capacity + maintained null terminator
	//Alignment is required, as this will also hold the str_buf_t.
	static char buf[BUFFER_CAPACITY + sizeof(strbuf_t) + 1] __attribute__ ((aligned));
	static bool already_allocated = false;

	if(size)
	{
		assert(size <= sizeof(buf));

		//check we are not trying to make a new allocation (re-allocation is ok)
		assert(ptr_to_free || !already_allocated);
		already_allocated = true;
	}
	else
		already_allocated = false;

	return buf;
}

