/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>
	#include <math.h>

	#include "strbuf.h"
	#include "strview.h"

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
		strview_t str = cstr(#arg);																	\
		double desired = arg;																	\
		double result = strview_to_float(str);														\
		DBG("\"%"PRIstr"\" returns "fmt, PRIstrarg(str), result);								\
		assert(desired-fabs(desired*.001) < result && result < desired+fabs(desired*.001));		\
	}while(0)


	#define TEST_LINE_POP(arg1)								\
	do{														\
		str2.data = NULL; str2.size = 0;					\
		while(!strview_is_valid(str2))							\
		{													\
			strbuf_append_char(&buf, *chrptr++);			\
			str1 = strbuf_view(&buf);						\
			str2 = strview_pop_line(&str1, &eol);				\
		};													\
		DBG("[%"PRIstr"]", PRIstrarg(str2));				\
		assert(!memcmp(str2.data, arg1, str1.size));		\
		strbuf_assign(&buf, str1);							\
	}while(0)

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
	(void)argc;
	(void)argv;
	#define INITIAL_BUF_CAPACITY 16

	strbuf_allocator_t strbuf_allocator = {.allocator = allocator};
	strbuf_t* buf;
	strview_t str1, str2, str3, search_result;
	const char* chrptr;
	unsigned long long tmpull;
	long long tmpll;

	printf("\n\n");
	DBG("Creating buffer with initial capacity of %i", INITIAL_BUF_CAPACITY);
	buf = strbuf_create(INITIAL_BUF_CAPACITY, &strbuf_allocator);

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
	strbuf_cat(&buf, strbuf_view(&buf), cstr("-AFTER"));
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	DBG("Prepending BEFORE- to existing buffer");
	strbuf_cat(&buf, cstr("BEFORE-"), strbuf_view(&buf));
	DBG("Result = %s\n", buf->cstr);
	assert(!strcmp(buf->cstr, "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	DBG("Extracting BBBBBBBBBB from center of string");
	str1 = strbuf_view(&buf);
	str1 = strview_sub(str1, 17, 27);
	DBG("Result = %"PRIstr"\n", PRIstrarg(str1));
	assert(str1.size == 10);
	assert(!memcmp("BBBBBBBBBB", str1.data, str1.size));

	DBG("Extracting BEFORE from start of string");
	str1 = strbuf_view(&buf);
	str1 = strview_sub(str1, 0, 6);
	DBG("Result = %"PRIstr"\n", PRIstrarg(str1));
	assert(str1.size == 6);
	assert(!memcmp("BEFORE", str1.data, str1.size));

	DBG("Extracting AFTER from end of string");
	str1 = strbuf_view(&buf);
	str1 = strview_sub(str1, -5, INT_MAX);
	DBG("Result = %"PRIstr"\n\n\n", PRIstrarg(str1));
	assert(str1.size == 5);
	assert(!memcmp(str1.data, "AFTER", str1.size));

	DBG("**Testing strview_pop_first_split()**\n");

	strbuf_cat(&buf, cstr("123/456/789"));
	str2 = strbuf_view(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("123", str1.data, str1.size));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("456", str1.data, str1.size));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("789", str1.data, str1.size));
	assert(!strview_is_valid(str2));

	str1 = strbuf_view(&buf);
	DBG("Meanwhile, the buffer remains unchanged! %"PRIstr"\n\n\n", PRIstrarg(str1));

	DBG("**Testing strview_pop_first_split_nocase()**\n");

	strbuf_cat(&buf, cstr("123r456R789"));
	str2 = strbuf_view(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = strview_pop_first_split_nocase(&str2, cstr("r"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("123", str1.data, str1.size));
	str1 = strview_pop_first_split_nocase(&str2, cstr("r"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("456", str1.data, str1.size));
	str1 = strview_pop_first_split_nocase(&str2, cstr("r"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(!memcmp("789", str1.data, str1.size));
	assert(!strview_is_valid(str2));

	DBG("**Testing edge cases for strview_pop_first_split()**\n");

	strbuf_cat(&buf, cstr("/456/789/"));
	str2 = strbuf_view(&buf);

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data);
	assert(str2.size == 8);
	assert(!memcmp("456/789/", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("456", str1.data, str1.size));
	assert(str2.size == 4);
	assert(!memcmp("789/", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("789", str1.data, str1.size));
	assert(str2.size == 0);
	assert(str2.data);		//remainder string should be valid and length 0, as a delimiter was found

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);		//split string should be the entire source, which is a valid string of length 0
	assert(str1.data);	
	assert(str2.size == 0);
	assert(str2.data == NULL); 	//remaining string should be invalid as there were no delimiters found

	strbuf_cat(&buf, cstr("no-delimiters"));
	str2 = strbuf_view(&buf);
	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_first_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.data);
	assert(str1.size == sizeof("no-delimiters")-1);
	assert(str2.data == NULL);
	assert(str2.size == 0);

	DBG("**Testing strview_pop_last_split()**\n");

	strbuf_cat(&buf, cstr("123/456/789"));
	str2 = strbuf_view(&buf);
	DBG("Splitting %"PRIstr, PRIstrarg(str2));
	
	str1 = strview_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("789", str1.data, str1.size));
	assert(str2.size == 7);
	assert(!memcmp("123/456", str2.data, str2.size));

	str1 = strview_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("456", str1.data, str1.size));
	assert(str2.size == 3);
	assert(!memcmp("123", str2.data, str2.size));


	DBG("**Testing edge cases for strview_pop_last_split()**\n");

	strbuf_cat(&buf, cstr("/456/789/"));
	str2 = strbuf_view(&buf);

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data);
	assert(str2.size == 8);
	assert(!memcmp("/456/789", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("789", str1.data, str1.size));
	assert(str2.size == 4);
	assert(!memcmp("/456", str2.data, str2.size));

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 3);
	assert(!memcmp("456", str1.data, str1.size));
	assert(str2.size == 0);
	assert(str2.data);

	DBG("Splitting \"%"PRIstr"\"", PRIstrarg(str2));
	str1 = strview_pop_last_split(&str2, cstr("/"));
	DBG("result = \"%"PRIstr"\" remaining = \"%"PRIstr"\"\n\n\n", PRIstrarg(str1), PRIstrarg(str2));
	assert(str1.size == 0);
	assert(str1.data);
	assert(str2.size == 0);
	assert(str2.data == NULL);

	DBG("**Testing strview_find_first()**\n");

	str1 = cstr("needle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 0);

	str1 = cstr("needleneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 0);

	str1 = cstr("needlneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 5);
	
	str1 = cstr("haystack");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu (a valid needle of length 0 should be found)\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 0);

	str2.data = NULL;
	DBG("Looking for (invalid) in \"%"PRIstr"\"", PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(!strview_is_valid(search_result));
	DBG("Not Found (a needle which is an invalid string should not be found)\n");

	str1.data = NULL;
	str1.size = 0;
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in (invalid)", PRIstrarg(str2));
	search_result = strview_find_first(str1, str2);
	assert(!strview_is_valid(search_result));
	DBG("Not Found (a needle in an invalid haystack should not be found)\n");


	str1 = cstr("");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu (a valid needle of length 0 should be found in a valid haystack of length 0)\n\n\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 0);



	DBG("**Testing strview_find_last()**\n");

	str1 = cstr("needle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_last(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 0);

	str1 = cstr("needleneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_last(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 6);

	str1 = cstr("needlneedle");
	str2 = cstr("needle");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_last(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 5);
	
	str1 = cstr("haystack");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_last(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu (a valid needle of length 0 should be found at the index of the last char+1)\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 8);

	str2.data = NULL;
	DBG("Looking for (invalid) in \"%"PRIstr"\"",  PRIstrarg(str1));
	search_result = strview_find_last(str1, str2);
	assert(!strview_is_valid(search_result));
	DBG("Not Found (a needle which is an invalid string should not be found)\n");

	str1.data = NULL;
	str1.size = 0;
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in (invalid)", PRIstrarg(str2));
	search_result = strview_find_first(str1, str2);
	assert(!strview_is_valid(search_result));
	DBG("Not Found (a needle in an invalid haystack should not be found)\n");

	str1 = cstr("");
	str2 = cstr("");
	DBG("Looking for \"%"PRIstr"\" in \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	search_result = strview_find_first(str1, str2);
	assert(strview_is_valid(search_result));
	DBG("Found at index %zu (a valid needle of length 0 should be found at the index of the last char+1)\n\n\n", (search_result.data - str1.data));
	assert((search_result.data - str1.data) == 0);

	DBG("** Testing strview_is_valid() **");
	str1.data = NULL;
	assert(!strview_is_valid(str1));
	str1 = cstr("valid");
	assert(strview_is_valid(str1));
	DBG("strview_is_valid() works\n\n");

	DBG("** Testing strbuf_append_char() **");
	strbuf_cat(&buf, cstr(""));
	strbuf_shrink(&buf);

	chrptr = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!";
	while(*chrptr)
		strbuf_append_char(&buf, *chrptr++);

	str1 = strbuf_view(&buf);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!", str1.data, str1.size));

	DBG("Current capacity = %i - Shrinking buffer", buf->capacity);
	strbuf_shrink(&buf);
	DBG("Current capacity = %i\n", buf->capacity);
	assert(buf->capacity == (int)strlen(buf->cstr));

	DBG("** Testing strbuf_cat() with a single invalid str **");
	DBG("strbuf_cat() should always return a valid string");
	str1.data = NULL;
	str1.size = 0;
	str1 = strbuf_cat(&buf, str1);
	DBG("Size = %i\n\n\n", str1.size);
	assert(str1.size == 0);
	assert(str1.data);

	DBG("** Testing strview_is_match() **");
	strbuf_cat(&buf, cstr("Hello"));
	assert(strview_is_match(strbuf_view(&buf), cstr("Hello")));
	str1 = cstr_SL("Test");
	str2 = cstr("Test");
	assert(strview_is_match(str1, str2));
	DBG("OK\n\n\n");

	DBG("** Testing strview_is_match_nocase() **");
	assert(strview_is_match_nocase(cstr("hEllO"), cstr("Hello")));
	assert(!strview_is_match_nocase(cstr("hEllO"), cstr("Hell")));
	DBG("OK\n\n\n");

	DBG("** Testing strview_trim() **");
	str1 = cstr("/.;/hello;./;.");
	str2 = cstr("./;");
	DBG("Trimming \"%"PRIstr"\" from \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	str1 = strview_trim(str1, str2);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("hello", str1.data, str1.size));

	str1 = cstr(" 123");
	str2 = cstr(" ");
	DBG("Trimming \"%"PRIstr"\" from \"%"PRIstr"\"", PRIstrarg(str2), PRIstrarg(str1));
	str1 = strview_trim(str1, str2);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("123", str1.data, str1.size));

	DBG("** Destroying the buffer **\n\n\n");
	strbuf_destroy(&buf);
	assert(buf == NULL);

	DBG("** Testing with a fixed capacity buffer (non-dynamic) from a given memory address");
	DBG("** This can be done, but if strbuf_cat() is passed a strview_t referencing data within the target buffer, it will fail and return an empty buffer.");
	DBG("** This is because it will be unable to allocate a temporary buffer to build the output\n");

	assert(!strbuf_create_fixed(static_buf+3, STATIC_BUFFER_SIZE));	//fail due to badly aligned address
	assert(!strbuf_create_fixed(static_buf, sizeof(strbuf_t)));		//fail due to insufficient space

	DBG("Creating strbuf_t from a fixed buffer of %i bytes", STATIC_BUFFER_SIZE);
	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);
	DBG("Available capacity is %i characters (size of strbuf_t = %zu)", buf->capacity, sizeof(strbuf_t));
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

	str1 = strbuf_view(&buf);
	DBG("Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp("Once upon a time there was a very smelly camel that poked it's tongue out and puffed it up.", str1.data, str1.size));

	DBG("Trying to pass data from the destination buffer into strbuf_cat() without a dynamic buffer (should fail and return empty buffer)");
	str1 = strbuf_view(&buf);
	str1 = strview_sub(str1, 5, 10);
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


	DBG("** Testing various edge cases of strview_to_cstr() **\n");
	static_buf[0] = 0x7F;

	strview_to_cstr(static_buf, 0, cstr_SL("string"));
	assert(static_buf[0] == 0x7F);

	strview_to_cstr(static_buf, 1, cstr_SL("string"));
	assert(static_buf[0] == 0);

	strview_to_cstr(static_buf, 4, cstr_SL("string"));
	assert(!memcmp(static_buf, "str", 4));

	strview_to_cstr(static_buf, 4, cstr_SL("str"));
	assert(!memcmp(static_buf, "str", 4));

	strview_to_cstr(static_buf, 100, cstr_SL("string"));
	assert(!memcmp(static_buf, "string", sizeof("string")));

	DBG("** OK **\n\n\n");

	DBG("** Testing edge cases for strview_sub() **\n");

	str1 = strview_sub(cstr("X"), 0, 0);	//return a valid string of length 0 within the buffer space
	assert(strview_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), -1, -1);	//return a valid string of length 0 within the buffer space
	assert(strview_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), 0, -1);	//return a valid string of length 0 within the buffer space
	assert(strview_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), -1, 0);	//return a valid string of length 0 within the buffer space
	assert(strview_is_valid(str1));
	assert(str1.size == 0);
	assert(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), 1, 0);	//return an invalid string if end is before beginning
	assert(!strview_is_valid(str1));
	assert(str1.size == 0);

	str1 = strview_sub(cstr("X"), 1, -1);	//return an invalid string if end is before beginning
	assert(!strview_is_valid(str1));
	assert(str1.size == 0);

	str1 = strview_sub(cstr(""), 0, 0);		//return a valid string of length 0
	assert(strview_is_valid(str1));
	assert(str1.size == 0);

	str1 = strview_sub(cstr(""), -1, -1);		//return a valid string of length 0
	assert(strview_is_valid(str1));
	assert(str1.size == 0);

	str1 = strview_sub(cstr(""), -1, 0);		//return a valid string of length 0
	assert(strview_is_valid(str1));
	assert(str1.size == 0);

	str1 = strview_sub(cstr(""), 0, -1);		//return a valid string of length 0
	assert(strview_is_valid(str1));
	assert(str1.size == 0);

	DBG("** OK **\n\n\n");

	DBG("** Testing number conversions **\n");

	tmpll = strview_to_ll(cstr("  -289765138"));
	assert(tmpll == -289765138);

	tmpll = strview_to_ll(cstr("-289765138  "));
	assert(tmpll == -289765138);

	tmpll = strview_to_ll(cstr("  289765138"));
	assert(tmpll == 289765138);

	tmpll = strview_to_ll(cstr("289765138  "));
	assert(tmpll == 289765138);

	tmpull = strview_to_ull(cstr("0xFf715cC  "));
	assert(tmpull == 0xFF715CC);

	tmpull = strview_to_ull(cstr("  0XFf715cC"));
	assert(tmpull == 0xFF715CC);

	tmpull = strview_to_ull(cstr("0b110011010111"));
	assert(tmpull == 0b110011010111);

	DBG("** OK **\n\n\n");

	DBG("** Testing string_compare() **\n");
	assert(strview_compare(cstr("abd"), cstr("abc")) > 0);
	assert(strview_compare(cstr("aba"), cstr("abd")) < 0);
	assert(strview_compare(cstr("abc"), cstr("abc")) == 0);
	assert(strview_compare(cstr("abca"), cstr("abc")) > 0);
	assert(strview_compare(cstr("abc"), cstr("abca")) < 0);
	str1.data = NULL;
	str1.size = 0;
	assert(!strview_compare(cstr(""), str1));	//an empty string == an invalid string for strview_compare();

	DBG("** OK **\n\n\n");

	DBG("** Testing strview_to_float() **\n");

	str1 = cstr("inf");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), strview_to_float(str1));
	assert(strview_to_float(str1) == INFINITY);

	str1 = cstr("-inf");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), strview_to_float(str1));
	assert(strview_to_float(str1) == -INFINITY);

	str1 = cstr("nan");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), strview_to_float(str1));
	assert(strview_to_float(str1) != strview_to_float(str1));

	str1 = cstr("fred");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), strview_to_float(str1));
	assert(strview_to_float(str1) != strview_to_float(str1));

	str1 = cstr(".");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), strview_to_float(str1));
	assert(strview_to_float(str1) != strview_to_float(str1));

	str1 = cstr(".E");
	DBG("\"%"PRIstr"\" returns %f", PRIstrarg(str1), strview_to_float(str1));
	assert(strview_to_float(str1) != strview_to_float(str1));

	TEST_STR_TO_FLOAT("%f", 183.4179);
	TEST_STR_TO_FLOAT("%f", -183.4179);
	TEST_STR_TO_FLOAT("%e", 1000000);
	TEST_STR_TO_FLOAT("%e", 148.913E-23);
	TEST_STR_TO_FLOAT("%f", .002);
	TEST_STR_TO_FLOAT("%f", -.002);
	TEST_STR_TO_FLOAT("%.9f", .100000002);
	TEST_STR_TO_FLOAT("%.9f", 39E-8);

	DBG("\n\n");
	DBG("** Testing strview_pop_split() **\n");
	str1 = cstr("123");
	str2 = strview_pop_split(&str1, 1);
	assert(str1.size == 2);
	assert(str2.size == 1);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str1.data, "23", str1.size));
	assert(!memcmp(str2.data, "1", str2.size));

	str1 = cstr("123");
	str2 = strview_pop_split(&str1, -1);
	assert(str1.size == 2);
	assert(str2.size == 1);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str1.data, "12", str1.size));
	assert(!memcmp(str2.data, "3", str2.size));

	str1 = cstr("123");
	str2 = strview_pop_split(&str1, 0);
	assert(str1.size == 3);
	assert(str2.size == 0);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str1.data, "123", str1.size));
	assert(!memcmp(str2.data, "", str2.size));

	str1 = cstr("123");
	str2 = strview_pop_split(&str1, -3);
	assert(str1.size == 0);
	assert(str2.size == 3);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str1.data, "", str1.size));
	assert(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = strview_pop_split(&str1, 3);
	assert(str1.size == 0);
	assert(str2.size == 3);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str1.data, "", str1.size));
	assert(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = strview_pop_split(&str1, 4);	//over range, pops ALL
	assert(str1.size == 0);
	assert(str2.size == 3);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = strview_pop_split(&str1, -4);	//out of range, pops all
	assert(str1.size == 0);
	assert(str2.size == 3);
	assert(strview_is_valid(str1));
	assert(strview_is_valid(str2));
	assert(!memcmp(str2.data, "123", str2.size));

	#ifdef STRBUF_PROVIDE_PRINTF
	DBG("\n\n");
	DBG("** Testing strbuf_printf() **\n");

	buf = strbuf_create(INITIAL_BUF_CAPACITY, &strbuf_allocator);
	str1 = strbuf_printf(&buf, "Hello from printf! have some numbers... %i %i %i %i %i %i %i", 6246456, 3466765, 435234, 4598756, 94572, 69, 42597);

	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from printf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place

	DBG("** Testing strbuf_append_printf() **\n");

	str1 = strbuf_append_printf(&buf, " Appending one more number %i", 748921);
	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from printf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597 Appending one more number 748921", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place
	strbuf_destroy(&buf);

	#endif

	#ifdef STRBUF_PROVIDE_PRNF
	DBG("\n\n");
	DBG("** Testing strbuf_prnf() **\n");

	buf = strbuf_create(INITIAL_BUF_CAPACITY, &strbuf_allocator);
	str1 = strbuf_prnf(&buf, "Hello from prnf! have some numbers... %i %i %i %i %i %i %i", 6246456, 3466765, 435234, 4598756, 94572, 69, 42597);

	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from prnf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place

	DBG("** Testing strbuf_append_prnf() **\n");

	str1 = strbuf_append_prnf(&buf, " Appending one more number %i", 748921);

	DBG("** Result = \"%"PRIstr"\"\n", PRIstrarg(str1));
	assert(!memcmp(str1.data, "Hello from prnf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597 Appending one more number 748921", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);	//check the 0 terminator is in place
	strbuf_destroy(&buf);
	#endif

	buf = strbuf_create(INITIAL_BUF_CAPACITY, &strbuf_allocator);
	DBG("** Testing strbuf_assign() source outside of the destination **\n");
	str1 = strbuf_assign(&buf, cstr("***Hello test***"));
	assert(!memcmp(str1.data, "***Hello test***", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);

	DBG("** Testing strbuf_assign() source from the destination **\n");
	strbuf_assign(&buf, strview_trim(strbuf_view(&buf), cstr("*")));
	str1 = strbuf_assign(&buf, cstr("Hello test"));
	assert(!memcmp(str1.data, "Hello test", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);

	DBG("** Testing strbuf_append(), with source from the destination **\n");
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_append(&buf, strbuf_view(&buf));
	assert(!memcmp(str1.data, "{Hello-testing-some-string}{Hello-testing-some-string}", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);

	DBG("** Testing strbuf_prepend(), with source from the destination **\n");
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_prepend(&buf, strbuf_view(&buf));
	assert(!memcmp(str1.data, "{Hello-testing-some-string}{Hello-testing-some-string}", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);

	DBG("** Testing strbuf_insert_at_index(), with source from the destination **\n");
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_insert_at_index(&buf, 6, strbuf_view(&buf));
	assert(!memcmp(str1.data, "{Hello{Hello-testing-some-string}-testing-some-string}", str1.size));
	assert((int)strlen(buf->cstr) == buf->size);

	DBG("** Testing line parser with a few lines **");

	const char* sometext = "\
First line CRLF\r\n\
Second line CR\r\
Third line LF\n\
CRLF line followed by an empty CR line\r\n\
\r\
CRLF line followed by an empty LF line\r\n\
\n\
CRLF line followed by an empty LFCR line\r\n\
\n\r\
LFCR line followed by an empty CR line\n\r\
\r\
LFCR line followed by an empty LF line\n\r\
\n\
LFCR line followed by an empty CRLF line\n\r\
\r\n\
CR line followed by an empty CRLF line\r\
\r\n\
CR line followed by an empty LFCR line\r\
\n\r\
LF line followed by an empty CRLF line\n\
\r\n\
LF line followed by an empty LFCR line\n\
\n\r\
This text has no line ending";

	str2 = cstr(sometext);
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "First line CRLF", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "Second line CR", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "Third line LF", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "CRLF line followed by an empty CR line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "CRLF line followed by an empty LF line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "CRLF line followed by an empty LFCR line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "LFCR line followed by an empty CR line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "LFCR line followed by an empty LF line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "LFCR line followed by an empty CRLF line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "CR line followed by an empty CRLF line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "CR line followed by an empty LFCR line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "LF line followed by an empty CRLF line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "LF line followed by an empty LFCR line", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));
	str1 = strview_pop_line(&str2, NULL); assert(strview_is_valid(str1)); assert(!memcmp(str1.data, "", str1.size));DBG("[%"PRIstr"]", PRIstrarg(str1));

	str1 = strview_pop_line(&str2, NULL); assert(!strview_is_valid(str1)); assert(!memcmp(str2.data, "This text has no line ending", str1.size));

	DBG("\n\n** Testing line parser by appending chars to a buffer 1 at a time **");

	char eol = 0;
	strbuf_assign(&buf, cstr(""));
	chrptr = sometext;

	TEST_LINE_POP("First line CRLF");
	TEST_LINE_POP("Second line CR");
	TEST_LINE_POP("Third line LF");
	TEST_LINE_POP("CRLF line followed by an empty CR line");
	TEST_LINE_POP("");
	TEST_LINE_POP("CRLF line followed by an empty LF line");
	TEST_LINE_POP("");
	TEST_LINE_POP("CRLF line followed by an empty LFCR line");
	TEST_LINE_POP("");
	TEST_LINE_POP("LFCR line followed by an empty CR line");
	TEST_LINE_POP("");
	TEST_LINE_POP("LFCR line followed by an empty LF line");
	TEST_LINE_POP("");
	TEST_LINE_POP("LFCR line followed by an empty CRLF line");
	TEST_LINE_POP("");
	TEST_LINE_POP("CR line followed by an empty CRLF line");
	TEST_LINE_POP("");
	TEST_LINE_POP("CR line followed by an empty LFCR line");
	TEST_LINE_POP("");
	TEST_LINE_POP("LF line followed by an empty CRLF line");
	TEST_LINE_POP("");
	TEST_LINE_POP("LF line followed by an empty LFCR line");
	TEST_LINE_POP("");

	DBG("\n\n** Testing strbuf_insert_before() **");

	strbuf_assign(&buf, cstr("Hello"));
	str1 = strview_find_last(strbuf_view(&buf), cstr(""));
	strbuf_insert_before(&buf, str1, cstr("-test"));
	assert(!memcmp(buf->cstr, "Hello-test", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_last(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_before(&buf, str1, cstr("..."));
	assert(!memcmp(buf->cstr, "Hello Me...llow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_before(&buf, str1, cstr("---"));
	assert(!memcmp(buf->cstr, "He---llo Mellow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr(""));
	strbuf_insert_before(&buf, str1, cstr("***"));
	assert(!memcmp(buf->cstr, "***Hello Mellow", buf->size));

	DBG("\n\n** Testing strbuf_insert_after() **");

	strbuf_assign(&buf, cstr("Hello"));
	str1 = strview_find_last(strbuf_view(&buf), cstr(""));
	strbuf_insert_after(&buf, str1, cstr("-test"));
	assert(!memcmp(buf->cstr, "Hello-test", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_last(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_after(&buf, str1, cstr("..."));
	assert(!memcmp(buf->cstr, "Hello Mell...ow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_after(&buf, str1, cstr("---"));
	assert(!memcmp(buf->cstr, "Hell---o Mellow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr(""));
	strbuf_insert_after(&buf, str1, cstr("***"));
	assert(!memcmp(buf->cstr, "***Hello Mellow", buf->size));

	strbuf_destroy(&buf);

	#ifdef STRBUF_DEFAULT_ALLOCATOR_STDLIB
	DBG("\n\n** Testing default allocator **");

	buf = strbuf_create(1000,NULL);
	assert(buf);
	assert(buf->cstr);
	assert(buf->size == 0);
	assert(buf->capacity == 1000);
	strbuf_destroy(&buf);
	#endif

	DBG("\n\n** Testing strview_pop_left() **");

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr("testing"));
	str3 = strview_pop_left(&str1, str2);
	assert(strview_is_match(str3, cstr("Hello-")));
	assert(strview_is_match(str1, cstr("testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str3 = strview_pop_left(&str1, str2);
	assert(strview_is_valid(str3)); assert(str3.size == 0);
	assert(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str3 = strview_pop_left(&str1, str2);
	assert(strview_is_valid(str1)); assert(str1.size == 0);
	assert(strview_is_match(str3, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str2.data--;
	str3 = strview_pop_left(&str1, str2);
	assert(!strview_is_valid(str3));
	assert(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str2.data++;
	str3 = strview_pop_left(&str1, str2);
	assert(!strview_is_valid(str3));
	assert(strview_is_match(str1, cstr("Hello-testing-123")));


	DBG("\n\n** Testing strview_pop_right() **");

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr("testing"));
	str3 = strview_pop_right(&str1, str2);
	assert(strview_is_match(str3, cstr("-123")));
	assert(strview_is_match(str1, cstr("Hello-testing")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str3 = strview_pop_right(&str1, str2);
	assert(strview_is_valid(str1)); assert(str1.size == 0);
	assert(strview_is_match(str3, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str3 = strview_pop_right(&str1, str2);
	assert(strview_is_valid(str3)); assert(str3.size == 0);
	assert(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str2.data--;
	str3 = strview_pop_right(&str1, str2);
	assert(!strview_is_valid(str3));
	assert(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str2.data++;
	str3 = strview_pop_right(&str1, str2);
	assert(!strview_is_valid(str3));
	assert(strview_is_match(str1, cstr("Hello-testing-123")));


	DBG("\n\n\n*** Everything worked ***\n");

	return 0;
}


/*	Split a strview_t at a position specified by pos

	If pos references characters within *strview_ptr, return a strview_t representing all characters to the left of pos.
	If pos references the upper limit of *strview_ptr, the entire *strview_ptr is returned.
	If pos references the start of *strview_ptr, a valid strview_t of length 0 is returned.

	The returned characters are removed (popped) from *strview_ptr

	If strview_ptr is NULL, *strview_ptr is invalid, or pos is not a valid reference, an invalid string is returned and strview_ptr is unmodified.
*/
	strview_t strview_pop_left(strview_t* strview_ptr, strview_t pos);

/*	Split a strview_t at a position specified by pos

	If pos references characters within *strview_ptr, return a strview_t representing all characters to the right of pos.
	If the upper limit of pos matches the upper limit of *strview_ptr, a valid strview_t of length 0 is returned.

	The returned characters are removed (popped) from *strview_ptr

	If strview_ptr is NULL, *strview_ptr is invalid, or pos is not a valid reference, an invalid string is returned and strview_ptr is unmodified.
*/
	strview_t strview_pop_right(strview_t* strview_ptr, strview_t pos);


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
