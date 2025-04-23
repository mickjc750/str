/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>
	#include <math.h>

	#include "greatest.h"
	#include "strbuf.h"
	#include "strview.h"
	#include "strnum.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************
 
//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	GREATEST_MAIN_DEFS();

//********************************************************************************************************
// Public variables
//********************************************************************************************************


//********************************************************************************************************
// Private variables
//********************************************************************************************************

	#define STATIC_BUFFER_SIZE	200
	static char static_buf[STATIC_BUFFER_SIZE] __attribute__ ((aligned));

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size);

	SUITE(suite_strbuf);
	TEST test_strbuf_create_using_malloc(void);
	TEST test_strbuf_create_using_allocator(void);
	TEST test_strbuf_create_static(void);
	TEST test_strbuf_create_init(void);
	TEST test_strbuf_strcat(void);
	TEST test_strbuf_shrink(void);
	TEST test_strbuf_printf(void);
	TEST test_strbuf_append_printf(void);
	TEST test_strbuf_prnf(void);
	TEST test_strbuf_append_prnf(void);
	TEST test_strbuf_assign(void);
	TEST test_strbuf_append(void);
	TEST test_strbuf_prepend(void);
	TEST test_strbuf_insert_at_index(void);
	TEST test_strbuf_insert_before(void);
	TEST test_strbuf_insert_after(void);
	TEST test_strbuf_to_cstr(void);
	TEST test_strbuf_terminate_views(void);

	SUITE(suite_strview);
	TEST test_strview_sub(void);
	TEST test_strview_sub_edge_cases(void);
	TEST test_strview_split_first_delim(void);
	TEST test_strview_split_all(void);
	TEST test_strview_split_first_delim_edge_cases(void);
	TEST test_strview_split_last_delim(void);
	TEST test_strview_split_last_delim_edge_cases(void);
	TEST test_strview_find_first(void);
	TEST test_strview_find_first_nocase(void);
	TEST test_strview_find_first_edge_cases(void);
	TEST test_strview_find_last(void);
	TEST test_strview_find_last_nocase(void);
	TEST test_strview_find_last_edge_cases(void);
	TEST test_strview_is_valid(void);
	TEST test_strview_append_char(void);
	TEST test_strview_is_match(void);
	TEST test_strview_is_match_nocase(void);
	TEST test_strview_starts_with(void);
	TEST test_strview_starts_with_nocase(void);
	TEST test_strview_trim_start(void);
	TEST test_strview_trim_end(void);
	TEST test_strview_trim(void);
	TEST test_strview_to_cstr(void);
	TEST test_strview_compare(void);
	TEST test_strview_split_index(void);
	TEST test_strview_split_line(void);
	TEST test_strview_split_left(void);
	TEST test_strview_split_right(void);
	TEST test_strview_dequote(void);
	TEST test_strview_contains(void);
	TEST test_strview_contains_nocase(void);
	TEST test_strnum_value(void);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(suite_strbuf);
	RUN_SUITE(suite_strview);
	GREATEST_MAIN_END();
	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static void* allocator(struct strbuf_allocator_t* this_allocator, void* ptr_to_free, size_t size)
{
	(void)this_allocator;
	void* result;
	result = realloc(ptr_to_free, size);
	assert(size==0 || result);	// You need to catch a failed allocation here.
	return result;
}

SUITE(suite_strbuf)
{
	RUN_TEST(test_strbuf_create_using_malloc);
	RUN_TEST(test_strbuf_create_init);
	RUN_TEST(test_strbuf_create_using_allocator);
	RUN_TEST(test_strbuf_create_static);
	RUN_TEST(test_strbuf_strcat);
	RUN_TEST(test_strbuf_shrink);
	RUN_TEST(test_strbuf_printf);
	RUN_TEST(test_strbuf_append_printf);
	RUN_TEST(test_strbuf_prnf);
	RUN_TEST(test_strbuf_append_prnf);
	RUN_TEST(test_strbuf_assign);
	RUN_TEST(test_strbuf_append);
	RUN_TEST(test_strbuf_prepend);
	RUN_TEST(test_strbuf_insert_at_index);
	RUN_TEST(test_strbuf_insert_before);
	RUN_TEST(test_strbuf_insert_after);
	RUN_TEST(test_strbuf_to_cstr);
	RUN_TEST(test_strbuf_terminate_views);
}

SUITE(suite_strview)
{
	RUN_TEST(test_strview_sub);
	RUN_TEST(test_strview_sub_edge_cases);
	RUN_TEST(test_strview_split_first_delim);
	RUN_TEST(test_strview_split_all);
	RUN_TEST(test_strview_split_first_delim_edge_cases);
	RUN_TEST(test_strview_split_last_delim);
	RUN_TEST(test_strview_split_last_delim_edge_cases);
	RUN_TEST(test_strview_find_first);
	RUN_TEST(test_strview_find_first_nocase);
	RUN_TEST(test_strview_find_first_edge_cases);
	RUN_TEST(test_strview_find_last);
	RUN_TEST(test_strview_find_last_nocase);
	RUN_TEST(test_strview_find_last_edge_cases);
	RUN_TEST(test_strview_is_valid);
	RUN_TEST(test_strview_append_char);
	RUN_TEST(test_strview_is_match);
	RUN_TEST(test_strview_is_match_nocase);
	RUN_TEST(test_strview_starts_with);
	RUN_TEST(test_strview_starts_with_nocase);
	RUN_TEST(test_strview_trim_start);
	RUN_TEST(test_strview_trim_end);
	RUN_TEST(test_strview_trim);
	RUN_TEST(test_strview_to_cstr);
	RUN_TEST(test_strview_compare);
	RUN_TEST(test_strview_split_index);
	RUN_TEST(test_strview_split_line);
	RUN_TEST(test_strview_split_left);
	RUN_TEST(test_strview_split_right);
	RUN_TEST(test_strnum_value);
	RUN_TEST(test_strview_dequote);
	RUN_TEST(test_strview_contains);
	RUN_TEST(test_strview_contains_nocase);
}

TEST test_strbuf_create_using_malloc(void)
{
	#define INITIAL_BUF_CAPACITY 16
	strbuf_t* buf;
	buf = strbuf_create(INITIAL_BUF_CAPACITY, NULL);

	ASSERT(buf);
	ASSERT(buf->cstr);
	ASSERT(buf->size == 0);
	ASSERT(buf->capacity == INITIAL_BUF_CAPACITY);
	ASSERT(buf->cstr[0] == 0);
	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
	#undef INITIAL_BUF_CAPACITY
}

TEST test_strbuf_create_init(void)
{
	#define TEST_STRING	"The quick brown fox jumped over the lazy dog"
	strbuf_t* buf;
	buf = strbuf_create(cstr(TEST_STRING), NULL);

	ASSERT(buf);
	ASSERT(buf->cstr);
	ASSERT(buf->size == sizeof(TEST_STRING)-1);
	ASSERT(buf->cstr[sizeof(TEST_STRING)-1] == 0);
	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
	#undef TEST_STRING
}

TEST test_strbuf_create_using_allocator(void)
{
	#define INITIAL_BUF_CAPACITY 16
	strbuf_allocator_t strbuf_allocator = {.allocator = allocator};
	strbuf_t* buf;
	buf = strbuf_create(INITIAL_BUF_CAPACITY, &strbuf_allocator);

	ASSERT(buf);
	ASSERT(buf->cstr);
	ASSERT(buf->size == 0);
	ASSERT(buf->capacity == INITIAL_BUF_CAPACITY);
	ASSERT(buf->cstr[0] == 0);
	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
	#undef INITIAL_BUF_CAPACITY
}

