/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>

	#include "str.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define STATIC_BUFFER_SIZE	1000

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	struct static_buf_struct
	{
		void* address;
		size_t size;
		bool already_allocated;
	};

//********************************************************************************************************
// Public variables
//********************************************************************************************************


//********************************************************************************************************
// Private variables
//********************************************************************************************************

	static char static_buf[STATIC_BUFFER_SIZE] __attribute__ ((aligned));
	
//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);
	static void* static_allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	#define INITIAL_BUF_CAPACITY 16

	str_allocator_t str_allocator = {.allocator = allocator};
	str_buf_t* buf;
	str_t str1, str2;
	str_search_result_t search_result;
	const char* chrptr;

	printf("\n\n");
	DBG("Creating buffer with initial capacity of %i", INITIAL_BUF_CAPACITY);
	buf = str_buf_create(INITIAL_BUF_CAPACITY, str_allocator);

	assert(buf->cstr);
	assert(buf->size == 0);
	assert(buf->capacity == INITIAL_BUF_CAPACITY);
	assert(buf->cstr[0] == 0);

	DBG("Concatenating AAAAAAAAAA BBBBBBBBBB CCCCCCCCCC");
	str_buf_cat(&buf, cstr("AAAAAAAAAA"), cstr("BBBBBBBBBB"), cstr("CCCCCCCCCC"));
	assert(buf->capacity >= 30);
	assert(buf->size == 30);
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC"));

	DBG("Appending -AFTER to existing buffer");
	str_buf_cat(&buf, str_buf_str(&buf), cstr("-AFTER"));
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	DBG("Prepending BEFORE- to existing buffer");
	str_buf_cat(&buf, cstr("BEFORE-"), str_buf_str(&buf));
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	DBG("Extracting BBBBBBBBBB from center of string");
	str1 = str_buf_str(&buf);
	str1 = str_sub(str1, 17, 27);
	DBG("Result = %"PRIstr"\n", PRIstrarg(str1));
	assert(str1.size == 10);
	assert(!memcmp("BBBBBBBBBB", str1.data, str1.size));

	DBG("Extracting BEFORE from start of string");
	str1 = str_buf_str(&buf);
	str1 = str_sub(str1, 0, 6);
	DBG("Result = %"PRIstr"\n", PRIstrarg(str1));
	assert(str1.size == 6);
	assert(!memcmp("BEFORE", str1.data, str1.size));

	DBG("Extracting AFTER from end of string");
	str1 = str_buf_str(&buf);
	str1 = str_sub(str1, -5, INT_MAX);
	DBG("Result = %"PRIstr"\n\n\n", PRIstrarg(str1));
	assert(str1.size == 5);
	assert(!memcmp(str1.data, "AFTER", str1.size));

	DBG("**Testing str_pop_first_split()**\n");

	str_buf_cat(&buf, cstr("123/456/789"));
	str2 = str_buf_str(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("123", str1.data, str1.size));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("456", str1.data, str1.size));
	assert(!memcmp("789", str2.data, str2.size));

	str1 = str_buf_str(&buf);
	DBG("Meanwhile, the buffer remains unchanged! %"PRIstr"\n\n\n", PRIstrarg(str1));

	DBG("**Testing edge cases for str_pop_first_split()**\n");

	str_buf_cat(&buf, cstr("/456/789/"));
	str2 = str_buf_str(&buf);

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data);
	assert(str2.size == 8);
	assert(!memcmp("456/789/", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("456", str1.data, str1.size));
	assert(str2.size == 4);
	assert(!memcmp("789/", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("789", str1.data, str1.size));
	assert(str2.size == 0);
	assert(str2.data);	//remainder string should always remain valid

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data == NULL);	//result string should be invalid as there were no delimiters
	assert(str2.size == 0);
	assert(str2.data);

	str_buf_cat(&buf, cstr("no-delimiters"));
	str2 = str_buf_str(&buf);
	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.data == NULL);
	assert(str1.size == 0);
	assert(str2.data);
	assert(str2.size == sizeof("no-delimiters")-1);

	DBG("**Testing str_pop_last_split()**\n");

	str_buf_cat(&buf, cstr("123/456/789"));
	str2 = str_buf_str(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = str_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("789", str1.data, str1.size));
	assert(str2.size == 7);
	assert(!memcmp("123/456", str2.data, str2.size));

	str1 = str_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("456", str1.data, str1.size));
	assert(str2.size == 3);
	assert(!memcmp("123", str2.data, str2.size));


	DBG("**Testing edge cases for str_pop_last_split()**\n");

	str_buf_cat(&buf, cstr("/456/789/"));
	str2 = str_buf_str(&buf);

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data);
	assert(str2.size == 8);
	assert(!memcmp("/456/789", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("789", str1.data, str1.size));
	assert(str2.size == 4);
	assert(!memcmp("/456", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("456", str1.data, str1.size));
	assert(str2.size == 0);
	assert(str2.data);

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data == NULL);
	assert(str2.size == 0);
	assert(str2.data);

	DBG("**Testing str_find_first()**\n");

	str1 = cstr("needle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu\n", search_result.index);
	assert(search_result.index == 0);

	str1 = cstr("needleneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu\n", search_result.index);
	assert(search_result.index == 0);

	str1 = cstr("needlneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu\n", search_result.index);
	assert(search_result.index == 5);
	
	str1 = cstr("haystack");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu (a valid needle of length 0 should be found)\n", search_result.index);
	assert(search_result.index == 0);

	str2.data = NULL;
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(!search_result.found);
	DBG("Not Found (a needle which is an invalid string should not be found)\n");

	str1.data = NULL;
	str1.size = 0;
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(!search_result.found);
	DBG("Not Found (a needle in an invalid haystack should not be found)\n");


	str1 = cstr("");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu (a valid needle of length 0 should be found in a valid haystack of length 0)\n\n\n", search_result.index);
	assert(search_result.index == 0);



	DBG("**Testing str_find_last()**\n");

	str1 = cstr("needle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_last(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu\n", search_result.index);
	assert(search_result.index == 0);

	str1 = cstr("needleneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_last(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu\n", search_result.index);
	assert(search_result.index == 6);

	str1 = cstr("needlneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_last(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu\n", search_result.index);
	assert(search_result.index == 5);
	
	str1 = cstr("haystack");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_last(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu (a valid needle of length 0 should be found at the index of the last char+1)\n", search_result.index);
	assert(search_result.index == 8);

	str2.data = NULL;
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_last(str1, str2);
	assert(!search_result.found);
	DBG("Not Found (a needle which is an invalid string should not be found)\n");

	str1.data = NULL;
	str1.size = 0;
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(!search_result.found);
	DBG("Not Found (a needle in an invalid haystack should not be found)\n");

	str1 = cstr("");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(search_result.found);
	DBG("Found at index %zu (a valid needle of length 0 should be found at the index of the last char+1)\n\n\n", search_result.index);
	assert(search_result.index == 0);

	DBG("** Testing str_is_valid() **");
	str1.data = NULL;
	assert(!str_is_valid(str1));
	str1 = cstr("valid");
	assert(str_is_valid(str1));
	DBG("str_is_valid() works\n\n");

	DBG("** Testing str_buf_append_char() **");
	str_buf_cat(&buf, cstr(""));
	str_buf_shrink(&buf);

	chrptr = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!";
	while(*chrptr)
		str_buf_append_char(&buf, *chrptr++);

	str1 = str_buf_str(&buf);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!", str1.data, str1.size));

	DBG("Current capacity = %zu - Shrinking buffer", buf->capacity);
	str_buf_shrink(&buf);
	DBG("Current capacity = %zu\n", buf->capacity);
	assert(buf->capacity == strlen(buf->cstr));

	DBG("** Testing str_buf_cat() with a single invalid str **");
	DBG("str_buf_cat() should always return a valid string");
	str1.data = NULL;
	str1.size = 0;
	str1 = str_buf_cat(&buf, str1);
	DBG("Size = %zu\n\n\n", str1.size);
	assert(str1.size == 0);
	assert(str1.data);

	DBG("** Testing str_is_match() **");
	str_buf_cat(&buf, cstr("Hello"));
	assert(str_is_match(str_buf_str(&buf), cstr("Hello")));
	str1 = cstr_SL("Test");
	str2 = cstr("Test");
	assert(str_is_match(str1, str2));
	DBG("OK\n\n\n");

	DBG("** Testing str_trim() **");
	str1 = cstr("/.;/hello;./;.");
	str2 = cstr("./;");
	DBG("Trimming \"%"PRIstr"\" from \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	str1 = str_trim(str1, str2);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("hello", str1.data, str1.size));

	DBG("** Destroying the buffer **\n\n\n");
	str_buf_destroy(&buf);
	assert(buf == NULL);

	DBG("** Testing with an allocator that only returns an address to a single static buffer");
	DBG("** This can be done, but str_buf_cat() must NOT be passed a str_t referencing data within the target buffer");
	DBG("** Because it will be unable to allocate an additional temporary buffer to build the output\n");

//	Create an allocator which simply returns the address of a static buffer (this could also be on the stack, static here meaning non-dynamic)

//	Information used mostly for error trapping
	struct static_buf_struct static_buf_info = (struct static_buf_struct){.address = static_buf, .size = sizeof(static_buf)};

//	Assign the allocator, if no error detection is desired, the .app_data could simply be the buffer address, and the static_buf_struct above eliminated.
	str_allocator_t str_static_allocator = (str_allocator_t){.allocator = static_allocator, .app_data = &static_buf_info};

	buf = str_buf_create(INITIAL_BUF_CAPACITY, str_static_allocator);

	DBG("Concatenating DDDDDDDDDD EEEEEEEEEE FFFFFFFFFF");
	str_buf_cat(&buf, cstr("DDDDDDDDDD"), cstr("EEEEEEEEEE"), cstr("FFFFFFFFFF"));
	assert(buf->capacity >= 30);
	assert(buf->size == 30);
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "DDDDDDDDDDEEEEEEEEEEFFFFFFFFFF"));

	DBG("** Testing str_buf_append_char() **");
	str_buf_cat(&buf, cstr(""));
	str_buf_shrink(&buf);

	chrptr = "Once upon a time there was a very smelly camel that poked it's tongue out and puffed it up.";
	while(*chrptr)
		str_buf_append_char(&buf, *chrptr++);

	str1 = str_buf_str(&buf);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("Once upon a time there was a very smelly camel that poked it's tongue out and puffed it up.", str1.data, str1.size));

	DBG("Current capacity = %zu - Shrinking buffer", buf->capacity);
	str_buf_shrink(&buf);
	DBG("Current capacity = %zu\n", buf->capacity);
	assert(buf->capacity == strlen(buf->cstr));

	DBG("** Destroying the buffer (str_buf_destroy)**\n");
	str_buf_destroy(&buf);
	assert(buf == NULL);

	DBG("** Now we can create the buffer again (str_buf_create)**\n\n\n");
	buf = str_buf_create(INITIAL_BUF_CAPACITY, str_static_allocator);
	assert(buf);	

	DBG("** Complete **\n");

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)this_allocator; (void)caller_filename; (void)caller_line;
	void* result;
	result = realloc(ptr_to_free, size);
	assert(size==0 || result);	// You need to catch a failed allocation here.
	return result;
}

static void* static_allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)caller_filename; (void)caller_line;
	struct static_buf_struct* info = this_allocator->app_data;
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
