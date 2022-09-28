// https://www.youtube.com/watch?v=QpAhX-gsHMs&t=3009s

	#include <stddef.h>
	#include <stdbool.h>
	#include <stdarg.h>
	#include <string.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//	The minimum expansion that can occur when re-allocating a buffers memory.
//	Higher values will reduce calls to the allocator, at the expense of more memory overhead.
	#ifndef STR_CAPACITY_GROW_STEP
	 	#define STR_CAPACITY_GROW_STEP 16
	#endif


//	These macros can be used with printf for printing str types using dynamic precision.
//	eg.  printf("name=%"PRIstr"\n", PRIstrarg(myname));
	#define PRIstr	".*s"
	#define PRIstrarg(arg)	(int)((arg).size),((arg).data)


//	This is used for counting the number of arguments to the str_buf_cat() macro below.
// 	From https://stackoverflow.com/questions/4421681/how-to-count-the-number-of-arguments-passed-to-a-function-that-accepts-a-variabl

	#define		PP_NARG(...) 	PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
	#define 	PP_NARG_(...) 	PP_128TH_ARG(__VA_ARGS__)

	#define PP_128TH_ARG( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
         _71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
         _81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
         _91,_92,_93,_94,_95,_96,_97,_98,_99,_100, \
         _101,_102,_103,_104,_105,_106,_107,_108,_109,_110, \
         _111,_112,_113,_114,_115,_116,_117,_118,_119,_120, \
         _121,_122,_123,_124,_125,_126,_127,N,...) N

	#define PP_RSEQ_N() \
         127,126,125,124,123,122,121,120, \
         119,118,117,116,115,114,113,112,111,110, \
         109,108,107,106,105,104,103,102,101,100, \
         99,98,97,96,95,94,93,92,91,90, \
         89,88,87,86,85,84,83,82,81,80, \
         79,78,77,76,75,74,73,72,71,70, \
         69,68,67,66,65,64,63,62,61,60, \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

/*	Use str_buf_cat to concatenate an arbitrary number of strings into a buffer.
	The buffers contents itself may be used as an argument.
	This facilitates appending, prepending, or even inserting by using str_sub().
	A string representing the result is returned. The string returned is always valid providing buf_ptr is not NULL.
	Example to append to a buffer:  str_buf_cat(&mybuffer, str_buf_str(&mybuffer), str_to_append) */
	#define str_buf_cat(buf_ptr, ...) _str_buf_cat(buf_ptr, PP_NARG(__VA_ARGS__), __VA_ARGS__)

/*	Structure for providing the buffer with an allocator.
	When a new allocation is required, ptr_to_free will be NULL and size will be >0
	When a re-allocation is required,  ptr_to_free will be not be NULL and size will be >0
	When a de-allocation is required,  ptr_to_free may or may not be NULL, and size will be 0
	stdlib's realloc handles this perfectly, example:

static void* allocator(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line)
{
	(void)this_allocator; (void)caller_filename; (void)caller_line;
	void* result;
	result = realloc(ptr_to_free, size);
	assert(size==0 || result);	// You need to catch a failed allocation here.
	return result;
}

then:
	str_allocator_t str_allocator = {.allocator = allocator};

*/
	typedef struct str_allocator_t
	{
		void* app_data;
		void* (*allocator)(struct str_allocator_t* this_allocator, void* ptr_to_free, size_t size, const char* caller_filename, int caller_line);
	} str_allocator_t;


//	String structure.
//	This does not own the memory used to hold the string. It references data either in a string buffer, or const chars in the string pool.
	typedef struct str_t
	{
		const char* data;
		size_t size;
	} str_t;


//	Buffer structure. Allocates and owns the buffer memory.
//	Maintains null termination, so classic c style (null terminated) strings can be accessed with .cstr
	typedef struct str_buf_t
	{
		size_t size;
		size_t capacity;
		str_allocator_t allocator;
		char cstr[];
	} str_buf_t;

//	Search result
	typedef struct str_search_result_t
	{
		bool found;
		size_t index;
	} str_search_result_t;

//	Can be used instead of cstr, to avoid measuring the length of string literals at runtime
	#define cstr_SL(sl_arg) ((str_t){.data=(sl_arg), .size=sizeof(sl_arg)-1})

//********************************************************************************************************
// Public variables
//********************************************************************************************************


//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	Create a new buffer and return it.
	str_buf_t* str_buf_create(size_t initial_capacity, str_allocator_t allocator);

/*	Concatenate one or more str_t and assign the result to the buffer.
	Ideally should be used from the macro str_buf_cat() which performs the argument counting for you.
	Returns a str_t of the buffer contents.
	The returned str_t is always valid, even if the none of the arguments are valid */
	str_t _str_buf_cat(str_buf_t** buf_ptr, int n_args, ...);

//	The non-variadic version of _str_buf_cat
	str_t str_buf_vcat(str_buf_t** buf_ptr, int n_args, va_list va);

//	Return a str_t of the buffer contents
	str_t str_buf_str(str_buf_t** buf_ptr);

//	Append a single character to the buffer, and return a str_t of the buffer contents
	str_t str_buf_append_char(str_buf_t** buf_ptr, char c);

//	Shrink the buffers capacity to the minimum required to hold it's contents.
//	No other buffer operations will perform this, they can only expand the buffer.
	str_t str_buf_shrink(str_buf_t** buf_ptr);

//	Calls the allocators deallocate() to free the buffers memory
	void str_buf_destroy(str_buf_t** buf_ptr);

//	Return a str_t from a null terminated const char string.
//	If the argument is a string literal, cstr_SL() may be used instead, to prevent traversing the string literal to measure it's length
	str_t cstr(const char* c_str);

//	Return true if the str_t is valid.
// 	A str_t may be invalid if a delimiter is not found when attempting to split a string with str_pop_first_split() or str_pop_last_split()
	bool str_is_valid(str_t str);

//	Return true if the strings match
	bool str_is_match(str_t str1, str_t str2);

/*	Return the sub string indexed by begin->end. end is non-inclusive.
	Negative values may be used, and will index from the end of the string backwards.
	The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string */
	str_t str_sub(str_t str, int begin, int end);

//	Return a str_t with the start and end trimmed of all characters present in chars_to_trim
	str_t str_trim(str_t str, str_t chars_to_trim);

/*	Return the search result (bool found & index) for the first occurrence of needle in haystack
	If needle is valid, and of length 0, it will always be found at the start of the string.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	str_search_result_t str_find_first(str_t haystack, str_t needle);

/*	Return the search result (bool found & index) for the last occurrence of needle in haystack
	If needle is valid, and of length 0, it will always be found at the index of the last character in haystack+1.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	str_search_result_t str_find_last(str_t haystack, str_t needle);

/*	Return a str_t representing the contents of the source string up to, but not including, any of the delimiters.
	Additionally this text, and the delimeter itself is removed (popped) from the source string.
	If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid() */
	str_t str_pop_first_split(str_t* str_ptr, str_t delimiters);

/*	Return a str_t representing the contents of the source string from (but not including) the last delimiter found.
	Additionally this text, and the delimeter itself is removed (popped) from the end of the source string.
	If no delimeter is found, the returned string is invalid, and should be tested with str_is_valid() */
	str_t str_pop_last_split(str_t* str_ptr, str_t delimiters);