TEST test_strbuf_create_static(void)
{
	strbuf_t* buf;

	ASSERT(!strbuf_create_fixed(static_buf+3, STATIC_BUFFER_SIZE));	//must fail due to badly aligned address
	ASSERT(!strbuf_create_fixed(static_buf, sizeof(strbuf_t)));		//must fail due to insufficient space

	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);
	ASSERT(buf);
	ASSERT(buf->cstr);
	ASSERT(buf->size == 0);
	ASSERT(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t)-1);
	ASSERT(buf->cstr[0] == 0);
	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
}

TEST test_strbuf_strcat(void)
{
	#define INITIAL_BUF_CAPACITY 16
	strbuf_t* buf;
	strview_t str1;
	buf = strbuf_create(INITIAL_BUF_CAPACITY, NULL);
	ASSERT(buf);

	//Concatenating "AAAAAAAAAA", "BBBBBBBBBB", "CCCCCCCCCC"
	strbuf_cat(&buf, cstr("AAAAAAAAAA"), cstr("BBBBBBBBBB"), cstr("CCCCCCCCCC"));
	ASSERT(buf->capacity >= 30);
	ASSERT(buf->size == 30);
	ASSERT(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC"));

	//Appending -AFTER to existing buffer
	strbuf_cat(&buf, strbuf_view(&buf), cstr("-AFTER"));
	ASSERT(!strcmp(buf->cstr, "AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	//Prepending BEFORE- to existing buffer
	strbuf_cat(&buf, cstr("BEFORE-"), strbuf_view(&buf));
	ASSERT(!strcmp(buf->cstr, "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"));

	// Testing strbuf_cat() with a single invalid str
	// strbuf_cat() should return a valid string of length 0
	str1 = STRVIEW_INVALID;
	str1 = strbuf_cat(&buf, str1);
	ASSERT(str1.size == 0);
	ASSERT(str1.data);

	strbuf_destroy(&buf);
	ASSERT(!buf);

	//Now perform some operations on a static buffer
	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);
	ASSERT(buf);
	ASSERT(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t) - 1);

	// Concatenating DDDDDDDDDD EEEEEEEEEE FFFFFFFFFF
	strbuf_cat(&buf, cstr("DDDDDDDDDD"), cstr("EEEEEEEEEE"), cstr("FFFFFFFFFF"));
	ASSERT(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t) - 1);	//capacity should not change
	ASSERT(buf->size == 30);
	ASSERT(!strcmp(buf->cstr, "DDDDDDDDDDEEEEEEEEEEFFFFFFFFFF"));

	// Trying to pass data from the destination buffer into strbuf_cat() without a dynamic buffer (should fail and return empty buffer
	str1 = strbuf_view(&buf);
	str1 = strview_sub(str1, 5, 10);
	strbuf_cat(&buf, cstr("never "), str1, cstr(" seen"));
	ASSERT(buf->size == 0);

	// Trying append too much data to the buffer (should fail and return empty buffer
	strbuf_assign(&buf, cstr(""));
	//this much should fit
	strbuf_append(&buf, cstr("BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH  "));
	//this much should empty the buffer
	strbuf_append(&buf, cstr("BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH BLAH  "));
	ASSERT(buf->size == 0);

	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
}

TEST test_strview_sub(void)
{
	strview_t str1;

	// Extracting BBBBBBBBBB from center of "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"
	str1 = cstr("BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER");
	str1 = strview_sub(str1, 17, 27);
	ASSERT(str1.size == 10);
	ASSERT(!memcmp("BBBBBBBBBB", str1.data, str1.size));

	// Extracting BEFORE from start of "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"
	str1 = cstr("BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER");
	str1 = strview_sub(str1, 0, 6);
	ASSERT(str1.size == 6);
	ASSERT(!memcmp("BEFORE", str1.data, str1.size));

	// Extracting AFTER from end of "BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER"
	str1 = cstr("BEFORE-AAAAAAAAAABBBBBBBBBBCCCCCCCCCC-AFTER");
	str1 = strview_sub(str1, -5, INT_MAX);
	ASSERT(str1.size == 5);
	ASSERT(!memcmp(str1.data, "AFTER", str1.size));

	PASS();
}

TEST test_strview_sub_edge_cases(void)
{
	strview_t str1;

	str1 = strview_sub(cstr("X"), 0, 0);	//return a valid string of length 0 within the buffer space
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);
	ASSERT(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), -1, -1);	//return a valid string of length 0 within the buffer space
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);
	ASSERT(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), 0, -1);	//return a valid string of length 0 within the buffer space
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);
	ASSERT(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), -1, 0);	//return a valid string of length 0 within the buffer space
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);
	ASSERT(str1.data[0] == 'X');

	str1 = strview_sub(cstr("X"), 1, 0);	//return an invalid string if end is before beginning
	ASSERT(!strview_is_valid(str1));
	ASSERT(str1.size == 0);

	str1 = strview_sub(cstr("X"), 1, -1);	//return an invalid string if end is before beginning
	ASSERT(!strview_is_valid(str1));
	ASSERT(str1.size == 0);

	str1 = strview_sub(cstr(""), 0, 0);		//return a valid string of length 0
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);

	str1 = strview_sub(cstr(""), -1, -1);		//return a valid string of length 0
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);

	str1 = strview_sub(cstr(""), -1, 0);		//return a valid string of length 0
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);

	str1 = strview_sub(cstr(""), 0, -1);		//return a valid string of length 0
	ASSERT(strview_is_valid(str1));
	ASSERT(str1.size == 0);
	PASS();
}

TEST test_strview_split_first_delim(void)
{
	strview_t str1, str2;

	str2 = cstr("123/456/789");

	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(!memcmp("123", str1.data, str1.size));
	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(!memcmp("456", str1.data, str1.size));
	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(!memcmp("789", str1.data, str1.size));
	ASSERT(!strview_is_valid(str2)); 	//source is entirely consumed


	str2 = cstr("ab\".\"c.d\"a.\"ef.gh\".a\"i");

	str1 = strview_split_first_delim(&str2, ".", true);
	ASSERT(!memcmp("ab\".\"c", str1.data, str1.size));
	str1 = strview_split_first_delim(&str2, ".", true);
	ASSERT(!memcmp("d\"a.\"ef", str1.data, str1.size));
	str1 = strview_split_first_delim(&str2, ".", true);
	ASSERT(!memcmp("gh\".a\"i", str1.data, str1.size));
	ASSERT(!strview_is_valid(str2)); 	//source is entirely consumed

	PASS();
}

TEST test_strview_split_all(void)
{
	#define DST_SIZE	4
	strview_t dst[DST_SIZE] = {0};
	int count;

	count = strview_split_all(2, dst, cstr("123/456/789"), "/", false);
	ASSERT(count == 2);
	ASSERT(!memcmp("123", dst[0].data, dst[0].size));
	ASSERT(!memcmp("456", dst[1].data, dst[1].size));
	ASSERT(!strview_is_valid(dst[2]));
	memset(dst, 0, sizeof(dst));

	count = strview_split_all(DST_SIZE, dst, cstr("123/456/789"), "/", false);
	ASSERT(count == 3);
	ASSERT(!memcmp("123", dst[0].data, dst[0].size));
	ASSERT(!memcmp("456", dst[1].data, dst[1].size));
	ASSERT(!memcmp("789", dst[2].data, dst[2].size));
	memset(dst, 0, sizeof(dst));

	count = strview_split_all(DST_SIZE, dst, STRVIEW_INVALID, "/", false);
	ASSERT(count == 0);
	ASSERT(!strview_is_valid(dst[0]));

	count = strview_split_all(DST_SIZE, dst, cstr("123/456/789"), "/", false);
	ASSERT(count == 3);
	ASSERT(!memcmp("123", dst[0].data, dst[0].size));
	ASSERT(!memcmp("456", dst[1].data, dst[1].size));
	ASSERT(!memcmp("789", dst[2].data, dst[2].size));

	#undef DST_SIZE
	PASS();
}

