/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>
	#include <math.h>

	#include "strbuf.h"
	#include "str.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define STATIC_BUFFER_SIZE	200

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	#define TEST_STR_TO_FLOAT(fmt, arg)															\
	do{																							\
		str_t str = cstr(#arg);																	\
		double desired = arg;																	\
		double result = str_to_float(str);														\
		DBG("\"%"PRIstr"\" returns "fmt, PRIstrarg(str), result);								\
		assert(desired-fabs(desired*.001) < result && result < desired+fabs(desired*.001));		\
	}while(0);


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

	static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	#define INITIAL_BUF_CAPACITY 16

	strbuf_allocator_t strbuf_allocator = {.allocator = allocator};
	strbuf_t* buf;
	str_t str1, str2;
	str_search_result_t search_result;
	const char* chrptr;
	unsigned long long tmpull;
	long long tmpll;

	printf("\n\n");
	DBG("Creating buffer with initial capacity of %i", INITIAL_BUF_CAPACITY);
	buf = strbuf_create(INITIAL_BUF_CAPACITY, strbuf_allocator);

	assert(buf->cstr);
	assert(buf->size == 0);
	assert(buf->capacity == INITIAL_BUF_CAPACITY);
	assert(buf->cstr[0] == 0);

	DBG("Concatenating AAAAAAAAAA BBBBBBBBBB CCCCCCCCCC");
	strbuf_cat(&buf, cstr("AAAAAAAAAA"), cstr("BBBBBBBBBB"), cstr("CCCCCCCCCC"));
	assert(buf->capacity >= 30);
	assert(buf->size == 30);
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC"));

	DBG("Appending -AFTER to existing buffer");
	strbuf_cat(&buf, strbuf_str(&buf), cstr("-AFTER"));
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	DBG("Prepending BEFORE- to existing buffer");
	strbuf_cat(&buf, cstr("BEFORE-"), strbuf_str(&buf));
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	DBG("Extracting BBBBBBBBBB from center of string");
	str1 = strbuf_str(&buf);
	str1 = str_sub(str1, 17, 27);
	DBG("Result = %"PRIstr"\n", PRIstrarg(str1));
	assert(str1.size == 10);
	assert(!memcmp("BBBBBBBBBB", str1.data, str1.size));

	DBG("Extracting BEFORE from start of string");
	str1 = strbuf_str(&buf);
	str1 = str_sub(str1, 0, 6);
	DBG("Result = %"PRIstr"\n", PRIstrarg(str1));
	assert(str1.size == 6);
	assert(!memcmp("BEFORE", str1.data, str1.size));

	DBG("Extracting AFTER from end of string");
	str1 = strbuf_str(&buf);
	str1 = str_sub(str1, -5, INT_MAX);
	DBG("Result = %"PRIstr"\n\n\n", PRIstrarg(str1));
	assert(str1.size == 5);
	assert(!memcmp(str1.data, "AFTER", str1.size));

	DBG("**Testing str_pop_first_split()**\n");

	strbuf_cat(&buf, cstr("123/456/789"));
	str2 = strbuf_str(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("123", str1.data, str1.size));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("456", str1.data, str1.size));
	assert(!memcmp("789", str2.data, str2.size));

	str1 = strbuf_str(&buf);
	DBG("Meanwhile, the buffer remains unchanged! %"PRIstr"\n\n\n", PRIstrarg(str1));

	DBG("**Testing str_pop_first_split_nocase()**\n");

	strbuf_cat(&buf, cstr("123r456R789"));
	str2 = strbuf_str(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = str_pop_first_split_nocase(&str2, cstr("r"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("123", str1.data, str1.size));
	str1 = str_pop_first_split_nocase(&str2, cstr("r"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("456", str1.data, str1.size));
	assert(!memcmp("789", str2.data, str2.size));

	DBG("**Testing edge cases for str_pop_first_split()**\n");

	strbuf_cat(&buf, cstr("/456/789/"));
	str2 = strbuf_str(&buf);

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

	strbuf_cat(&buf, cstr("no-delimiters"));
	str2 = strbuf_str(&buf);
	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = str_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.data == NULL);
	assert(str1.size == 0);
	assert(str2.data);
	assert(str2.size == sizeof("no-delimiters")-1);

	DBG("**Testing str_pop_last_split()**\n");

	strbuf_cat(&buf, cstr("123/456/789"));
	str2 = strbuf_str(&buf);
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

	strbuf_cat(&buf, cstr("/456/789/"));
	str2 = strbuf_str(&buf);

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
	DBG("Looking for (invalid) in \"%"PRIstr"\"", PRIstrarg(str1));
	search_result = str_find_first(str1, str2);
	assert(!search_result.found);
	DBG("Not Found (a needle which is an invalid string should not be found)\n");

	str1.data = NULL;
	str1.size = 0;
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in (invalid)", PRIstrarg(str2));
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
	DBG("Looking for (invalid) in \"%"PRIstr"\"",  PRIstrarg(str1));
	search_result = str_find_last(str1, str2);
	assert(!search_result.found);
	DBG("Not Found (a needle which is an invalid string should not be found)\n");

	str1.data = NULL;
	str1.size = 0;
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in (invalid)", PRIstrarg(str2));
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

	DBG("** Testing strbuf_append_char() **");
	strbuf_cat(&buf, cstr(""));
	strbuf_shrink(&buf);

	chrptr = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!";
	while(*chrptr)
		strbuf_append_char(&buf, *chrptr++);

	str1 = strbuf_str(&buf);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!", str1.data, str1.size));

	DBG("Current capacity = %zu - Shrinking buffer", buf->capacity);
	strbuf_shrink(&buf);
	DBG("Current capacity = %zu\n", buf->capacity);
	assert(buf->capacity == strlen(buf->cstr));

	DBG("** Testing strbuf_cat() with a single invalid str **");
	DBG("strbuf_cat() should always return a valid string");
	str1.data = NULL;
	str1.size = 0;
	str1 = strbuf_cat(&buf, str1);
	DBG("Size = %zu\n\n\n", str1.size);
	assert(str1.size == 0);
	assert(str1.data);

	DBG("** Testing str_is_match() **");
	strbuf_cat(&buf, cstr("Hello"));
	assert(str_is_match(strbuf_str(&buf), cstr("Hello")));
	str1 = cstr_SL("Test");
	str2 = cstr("Test");
	assert(str_is_match(str1, str2));
	DBG("OK\n\n\n");

	DBG("** Testing str_is_match_nocase() **");
	assert(str_is_match_nocase(cstr("hEllO"), cstr("Hello")));
	assert(!str_is_match_nocase(cstr("hEllO"), cstr("Hell")));
	DBG("OK\n\n\n");

	DBG("** Testing str_trim() **");
	str1 = cstr("/.;/hello;./;.");
	str2 = cstr("./;");
	DBG("Trimming \"%"PRIstr"\" from \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	str1 = str_trim(str1, str2);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("hello", str1.data, str1.size));

	str1 = cstr(" 123");
	str2 = cstr(" ");
	DBG("Trimming \"%"PRIstr"\" from \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	str1 = str_trim(str1, str2);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("123", str1.data, str1.size));

	DBG("** Destroying the buffer **\n\n\n");
	strbuf_destroy(&buf);
	assert(buf == NULL);

	DBG("** Testing with a fixed capacity buffer (non-dynamic) from a given memory address");
	DBG("** This can be done, but if strbuf_cat() is passed a str_t referencing data within the target buffer, it will fail and return an empty buffer.");
	DBG("** This is because it will be unable to allocate a temporary buffer to build the output\n");

	assert(!strbuf_create_fixed(static_buf+3, STATIC_BUFFER_SIZE));	//fail due to badly aligned address
	assert(!strbuf_create_fixed(static_buf, sizeof(strbuf_t)));		//fail due to insufficient space

	DBG("Creating strbuf_t from a fixed buffer of %i bytes", STATIC_BUFFER_SIZE);
	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);
	DBG("Available capacity is %zu characters (size of strbuf_t = %zu)", buf->capacity, sizeof(strbuf_t));
	assert(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t)-1);

	DBG("Concatenating DDDDDDDDDD EEEEEEEEEE FFFFFFFFFF");
	strbuf_cat(&buf, cstr("DDDDDDDDDD"), cstr("EEEEEEEEEE"), cstr("FFFFFFFFFF"));
	assert(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t)-1);
	strbuf_shrink(&buf);
	assert(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t)-1);	//strbuf_shrink should do nothing to a non-dynamic buffer
	assert(buf->size == 30);
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "DDDDDDDDDDEEEEEEEEEEFFFFFFFFFF"));

	DBG("** Testing strbuf_append_char() **");
	strbuf_cat(&buf, cstr(""));

	chrptr = "Once upon a time there was a very smelly camel that poked it's tongue out and puffed it up.";
	while(*chrptr)
		strbuf_append_char(&buf, *chrptr++);

	str1 = strbuf_str(&buf);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("Once upon a time there was a very smelly camel that poked it's tongue out and puffed it up.", str1.data, str1.size));

	DBG("Trying to pass data from the destination buffer into strbuf_cat() without a dynamic buffer (should fail and return empty buffer)");
	str1 = strbuf_str(&buf);
	str1 = str_sub(str1, 5, 10);
	strbuf_cat(&buf, cstr("never "), str1, cstr(" seen"));
	DBG("Result = %s\n", buf->cstr);
	assert(buf->size == 0);
	DBG("Trying append too much data to the buffer (should fail and return empty buffer)");
	strbuf_cat(&buf, cstr("123"));
	DBG("Result = %s", buf->cstr);
	strbuf_append(&buf, cstr("BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH  "));
	DBG("Result = %s", buf->cstr);
	strbuf_append(&buf, cstr("BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH  "));
	DBG("Result = %s\n\n", buf->cstr);
	assert(buf->size == 0);


	DBG("** Destroying the buffer (strbuf_destroy)**\n");
	strbuf_destroy(&buf);
	assert(buf == NULL);


	DBG("** Testing various edge cases of str_to_cstr() **\n");
	static_buf[0] = 0x7F;

	str_to_cstr(static_buf, 0, cstr_SL("string"));
	assert(static_buf[0] == 0x7F);

	str_to_cstr(static_buf, 1, cstr_SL("string"));
	assert(static_buf[0] == 0);

	str_to_cstr(static_buf, 4, cstr_SL("string"));
	assert(!memcmp(static_buf, "str", 4));

	str_to_cstr(static_buf, 4, cstr_SL("str"));
	assert(!memcmp(static_buf, "str", 4));

	str_to_cstr(static_buf, 100, cstr_SL("string"));
	assert(!memcmp(static_buf, "string", sizeof("string")));

	DBG("** OK **\n\n\n");

	DBG("** Testing edge cases for str_sub() **\n");

	str1 = str_sub(cstr("X"), 0, 0);	//return a valid string of length 0 within the buffer space
	assert(str_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = str_sub(cstr("X"), -1, -1);	//return a valid string of length 0 within the buffer space
	assert(str_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = str_sub(cstr("X"), 0, -1);	//return a valid string of length 0 within the buffer space
	assert(str_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = str_sub(cstr("X"), -1, 0);	//return a valid string of length 0 within the buffer space
	assert(str_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = str_sub(cstr("X"), 1, 0);	//return an invalid string if end is before beginning
	assert(!str_is_valid(str1));
	assert(str1.size == 0);

	str1 = str_sub(cstr("X"), 1, -1);	//return an invalid string if end is before beginning
	assert(!str_is_valid(str1));
	assert(str1.size == 0);

	str1 = str_sub(cstr(""), 0, 0);		//return a valid string of length 0
	assert(str_is_valid(str1));
	assert(str1.size == 0);

	str1 = str_sub(cstr(""), -1, -1);		//return a valid string of length 0
	assert(str_is_valid(str1));
	assert(str1.size == 0);

	str1 = str_sub(cstr(""), -1, 0);		//return a valid string of length 0
	assert(str_is_valid(str1));
	assert(str1.size == 0);

	str1 = str_sub(cstr(""), 0, -1);		//return a valid string of length 0
	assert(str_is_valid(str1));
	assert(str1.size == 0);

	DBG("** OK **\n\n\n");

	DBG("** Testing number conversions **\n");

	tmpll = str_to_ll(cstr("  -289765138"));
	assert(tmpll == -289765138);

	tmpll = str_to_ll(cstr("-289765138  "));
	assert(tmpll == -289765138);

	tmpll = str_to_ll(cstr("  289765138"));
	assert(tmpll == 289765138);

	tmpll = str_to_ll(cstr("289765138  "));
	assert(tmpll == 289765138);

	tmpull = str_to_ull(cstr("0xFf715cC  "));
	assert(tmpull == 0xFF715CC);

	tmpull = str_to_ull(cstr("  0XFf715cC"));
	assert(tmpull == 0xFF715CC);

	tmpull = str_to_ull(cstr("0b110011010111"));
	assert(tmpull == 0b110011010111);

	DBG("** OK **\n\n\n");

	DBG("** Testing string_compare() **\n");
	assert(str_compare(cstr("abd"), cstr("abc")) > 0);
	assert(str_compare(cstr("aba"), cstr("abd")) < 0);
	assert(str_compare(cstr("abc"), cstr("abc")) == 0);
	assert(str_compare(cstr("abca"), cstr("abc")) > 0);
	assert(str_compare(cstr("abc"), cstr("abca")) < 0);
	str1.data = NULL;
	str1.size = 0;
	assert(!str_compare(cstr(""), str1));	//an empty string == an invalid string for str_compare();

	DBG("** OK **\n\n\n");

	DBG("** Testing str_to_float() **\n");

	str1 = cstr("inf");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), str_to_float(str1));
	assert(str_to_float(str1) == INFINITY);

	str1 = cstr("-inf");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), str_to_float(str1));
	assert(str_to_float(str1) == -INFINITY);

	str1 = cstr("nan");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), str_to_float(str1));
	assert(str_to_float(str1) != str_to_float(str1));

	TEST_STR_TO_FLOAT("%f", 183.4179);
	TEST_STR_TO_FLOAT("%f", -183.4179);
	TEST_STR_TO_FLOAT("%e", 1000000);
	TEST_STR_TO_FLOAT("%e", 148.913E-23);
	TEST_STR_TO_FLOAT("%f", .002);
	TEST_STR_TO_FLOAT("%f", -.002);
	TEST_STR_TO_FLOAT("%.9f", .100000002);
	TEST_STR_TO_FLOAT("%.9f", 39E-8);

	DBG("\n\n");
	DBG("** Testing str_pop_split() **\n");
	str1 = cstr("123");
	str2 = str_pop_split(&str1, 1);
	assert(str1.size == 2);
	assert(str2.size == 1);
	assert(str_is_valid(str1));
	assert(str_is_valid(str2));
	assert(!memcmp(str1.data, "23", str1.size));
	assert(!memcmp(str2.data, "1", str2.size));

	str1 = cstr("123");
	str2 = str_pop_split(&str1, -1);
	assert(str1.size == 2);
	assert(str2.size == 1);
	assert(str_is_valid(str1));
	assert(str_is_valid(str2));
	assert(!memcmp(str1.data, "12", str1.size));
	assert(!memcmp(str2.data, "3", str2.size));

	str1 = cstr("123");
	str2 = str_pop_split(&str1, 0);
	assert(str1.size == 3);
	assert(str2.size == 0);
	assert(str_is_valid(str1));
	assert(str_is_valid(str2));
	assert(!memcmp(str1.data, "123", str1.size));
	assert(!memcmp(str2.data, "", str2.size));

	str1 = cstr("123");
	str2 = str_pop_split(&str1, -3);
	assert(str1.size == 0);
	assert(str2.size == 3);
	assert(str_is_valid(str1));
	assert(str_is_valid(str2));
	assert(!memcmp(str1.data, "", str1.size));
	assert(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = str_pop_split(&str1, 3);
	assert(str1.size == 0);
	assert(str2.size == 3);
	assert(str_is_valid(str1));
	assert(str_is_valid(str2));
	assert(!memcmp(str1.data, "", str1.size));
	assert(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = str_pop_split(&str1, 4);	//out of range
	assert(str1.size == 3);
	assert(str_is_valid(str1));
	assert(!str_is_valid(str2));
	assert(!memcmp(str1.data, "123", str1.size));

	str1 = cstr("123");
	str2 = str_pop_split(&str1, -4);	//out of range
	assert(str1.size == 3);
	assert(str_is_valid(str1));
	assert(!str_is_valid(str2));
	assert(!memcmp(str1.data, "123", str1.size));

	#ifdef STRBUF_PROVIDE_PRINTF
	DBG("\n\n");
	DBG("** Testing strbuf_printf() **\n");

	buf = strbuf_create(INITIAL_BUF_CAPACITY, strbuf_allocator);
	str1 = strbuf_printf(&buf, "Hello from printf! have some numbers... %i %i %i %i %i %i %i", 6246456, 3466765, 435234, 4598756, 94572, 69, 42597);

	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from printf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597", str1.size));
	assert(strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place

	DBG("** Testing strbuf_append_printf() **\n");

	str1 = strbuf_append_printf(&buf, " Appending one more number %i", 748921);
	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from printf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597 Appending one more number 748921", str1.size));
	assert(strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place

	#endif

	#ifdef STRBUF_PROVIDE_PRNF
	DBG("\n\n");
	DBG("** Testing strbuf_prnf() **\n");

	buf = strbuf_create(INITIAL_BUF_CAPACITY, strbuf_allocator);
	str1 = strbuf_prnf(&buf, "Hello from prnf! have some numbers... %i %i %i %i %i %i %i", 6246456, 3466765, 435234, 4598756, 94572, 69, 42597);

	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from prnf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597", str1.size));
	assert(strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place
	#endif

	DBG("** Testing strbuf_append_prnf() **\n");

	str1 = strbuf_append_prnf(&buf, " Appending one more number %i", 748921);

	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from prnf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597 Appending one more number 748921", str1.size));
	assert(strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place

	DBG("\n\n\n*** Everything worked ***\n");

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
