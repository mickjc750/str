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

	#define BUFFER_CAPACITY 200

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	// A structure which holds information about the buffer
	typedef struct buf_info_t
	{
		void* address;
		size_t size;
		bool already_allocated;
	} buf_info_t;

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	// Declare a buffer. Alignment is required, as this will also hold the str_buf_t.
	// The size must allow for a str_buf_t + the capacity of the buffer (in chars) + 1 for the maintained null terminator
	char stackbuf[BUFFER_CAPACITY + sizeof(strbuf_t) + 1] __attribute__ ((aligned));

	buf_info_t buf_info = {.address = stackbuf, .size = sizeof(stackbuf)};
	str_allocator_t str_allocator = {.allocator = allocator, .app_data = &buf_info};
	strbuf_t* buf;
	size_t index;

	DBG("Creating buffer with capacity of %i", BUFFER_CAPACITY);
	buf = strbuf_create(BUFFER_CAPACITY, str_allocator);

	strbuf_append(&buf, cstr("buffer "));
	strbuf_append(&buf, cstr("lives "));
	strbuf_append(&buf, cstr("the "));
	strbuf_append(&buf, cstr("stack "));
	strbuf_prepend(&buf, cstr("This "));

	index = str_find_first(strbuf_str(&buf), cstr("the")).index;
	strbuf_insert(&buf, index, cstr("on "));

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

// An allocator function for returning the address of a single buffer.
static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)caller_filename; (void)caller_line;
	buf_info_t* info = this_allocator->app_data;
	intptr_t alignment_mask;

	if(size)
	{
		//check alignment
		alignment_mask = sizeof(void*)-1;
		alignment_mask &= (intptr_t)info->address;
		assert(alignment_mask == 0);

		//check size
		assert(size <= info->size);

		//check we are not trying to make a new allocation (re-allocation is ok, a new allocation is not)
		assert(ptr_to_free || !info->already_allocated);
		info->already_allocated = true;
	}
	else
		info->already_allocated = false;

	return info->address;
}