TEST test_strview_split_first_delim_edge_cases(void)
{
	strview_t str1, str2;

	str2 = cstr("/456/789/");

	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(str1.size == 0);
	ASSERT(str1.data);
	ASSERT(str2.size == 8);
	ASSERT(!memcmp("456/789/", str2.data, str2.size));

	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(str1.size == 3);
	ASSERT(!memcmp("456", str1.data, str1.size));
	ASSERT(str2.size == 4);
	ASSERT(!memcmp("789/", str2.data, str2.size));

	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(str1.size == 3);
	ASSERT(!memcmp("789", str1.data, str1.size));
	ASSERT(str2.size == 0);
	ASSERT(str2.data);		//remainder string should be valid and length 0, as a delim was found

	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(str1.size == 0);		//split string should be the entire source, which is a valid string of length 0
	ASSERT(str1.data);	
	ASSERT(str2.size == 0);
	ASSERT(str2.data == NULL); 	//remaining string should be invalid as there were no delims found

	str2 = cstr("no-delims");
	str1 = strview_split_first_delim(&str2, "/", false);
	ASSERT(str1.data);
	ASSERT(str1.size == sizeof("no-delims")-1);
	ASSERT(str2.data == NULL);
	ASSERT(str2.size == 0);

	PASS();
}

TEST test_strview_split_last_delim(void)
{
	strview_t str1, str2;

	str2 = cstr("123/456/789");
	str1 = strview_split_last_delim(&str2, "/", false);
	ASSERT(str1.size == 3);
	ASSERT(!memcmp("789", str1.data, str1.size));
	ASSERT(str2.size == 7);
	ASSERT(!memcmp("123/456", str2.data, str2.size));

	str1 = strview_split_last_delim(&str2, "/", false);
	ASSERT(str1.size == 3);
	ASSERT(!memcmp("456", str1.data, str1.size));
	ASSERT(str2.size == 3);
	ASSERT(!memcmp("123", str2.data, str2.size));


	str2 = cstr("ab\".\"c.d\"a.\"ef.gh\".a\"i");

	str1 = strview_split_last_delim(&str2, ".", true);
	ASSERT(!memcmp("gh\".a\"i", str1.data, str1.size));
	str1 = strview_split_last_delim(&str2, ".", true);
	ASSERT(!memcmp("d\"a.\"ef", str1.data, str1.size));
	str1 = strview_split_last_delim(&str2, ".", true);
	ASSERT(!memcmp("ab\".\"c", str1.data, str1.size));

	ASSERT(!strview_is_valid(str2)); 	//source is entirely consumed

	PASS();
}

TEST test_strview_split_last_delim_edge_cases(void)
{
	strview_t str1, str2;

	str2 = cstr("/456/789/");

	str1 = strview_split_last_delim(&str2, "/", false);
	ASSERT(str1.size == 0);
	ASSERT(str1.data);
	ASSERT(str2.size == 8);
	ASSERT(!memcmp("/456/789", str2.data, str2.size));

	str1 = strview_split_last_delim(&str2, "/", false);
	ASSERT(str1.size == 3);
	ASSERT(!memcmp("789", str1.data, str1.size));
	ASSERT(str2.size == 4);
	ASSERT(!memcmp("/456", str2.data, str2.size));

	str1 = strview_split_last_delim(&str2, "/", false);
	ASSERT(str1.size == 3);
	ASSERT(!memcmp("456", str1.data, str1.size));
	ASSERT(str2.size == 0);
	ASSERT(str2.data);

	str1 = strview_split_last_delim(&str2, "/", false);
	ASSERT(str1.size == 0);
	ASSERT(str1.data);
	ASSERT(str2.size == 0);
	ASSERT(str2.data == NULL);

	PASS();
}

TEST test_strview_find_first(void)
{
	strview_t str1, str2, search_result;

	str1 = cstr("needle");
	str2 = cstr("needle");
	search_result = strview_find_first(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needle");
	search_result = strview_find_first(str1, "needle");
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needleneedle");
	str2 = cstr("needle");
	search_result = strview_find_first(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needlneedle");
	str2 = cstr("needle");
	search_result = strview_find_first(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 5);
	PASS();
}

TEST test_strview_find_first_nocase(void)
{
	strview_t str1, str2, search_result;

	str1 = cstr("nEedle");
	str2 = cstr("neeDle");
	search_result = strview_find_first_nocase(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("Needle");
	search_result = strview_find_first_nocase(str1, "needlE");
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("neEDleneEdlE");
	str2 = cstr("Needle");
	search_result = strview_find_first_nocase(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needLneedLe");
	str2 = cstr("needle");
	search_result = strview_find_first_nocase(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 5);
	PASS();
}

TEST test_strview_find_first_edge_cases(void)
{
	strview_t str1, str2, search_result;

	str1 = cstr("haystack");
	str2 = cstr("");
	search_result = strview_find_first(str1, str2);
	ASSERT(strview_is_valid(search_result));		// a valid needle of length 0 should be found
	ASSERT((search_result.data - str1.data) == 0);	// at the start

	str2 = STRVIEW_INVALID;
	search_result = strview_find_first(str1, str2);
	ASSERT(!strview_is_valid(search_result));		// a needle which is an invalid string should not be found
	
	str1 = STRVIEW_INVALID;
	str2 = cstr("");
	search_result = strview_find_first(str1, str2);
	ASSERT(!strview_is_valid(search_result));		// a needle in an invalid haystack should not be found

	str1 = cstr("");
	str2 = cstr("");
	search_result = strview_find_first(str1, str2);
	ASSERT(strview_is_valid(search_result));		//a valid needle of length 0 should be found in a valid haystack of length 0
	ASSERT((search_result.data - str1.data) == 0);	//at the start

	PASS();
}

TEST test_strview_find_last(void)
{
	strview_t str1, str2, search_result;

	str1 = cstr("needle");
	str2 = cstr("needle");
	search_result = strview_find_last(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needle");
	search_result = strview_find_last(str1, "needle");
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needleneedle");
	str2 = cstr("needle");
	search_result = strview_find_last(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 6);

	str1 = cstr("needlneedle");
	str2 = cstr("needle");
	search_result = strview_find_last(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 5);

	PASS();
}

TEST test_strview_find_last_nocase(void)
{
	strview_t str1, str2, search_result;

	str1 = cstr("needle");
	str2 = cstr("needlE");
	search_result = strview_find_last_nocase(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("neeDle");
	search_result = strview_find_last_nocase(str1, "nEedle");
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 0);

	str1 = cstr("needleNeedle");
	str2 = cstr("needle");
	search_result = strview_find_last_nocase(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 6);

	str1 = cstr("needlneedle");
	str2 = cstr("needLe");
	search_result = strview_find_last_nocase(str1, str2);
	ASSERT(strview_is_valid(search_result));
	ASSERT((search_result.data - str1.data) == 5);

	PASS();
}

TEST test_strview_find_last_edge_cases(void)
{
	strview_t str1, str2, search_result;

	str1 = cstr("haystack");
	str2 = cstr("");
	search_result = strview_find_last(str1, str2);
	ASSERT(strview_is_valid(search_result)); 		// a valid needle of length 0 should be found at the index of the last char+1
	ASSERT((search_result.data - str1.data) == 8);

	str2 = STRVIEW_INVALID;
	search_result = strview_find_last(str1, str2);
	ASSERT(!strview_is_valid(search_result));		// a needle which is an invalid string should not be found

	str1 = STRVIEW_INVALID;
	str2 = cstr("");
	search_result = strview_find_first(str1, str2);
	ASSERT(!strview_is_valid(search_result));		// a needle in an invalid haystack should not be found

	str1 = cstr("");
	str2 = cstr("");
	search_result = strview_find_first(str1, str2);
	ASSERT(strview_is_valid(search_result));		// a valid needle of length 0 should be found at the index of the last char+1
	ASSERT((search_result.data - str1.data) == 0);

	PASS();
}

TEST test_strview_is_valid(void)
{
	strview_t str1 = STRVIEW_INVALID;
	ASSERT(!strview_is_valid(str1));
	str1 = cstr("valid");
	ASSERT(strview_is_valid(str1));

	PASS();
}

TEST test_strview_append_char(void)
{
	strview_t str1;
	strbuf_t* buf = strbuf_create(0, NULL);
	const char* chrptr = "THE QUICK BROWN FOX JUMPES OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!";
	ASSERT(buf);
	while(*chrptr)
		strbuf_append_char(&buf, *chrptr++);

	str1 = strbuf_view(&buf);
	ASSERT(!memcmp("THE QUICK BROWN FOX JUMPES OVER THE LAZY DOG. CONGRATULATIONS, YOUR TYPEWRITER WORKS!", str1.data, str1.size));

	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
}

TEST test_strbuf_shrink(void)
{
	strbuf_t* buf = strbuf_create(200, NULL);

	strbuf_assign(&buf, cstr("hello-test"));
	strbuf_shrink(&buf);
	ASSERT(buf->size == 10);
	ASSERT(!memcmp("hello-test", buf->cstr, 10));
	ASSERT(buf->capacity == 10);

	strbuf_destroy(&buf);
	ASSERT(!buf);

	buf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);
	ASSERT(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t) - 1);
	strbuf_assign(&buf, cstr("hello-test"));
	strbuf_shrink(&buf);
	ASSERT(buf->size == 10);
	ASSERT(!memcmp("hello-test", buf->cstr, 10));
	ASSERT(buf->capacity == STATIC_BUFFER_SIZE - sizeof(strbuf_t) - 1);	//strbuf_shrink should not change a static buffers capacity

	strbuf_destroy(&buf);
	ASSERT(!buf);

	PASS();
}

TEST test_strview_is_match(void)
{
	ASSERT(strview_is_match(cstr("Hello"), cstr("Hello")));
	ASSERT(strview_is_match(STRVIEW_INVALID, STRVIEW_INVALID));
	ASSERT(strview_is_match(cstr("Hello"), "Hello"));
	ASSERT(strview_is_match(STRVIEW_INVALID, (const char*)NULL));

	ASSERT(!strview_is_match(cstr("Hello"), cstr("HellO")));
	ASSERT(!strview_is_match(cstr("Hello"), cstr("Hell")));
	ASSERT(!strview_is_match(cstr("Hell"), cstr("Hello")));
	ASSERT(!strview_is_match(cstr("Hello"), STRVIEW_INVALID));
	ASSERT(!strview_is_match(STRVIEW_INVALID, cstr("Hello")));
	PASS();
}

TEST test_strview_is_match_nocase(void)
{
	ASSERT(strview_is_match_nocase(cstr("Hello"), cstr("Hello")));
	ASSERT(strview_is_match_nocase(STRVIEW_INVALID, STRVIEW_INVALID));
	ASSERT(strview_is_match_nocase(cstr("Hello"), cstr("HellO")));
	ASSERT(strview_is_match_nocase(cstr("Hello"), cstr("hello")));

	ASSERT(strview_is_match_nocase(cstr("Hello"), "Hello"));
	ASSERT(strview_is_match_nocase(STRVIEW_INVALID, (const char*)NULL));
	ASSERT(strview_is_match_nocase(cstr("Hello"), "HellO"));
	ASSERT(strview_is_match_nocase(cstr("Hello"), "hello"));

	ASSERT(!strview_is_match_nocase(cstr("Hello"), cstr("Hell")));
	ASSERT(!strview_is_match_nocase(cstr("Hell"), cstr("Hello")));
	ASSERT(!strview_is_match_nocase(cstr("Hello"), STRVIEW_INVALID));
	ASSERT(!strview_is_match_nocase(STRVIEW_INVALID, cstr("Hello")));
	PASS();
}

TEST test_strview_starts_with(void)
{
	ASSERT(strview_starts_with(cstr("Hello..."), cstr("Hello")));
	ASSERT(strview_starts_with(STRVIEW_INVALID, STRVIEW_INVALID));
	ASSERT(!strview_starts_with(cstr("Hello.."), cstr("Hello...")));
	ASSERT(!strview_starts_with(cstr("Hello"), STRVIEW_INVALID));
	ASSERT(!strview_starts_with(STRVIEW_INVALID, cstr("Hello")));

	ASSERT(strview_starts_with(cstr("Hello..."), "Hello"));
	ASSERT(strview_starts_with(STRVIEW_INVALID, (const char*)NULL));
	ASSERT(!strview_starts_with(cstr("Hello.."), "Hello..."));
	ASSERT(!strview_starts_with(cstr("Hello"), (const char*)NULL));
	ASSERT(!strview_starts_with(STRVIEW_INVALID, "Hello"));

	PASS();
}

TEST test_strview_starts_with_nocase(void)
{
	ASSERT(strview_starts_with_nocase(cstr("hELlo..."), cstr("Hello")));
	ASSERT(strview_starts_with_nocase(STRVIEW_INVALID, STRVIEW_INVALID));
	ASSERT(!strview_starts_with_nocase(cstr("Hello.."), cstr("Hello...")));
	ASSERT(!strview_starts_with_nocase(cstr("Hello"), STRVIEW_INVALID));
	ASSERT(!strview_starts_with_nocase(STRVIEW_INVALID, cstr("Hello")));

	ASSERT(strview_starts_with_nocase(cstr("hELlo..."), "Hello"));
	ASSERT(strview_starts_with_nocase(STRVIEW_INVALID, (const char*)NULL));
	ASSERT(!strview_starts_with_nocase(cstr("Hello.."), "Hello..."));
	ASSERT(!strview_starts_with_nocase(cstr("Hello"), (const char*)NULL));
	ASSERT(!strview_starts_with_nocase(STRVIEW_INVALID, "Hello"));

	PASS();
}

TEST test_strview_trim_start(void)
{
	strview_t str1, str2;

	str1 = cstr("/.;/hello;./;.");
	str2 = strview_trim_start(str1, cstr(";./"));
	ASSERT(!memcmp("hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, cstr("/;"));
	ASSERT(!memcmp(".;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, cstr("/"));
	ASSERT(!memcmp(".;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, cstr(""));			//trim nothing
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, STRVIEW_INVALID);	//trim nothing
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	str1 = cstr("/.;/hello;./;.");
	str2 = strview_trim_start(str1, ";./");
	ASSERT(!memcmp("hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, "/;");
	ASSERT(!memcmp(".;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, "/");
	ASSERT(!memcmp(".;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_start(str1, "");				//trim nothing
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	PASS();
}

TEST test_strview_trim_end(void)
{
	strview_t str1, str2;

	str1 = cstr("/.;/hello;./;.");
	str2 = strview_trim_end(str1, cstr(";./"));
	ASSERT(!memcmp("/.;/hello", str2.data, str2.size));

	str2 = strview_trim_end(str1, cstr(";."));
	ASSERT(!memcmp("/.;/hello;./", str2.data, str2.size));

	str2 = strview_trim_end(str1, cstr(";"));
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_end(str1, cstr(""));	//trim nothing
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_end(str1, STRVIEW_INVALID);	//trim nothing
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	str1 = cstr("/.;/hello;./;.");
	str2 = strview_trim_end(str1, ";./");
	ASSERT(!memcmp("/.;/hello", str2.data, str2.size));

	str2 = strview_trim_end(str1, ";.");
	ASSERT(!memcmp("/.;/hello;./", str2.data, str2.size));

	str2 = strview_trim_end(str1, ";");
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	str2 = strview_trim_end(str1, "");	//trim nothing
	ASSERT(!memcmp("/.;/hello;./;.", str2.data, str2.size));

	PASS();
}

TEST test_strview_trim(void)
{
	strview_t str1, str2;

	str1 = cstr("123hello321");
	str2 = strview_trim(str1, cstr("132"));
	ASSERT(!memcmp("hello", str2.data, str2.size));

	str1 = cstr("123hello321");
	str2 = strview_trim(str1, "132");
	ASSERT(!memcmp("hello", str2.data, str2.size));
	PASS();
}

TEST test_strview_to_cstr(void)
{
	// Testing various edge cases of strview_to_cstr()
	static_buf[0] = 0x7F;

	strview_to_cstr(static_buf, 0, cstr_SL("string"));	//don't write anything
	ASSERT(static_buf[0] == 0x7F);

	strview_to_cstr(static_buf, 1, cstr_SL("string"));	//write null terminator only
	ASSERT(static_buf[0] == 0);

	strview_to_cstr(static_buf, 4, cstr_SL("string"));
	ASSERT(!memcmp(static_buf, "str", 4));

	strview_to_cstr(static_buf, 4, cstr_SL("str"));
	ASSERT(!memcmp(static_buf, "str", 4));

	strview_to_cstr(static_buf, STATIC_BUFFER_SIZE, cstr_SL("string"));
	ASSERT(!memcmp(static_buf, "string", sizeof("string")));
	PASS();
}

TEST test_strview_compare(void)
{
	strview_t str1 = STRVIEW_INVALID;

	ASSERT(strview_compare(cstr("abd"), cstr("abc")) > 0);
	ASSERT(strview_compare(cstr("aba"), cstr("abd")) < 0);
	ASSERT(strview_compare(cstr("abc"), cstr("abc")) == 0);
	ASSERT(strview_compare(cstr("abca"), cstr("abc")) > 0);
	ASSERT(strview_compare(cstr("abc"), cstr("abca")) < 0);
	ASSERT(!strview_compare(cstr(""), str1));	//an empty string == an invalid string for strview_compare();

	PASS();
}

TEST test_strview_split_index(void)
{
	strview_t str1, str2;

	str1 = cstr("123");
	str2 = strview_split_index(&str1, 1);
	ASSERT(str1.size == 2);
	ASSERT(str2.size == 1);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str1.data, "23", str1.size));
	ASSERT(!memcmp(str2.data, "1", str2.size));

	str1 = cstr("123");
	str2 = strview_split_index(&str1, -1);
	ASSERT(str1.size == 2);
	ASSERT(str2.size == 1);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str1.data, "12", str1.size));
	ASSERT(!memcmp(str2.data, "3", str2.size));

	str1 = cstr("123");
	str2 = strview_split_index(&str1, 0);
	ASSERT(str1.size == 3);
	ASSERT(str2.size == 0);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str1.data, "123", str1.size));
	ASSERT(!memcmp(str2.data, "", str2.size));

	str1 = cstr("123");
	str2 = strview_split_index(&str1, -3);
	ASSERT(str1.size == 0);
	ASSERT(str2.size == 3);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str1.data, "", str1.size));
	ASSERT(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = strview_split_index(&str1, 3);
	ASSERT(str1.size == 0);
	ASSERT(str2.size == 3);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str1.data, "", str1.size));
	ASSERT(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = strview_split_index(&str1, 4);	//over range, pops ALL
	ASSERT(str1.size == 0);
	ASSERT(str2.size == 3);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str2.data, "123", str2.size));

	str1 = cstr("123");
	str2 = strview_split_index(&str1, -4);	//out of range, pops all
	ASSERT(str1.size == 0);
	ASSERT(str2.size == 3);
	ASSERT(strview_is_valid(str1));
	ASSERT(strview_is_valid(str2));
	ASSERT(!memcmp(str2.data, "123", str2.size));

	PASS();
}

TEST test_strbuf_printf(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	str1 = strbuf_printf(&buf, "Hello from printf! have some numbers... %i %i %i %i %i %i %i", 6246456, 3466765, 435234, 4598756, 94572, 69, 42597);

	ASSERT(!memcmp(str1.data, "Hello from printf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_append_printf(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	strbuf_assign(&buf, cstr("Hello"));
	str1 = strbuf_append_printf(&buf, " Appending one more number %i", 748921);
	ASSERT(!memcmp(str1.data, "Hello Appending one more number 748921", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_prnf(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	str1 = strbuf_prnf(&buf, "Hello from prnf! have some numbers... %i %i %i %i %i %i %i", 6246456, 3466765, 435234, 4598756, 94572, 69, 42597);

	ASSERT(!memcmp(str1.data, "Hello from prnf! have some numbers... 6246456 3466765 435234 4598756 94572 69 42597", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_append_prnf(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	strbuf_assign(&buf, cstr("Hello"));
	str1 = strbuf_append_prnf(&buf, " Appending one more number %i", 748921);
	ASSERT(!memcmp(str1.data, "Hello Appending one more number 748921", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	//assign 240 bytes
	strbuf_assign(&buf, cstr("\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF\
0123456789ABCDEF"));

	//append 16
	strbuf_append_printf(&buf, "012345670123456~");

	ASSERT(buf->cstr[255] == '~');

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_assign(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	// Testing strbuf_assign() source outside of the destination
	str1 = strbuf_assign(&buf, cstr("***Hello test***"));
	ASSERT(!memcmp(str1.data, "***Hello test***", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	// Testing strbuf_assign() source from the destination
	strbuf_assign(&buf, strview_trim(strbuf_view(&buf), cstr("*")));
	str1 = strbuf_assign(&buf, cstr("Hello test"));
	ASSERT(!memcmp(str1.data, "Hello test", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_append(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	// Testing strbuf_append(), with source from the destination
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_append(&buf, strbuf_view(&buf));
	ASSERT(!memcmp(str1.data, "{Hello-testing-some-string}{Hello-testing-some-string}", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	// Testing strbuf_append()
	str1 = strbuf_assign(&buf, cstr("{Hello-Fred}"));
	strbuf_shrink(&buf);
	str1 = strbuf_append(&buf, "{Hello-Bob}");
	ASSERT(!memcmp(str1.data, "{Hello-Fred}{Hello-Bob}", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_prepend(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	// Testing strbuf_prepend(), with source from the destination
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_prepend(&buf, strbuf_view(&buf));
	ASSERT(!memcmp(str1.data, "{Hello-testing-some-string}{Hello-testing-some-string}", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	// Testing strbuf_prepend()
	str1 = strbuf_assign(&buf, cstr("{Hello-Fred}"));
	strbuf_shrink(&buf);
	str1 = strbuf_prepend(&buf, "{Hello-Bob}");
	ASSERT(!memcmp(str1.data, "{Hello-Bob}{Hello-Fred}", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_insert_at_index(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	// Testing strbuf_insert_at_index(), with source from the destination
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_insert_at_index(&buf, 6, strbuf_view(&buf));
	ASSERT(!memcmp(str1.data, "{Hello{Hello-testing-some-string}-testing-some-string}", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	// Testing strbuf_insert_at_index(), with source from the destination
	str1 = strbuf_assign(&buf, cstr("{Hello-testing-some-string}"));
	strbuf_shrink(&buf);
	str1 = strbuf_insert_at_index(&buf, 6, "(random text)");
	ASSERT(!memcmp(str1.data, "{Hello(random text)-testing-some-string}", str1.size));
	ASSERT(buf->cstr[buf->size] == 0);	//check the 0 terminator is in place

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strview_split_line(void)
{
	#define TEST_LINE_POP(arg1)								\
	do{														\
		str2.data = NULL; str2.size = 0;					\
		while(!strview_is_valid(str2))						\
		{													\
			strbuf_append_char(&buf, *chrptr++);			\
			str1 = strbuf_view(&buf);						\
			str2 = strview_split_line(&str1, &eol);			\
		};													\
		ASSERT(!memcmp(str2.data, arg1, str1.size));		\
		strbuf_assign(&buf, str1);							\
	}while(0)

	strview_t str1, str2;
	strbuf_t* buf;
	char eol = 0;
	const char* chrptr;

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

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	str2 = cstr(sometext);
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "First line CRLF", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "Second line CR", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "Third line LF", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "CRLF line followed by an empty CR line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "CRLF line followed by an empty LF line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "CRLF line followed by an empty LFCR line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "LFCR line followed by an empty CR line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "LFCR line followed by an empty LF line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "LFCR line followed by an empty CRLF line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "CR line followed by an empty CRLF line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "CR line followed by an empty LFCR line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "LF line followed by an empty CRLF line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "LF line followed by an empty LFCR line", str1.size));
	str1 = strview_split_line(&str2, NULL); ASSERT(strview_is_valid(str1)); ASSERT(!memcmp(str1.data, "", str1.size));

	str1 = strview_split_line(&str2, NULL); ASSERT(!strview_is_valid(str1)); ASSERT(!memcmp(str2.data, "This text has no line ending", str1.size));

	// Testing line parser by appending chars to a buffer 1 at a time
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

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_insert_before(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	strbuf_assign(&buf, cstr("Hello"));
	str1 = strview_find_last(strbuf_view(&buf), cstr(""));
	strbuf_insert_before(&buf, str1, cstr("-test"));
	ASSERT(!memcmp(buf->cstr, "Hello-test", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_last(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_before(&buf, str1, cstr("..."));
	ASSERT(!memcmp(buf->cstr, "Hello Me...llow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_before(&buf, str1, cstr("---"));
	ASSERT(!memcmp(buf->cstr, "He---llo Mellow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr(""));
	strbuf_insert_before(&buf, str1, cstr("***"));
	ASSERT(!memcmp(buf->cstr, "***Hello Mellow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_last(strbuf_view(&buf), "ll");
	strbuf_insert_before(&buf, str1, "...");
	ASSERT(!memcmp(buf->cstr, "Hello Me...llow", buf->size));

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_insert_after(void)
{
	strbuf_t* buf;
	strview_t str1;

	buf = strbuf_create(0, NULL);
	ASSERT(buf);

	strbuf_assign(&buf, cstr("Hello"));
	str1 = strview_find_last(strbuf_view(&buf), cstr(""));
	strbuf_insert_after(&buf, str1, cstr("-test"));
	ASSERT(!memcmp(buf->cstr, "Hello-test", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_last(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_after(&buf, str1, cstr("..."));
	ASSERT(!memcmp(buf->cstr, "Hello Mell...ow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr("ll"));
	strbuf_insert_after(&buf, str1, cstr("---"));
	ASSERT(!memcmp(buf->cstr, "Hell---o Mellow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_first(strbuf_view(&buf), cstr(""));
	strbuf_insert_after(&buf, str1, cstr("***"));
	ASSERT(!memcmp(buf->cstr, "***Hello Mellow", buf->size));

	strbuf_assign(&buf, cstr("Hello Mellow"));
	str1 = strview_find_last(strbuf_view(&buf), "ll");
	strbuf_insert_after(&buf, str1, "...");
	ASSERT(!memcmp(buf->cstr, "Hello Mell...ow", buf->size));

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}

TEST test_strbuf_to_cstr(void)
{
	strbuf_t* dbuf = strbuf_create(0, NULL);
	strbuf_t* sbuf = strbuf_create_fixed(static_buf, STATIC_BUFFER_SIZE);
	char* dstr;
	char* sstr;

	strbuf_assign(&dbuf, cstr("Some test string in a dynamic buffer, hello test."));
	strbuf_assign(&sbuf, cstr("Some test string in a static buffer, hello test."));

	dstr = strbuf_to_cstr(&dbuf);
	sstr = strbuf_to_cstr(&sbuf);

	ASSERT(dbuf == NULL);
	ASSERT(sbuf == NULL);

	ASSERT(!strcmp(dstr, "Some test string in a dynamic buffer, hello test."));
	ASSERT(!strcmp(sstr, "Some test string in a static buffer, hello test."));

	free(dstr);	// this should not segfault or leak memory.
	PASS();
}

TEST test_strbuf_terminate_views(void)
{
	strbuf_t* dbuf = strbuf_create(0, NULL);
	strbuf_t* sbuf = strbuf_create_fixed(static_buf, 8+sizeof(strbuf_t));
	strview_t view[3];
	strview_t result;
	strview_t src;

//	in		AAA
//	out		AAA.
	view[0] = strbuf_assign(&dbuf, cstr("AAA      "));
	view[0].size = 3;
	result = strbuf_terminate_views(&dbuf, 1, view);
	ASSERT(view[0].size == 4);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(dbuf->size == 4);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		(a single view of size 0)
//	out		.
	view[0] = strbuf_assign(&dbuf, cstr("      "));
	view[0].size = 0;
	result = strbuf_terminate_views(&dbuf, 1, view);
	ASSERT(view[0].size == 1);
	ASSERT(view[0].data[0] == 0);
	ASSERT(dbuf->size == 1);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAABBBCCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAABBBCCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 3, 6);
	view[2] = strview_sub(strbuf_view(&dbuf), 6, 9);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);


//	in		AAA.BBBCCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAA.BBBCCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 4, 7);
	view[2] = strview_sub(strbuf_view(&dbuf), 7, 10);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAA.BBB.CCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAA.BBB.CCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 4, 7);
	view[2] = strview_sub(strbuf_view(&dbuf), 8, 11);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAA.BBB..CCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAA.BBB..CCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 4, 7);
	view[2] = strview_sub(strbuf_view(&dbuf), 9, 12);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAA..BBB.CCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAA..BBB.CCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 5, 8);
	view[2] = strview_sub(strbuf_view(&dbuf), 9, 12);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAABBB...CCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAABBB...CCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 3, 6);
	view[2] = strview_sub(strbuf_view(&dbuf), 9, 12);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		.AAA.BBBCCC
//	out		AAA.BBB.CCC.
	strbuf_assign(&dbuf, cstr(".AAA.BBBCCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 1, 4);
	view[1] = strview_sub(strbuf_view(&dbuf), 5, 8);
	view[2] = strview_sub(strbuf_view(&dbuf), 8, 11);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 12);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAA	 (view B is of size 0 but still produces a terminator in the buffer)
//	out		AAA..
	strbuf_assign(&dbuf, cstr("AAA"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 3, 3);
	result = strbuf_terminate_views(&dbuf, 2, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, ""));
	ASSERT(dbuf->size == 5);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAABBB...CCC	(a is invalid view)
//	out		BBB.CCC.
	strbuf_assign(&dbuf, cstr("AAABBB...CCC"));
	view[0] = STRVIEW_INVALID;
	view[1] = strview_sub(strbuf_view(&dbuf), 3, 6);
	view[2] = strview_sub(strbuf_view(&dbuf), 9, 12);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 8);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);


//	in		AAABBB...CCC	(b is invalid view)
//	out		AAA.CCC.
	strbuf_assign(&dbuf, cstr("AAABBB...CCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = STRVIEW_INVALID;
	view[2] = strview_sub(strbuf_view(&dbuf), 9, 12);
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[2].data, "CCC"));
	ASSERT(dbuf->size == 8);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAABBB...CCC	(c is invalid view)
//	out		AAA.BBB.
	strbuf_assign(&dbuf, cstr("AAABBB...CCC"));
	view[0] = strview_sub(strbuf_view(&dbuf), 0, 3);
	view[1] = strview_sub(strbuf_view(&dbuf), 3, 6);
	view[2] = STRVIEW_INVALID;
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[0].data, "AAA"));
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(dbuf->size == 8);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAABBB...CCC	(only B is a valid view)
//	out		BBB.
	strbuf_assign(&dbuf, cstr("AAABBB...CCC"));
	view[0] = STRVIEW_INVALID;
	view[1] = strview_sub(strbuf_view(&dbuf), 3, 6);
	view[2] = STRVIEW_INVALID;
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(!strcmp(view[1].data, "BBB"));
	ASSERT(dbuf->size == 4);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	in		AAABBB...CCC	(all views invalid)
//	out		<empty buffer>
	strbuf_assign(&dbuf, cstr("AAABBB...CCC"));
	view[0] = STRVIEW_INVALID;
	view[1] = STRVIEW_INVALID;
	view[2] = STRVIEW_INVALID;
	result = strbuf_terminate_views(&dbuf, 3, view);
	ASSERT(dbuf->size == 0);
	ASSERT(result.data == dbuf->cstr);
	ASSERT(result.size == dbuf->size);

//	Try an operation on a static buffer that just fits
	src = cstr("***********************************************************************************************");
	src.size = sbuf->capacity-1;
	strbuf_assign(&sbuf, src);
	view[0] = strbuf_view(&sbuf);
	result = strbuf_terminate_views(&sbuf, 1, view);
	ASSERT(strview_is_valid(result));
	ASSERT(strview_is_match(cstr(view[0].data), src));

//	Try an operation on a static buffer that does not fit
	src = cstr("***********************************************************************************************");
	src.size = sbuf->capacity;
	strbuf_assign(&sbuf, src);
	view[0] = strbuf_view(&sbuf);
	result = strbuf_terminate_views(&sbuf, 1, view);
	ASSERT(!strview_is_valid(result));
	ASSERT(sbuf->size == 0);
}

TEST test_strview_split_left(void)
{
	strview_t str1,str2,str3;

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr("testing"));
	str3 = strview_split_left(&str1, str2);
	ASSERT(strview_is_match(str3, cstr("Hello-")));
	ASSERT(strview_is_match(str1, cstr("testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str3 = strview_split_left(&str1, str2);
	ASSERT(strview_is_valid(str3)); ASSERT(str3.size == 0);
	ASSERT(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str3 = strview_split_left(&str1, str2);
	ASSERT(strview_is_valid(str1)); ASSERT(str1.size == 0);
	ASSERT(strview_is_match(str3, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str2.data--;
	str3 = strview_split_left(&str1, str2);
	ASSERT(!strview_is_valid(str3));
	ASSERT(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str2.data++;
	str3 = strview_split_left(&str1, str2);
	ASSERT(!strview_is_valid(str3));
	ASSERT(strview_is_match(str1, cstr("Hello-testing-123")));

	PASS();
}

TEST test_strview_split_right(void)
{
	strview_t str1,str2,str3;

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr("testing"));
	str3 = strview_split_right(&str1, str2);
	ASSERT(strview_is_match(str3, cstr("-123")));
	ASSERT(strview_is_match(str1, cstr("Hello-testing")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str3 = strview_split_right(&str1, str2);
	ASSERT(strview_is_valid(str1)); ASSERT(str1.size == 0);
	ASSERT(strview_is_match(str3, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str3 = strview_split_right(&str1, str2);
	ASSERT(strview_is_valid(str3)); ASSERT(str3.size == 0);
	ASSERT(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_first(str1, cstr(""));
	str2.data--;
	str3 = strview_split_right(&str1, str2);
	ASSERT(!strview_is_valid(str3));
	ASSERT(strview_is_match(str1, cstr("Hello-testing-123")));

	str1 = cstr("Hello-testing-123");
	str2 = strview_find_last(str1, cstr(""));
	str2.data++;
	str3 = strview_split_right(&str1, str2);
	ASSERT(!strview_is_valid(str3));
	ASSERT(strview_is_match(str1, cstr("Hello-testing-123")));

	PASS();
}

TEST test_strview_dequote(void)
{
	strview_t str1;

	str1 = strview_dequote(cstr("\"Fred\""));
	ASSERT(strview_is_match(str1, cstr("Fred")));

	str1 = strview_dequote(cstr("\'Fred\'"));
	ASSERT(strview_is_match(str1, cstr("Fred")));

	str1 = strview_dequote(cstr("\"\'Fred\'\""));
	ASSERT(strview_is_match(str1, cstr("'Fred'")));

	str1 = strview_dequote(cstr("\'\"Fred\"\'"));
	ASSERT(strview_is_match(str1, cstr("\"Fred\"")));

	str1 = strview_dequote(cstr("\'\'\"Fred\"\'\'"));
	ASSERT(strview_is_match(str1, cstr("\"Fred\"")));

	str1 = strview_dequote(cstr("\"\"\'Fred\'\"\""));
	ASSERT(strview_is_match(str1, cstr("'Fred'")));

	str1 = strview_dequote(cstr("\"\'Fred\"\'"));		//dont remove quotes that don't match ie. 'Fred"
	ASSERT(strview_is_match(str1, cstr("\"\'Fred\"\'")));

	str1 = strview_dequote(cstr(" \"Fred\""));			//don't trim space
	ASSERT(strview_is_match(str1, cstr(" \"Fred\"")));

	PASS();
}

TEST test_strview_contains(void)
{
	ASSERT(!strview_contains(cstr("bc..."), cstr("abc")));
	ASSERT( strview_contains(cstr("abc.."), cstr("abc")));
	ASSERT( strview_contains(cstr(".abc."), cstr("abc")));
	ASSERT(!strview_contains(cstr(".abc."), cstr("aBc")));
	ASSERT( strview_contains(cstr("..abc"), cstr("abc")));
	ASSERT(!strview_contains(cstr("...ab"), cstr("abc")));
	PASS();
}

TEST test_strview_contains_nocase(void)
{
	ASSERT(!strview_contains_nocase(cstr("bc..."), cstr("Abc")));
	ASSERT( strview_contains_nocase(cstr("abc.."), cstr("aBc")));
	ASSERT( strview_contains_nocase(cstr(".abc."), cstr("ABc")));
	ASSERT( strview_contains_nocase(cstr("..abc"), cstr("abC")));
	ASSERT(!strview_contains_nocase(cstr("...ab"), cstr("AbC")));
	PASS();
}

TEST test_strnum_value(void)
{
	strbuf_t* buf = strbuf_create(0,NULL);
	int err;
	strview_t v;
	unsigned char		iuchar;
	unsigned short		iushort;
	unsigned int		iuint;
	unsigned long		iulong;
	unsigned long long	iullong;
	char				ichar;
	short				ishort;
	int					iint;
	long				ilong;
	long long			illong;
	float				ifloat;
	double				idouble;
	long double			ildouble;

	ASSERT(buf);

	// Test all integer types at their limits
	#define TEST_AT_LIMIT(fmt, lim, var)				\
	do													\
	{													\
		v = strbuf_printf(&buf, fmt, (lim));			\
		err = strnum_value(&(var), &v, 0);				\
		ASSERT(err == 0);								\
		ASSERT(var == (lim));							\
	}while(0);											\

	TEST_AT_LIMIT("%u", UCHAR_MAX, iuchar);
	TEST_AT_LIMIT("%u", USHRT_MAX, iushort);
	TEST_AT_LIMIT("%u", UINT_MAX,  iuint);
	TEST_AT_LIMIT("%lu", ULONG_MAX, iulong);
	TEST_AT_LIMIT("%llu", ULLONG_MAX, iullong);
	TEST_AT_LIMIT("%i", CHAR_MIN, ichar);
	TEST_AT_LIMIT("%i", CHAR_MAX, ichar);
	TEST_AT_LIMIT("%i", SHRT_MIN, ishort);
	TEST_AT_LIMIT("%i", SHRT_MAX, ishort);
	TEST_AT_LIMIT("%i", INT_MIN, iint);
	TEST_AT_LIMIT("%i", INT_MAX, iint);
	TEST_AT_LIMIT("%li", LONG_MIN, ilong);
	TEST_AT_LIMIT("%li", LONG_MAX, ilong);
	TEST_AT_LIMIT("%lli", LLONG_MIN, illong);
	TEST_AT_LIMIT("%lli", LLONG_MAX, illong);

	//Test all integer types over their limits by 1
	//It just so happens that no relevant powers of 2 end in 9, so the last digit can always be incremented to exceed the limit
	#define TEST_OVER_LIMIT(fmt, lim, var)				\
	do													\
	{													\
		v = strbuf_printf(&buf, fmt, (lim));			\
		buf->cstr[buf->size-1]++;						\
		err = strnum_value(&(var), &v, 0);				\
		ASSERT(err == ERANGE);							\
	}while(0);											\

	TEST_OVER_LIMIT("%u", UCHAR_MAX, iuchar);
	TEST_OVER_LIMIT("%u", USHRT_MAX, iushort);
	TEST_OVER_LIMIT("%u", UINT_MAX,  iuint);
	TEST_OVER_LIMIT("%lu", ULONG_MAX, iulong);
	TEST_OVER_LIMIT("%llu", ULLONG_MAX, iullong);
	TEST_OVER_LIMIT("%i", CHAR_MIN, ichar);
	TEST_OVER_LIMIT("%i", CHAR_MAX, ichar);
	TEST_OVER_LIMIT("%i", SHRT_MIN, ishort);
	TEST_OVER_LIMIT("%i", SHRT_MAX, ishort);
	TEST_OVER_LIMIT("%i", INT_MIN, iint);
	TEST_OVER_LIMIT("%i", INT_MAX, iint);
	TEST_OVER_LIMIT("%li", LONG_MIN, ilong);
	TEST_OVER_LIMIT("%li", LONG_MAX, ilong);
	TEST_OVER_LIMIT("%lli", LLONG_MIN, illong);
	TEST_OVER_LIMIT("%lli", LLONG_MAX, illong);

	// check that white space is tolerated
	v = cstr(" 123");
	err = strnum_value(&iint, &v, 0);
	ASSERT(!err);
	ASSERT(iint == 123);
	// check that white space is not tolerated
	iint = 0;
	v = cstr(" 123");
	err = strnum_value(&iint, &v, STRNUM_NOSPACE);
	ASSERT(err == EINVAL);
	ASSERT(iint == 0);

	// test 0b 0B 0x 0X base prefix
	v = cstr("0b1001");
	err = strnum_value(&iint, &v, 0);
	ASSERT(!err);
	ASSERT(iint == 0b1001);
	v = cstr("0B1001");
	err = strnum_value(&iint, &v, 0);
	ASSERT(!err);
	ASSERT(iint == 0b1001);
	v = cstr("0x3fE1");
	err = strnum_value(&iint, &v, 0);
	ASSERT(!err);
	ASSERT(iint == 0x3FE1);
	v = cstr("0X3Fe1");
	err = strnum_value(&iint, &v, 0);
	ASSERT(!err);
	ASSERT(iint == 0x3FE1);

	// test rejection of 0b 0B 0x 0X base prefix
	v = cstr("0b1001");
	iint = 1;
	err = strnum_value(&iint, &v, STRNUM_NOBX);
	ASSERT(!err);
	ASSERT(iint == 0);
	ASSERT(strview_is_match(v, cstr("b1001")));
	v = cstr("0B1001");
	iint = 1;
	err = strnum_value(&iint, &v, STRNUM_NOBX);
	ASSERT(!err);
	ASSERT(iint == 0);
	ASSERT(strview_is_match(v, cstr("B1001")));
	v = cstr("0x3F711");
	iint = 1;
	err = strnum_value(&iint, &v, STRNUM_NOBX);
	ASSERT(!err);
	ASSERT(iint == 0);
	ASSERT(strview_is_match(v, cstr("x3F711")));
	v = cstr("0x3F711");
	iint = 1;
	err = strnum_value(&iint, &v, STRNUM_NOBX);
	ASSERT(!err);
	ASSERT(iint == 0);
	ASSERT(strview_is_match(v, cstr("x3F711")));

	// test rejection of sign character
	v = cstr("+123");
	iint = 0;
	err = strnum_value(&iint, &v, STRNUM_NOSIGN);
	ASSERT(err == EINVAL);
	ASSERT(iint == 0);
	ASSERT(strview_is_match(v, cstr("+123")));

	// test binary digits without prefix
	v = cstr("1001");
	iint = 0;
	err = strnum_value(&iint, &v, STRNUM_BASE_BIN);
	ASSERT(!err);
	ASSERT(iint == 0b1001);

	// test binary digits with 0b prefix
	v = cstr("0b1001");
	iint = 0;
	err = strnum_value(&iint, &v, STRNUM_BASE_BIN);
	ASSERT(!err);
	ASSERT(iint == 0b1001);

	// test rejection of 0x when binary is expected
	v = cstr("0x13F");
	iint = 1;
	err = strnum_value(&iint, &v, STRNUM_BASE_BIN);
	ASSERT(!err);
	ASSERT(iint == 0);
	ASSERT(strview_is_match(v, cstr("x13F")));

	// test hex digits without prefix
	v = cstr("3eF1");
	iint = 0;
	err = strnum_value(&iint, &v, STRNUM_BASE_HEX);
	ASSERT(!err);
	ASSERT(iint == 0x3EF1);

	// test hex digits with prefix
	v = cstr("0x3eF1");
	iint = 0;
	err = strnum_value(&iint, &v, STRNUM_BASE_HEX);
	ASSERT(!err);
	ASSERT(iint == 0x3EF1);

	// test interpretation of 0b as hex digits when hex is expected
	v = cstr("0b100");
	iint = 0;
	err = strnum_value(&iint, &v, STRNUM_BASE_HEX);
	ASSERT(!err);
	ASSERT(iint == 0xb100);

	// test float types close to their limits
	v = strbuf_printf(&buf, "%Le", (long double)(__FLT_MAX__ * 0.998));
	err = strnum_value(&ifloat, &v, 0);
	ASSERT(!err);
	ASSERT(__FLT_MAX__*0.997 < ifloat && ifloat < __FLT_MAX__ * 0.999);
	v = strbuf_printf(&buf, "%Le", (long double)(__DBL_MAX__ * 0.998));
	err = strnum_value(&idouble, &v, 0);
	ASSERT(!err);
	ASSERT(__DBL_MAX__*0.997 < idouble && idouble < __DBL_MAX__ * 0.999);
	v = strbuf_printf(&buf, "%Le", (long double)(__LDBL_MAX__ * 0.998));
	err = strnum_value(&ildouble, &v, 0);
	ASSERT(!err);
	ASSERT(__LDBL_MAX__*0.997 < ildouble && ildouble < __LDBL_MAX__ * 0.999);
	v = strbuf_printf(&buf, "%Le", (long double)(__FLT_MIN__ * 0.998));
	err = strnum_value(&ifloat, &v, 0);
	ASSERT(!err);
	ASSERT(__FLT_MIN__*0.997 < ifloat && ifloat < __FLT_MIN__ * 0.999);
	v = strbuf_printf(&buf, "%Le", (long double)(__DBL_MIN__ * 0.998));
	err = strnum_value(&idouble, &v, 0);
	ASSERT(!err);
	ASSERT(__DBL_MIN__*0.997 < idouble && idouble < __DBL_MIN__ * 0.999);
	v = strbuf_printf(&buf, "%Le", (long double)(__LDBL_MIN__ * 0.998));
	err = strnum_value(&ildouble, &v, 0);
	ASSERT(!err);
	ASSERT(__LDBL_MIN__*0.997 < ildouble && ildouble < __LDBL_MIN__ * 0.999);

	// test rejection of exponent
	v = cstr("3.17E2");
	ifloat = 0.0;
	err = strnum_value(&ifloat, &v, STRNUM_NOEXP);
	ASSERT(!err);
	ASSERT(3.1699 < ifloat && ifloat < 3.1701);
	ASSERT(strview_is_match(v, cstr("E2")));

	strbuf_destroy(&buf);
	ASSERT(!buf);
	PASS();
}