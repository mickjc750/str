/*/
*/

#ifndef _STRVIEW_H_
	#define _STRVIEW_H_

	#include <stddef.h>
	#include <stdbool.h>
	#include <stdarg.h>
	#include <string.h>
	#include <errno.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//	Options flags for number conversions

//	Accept only binary digits, with an optional 0b or 0B prefix
	#define STR_BASE_BIN	(1<<0)

//	Accept only hex digits, with an optional 0x or 0X prefix
	#define STR_BASE_HEX	(1<<1)

//	Do not accept 0b 0B 0x or 0X 
	#define STR_NOBX		(1<<2)

//	Do not accept a sign character, even if the destination is a signed type
	#define STR_NOSIGN		(1<<3)

//	Do not accept leading whitespace
	#define STR_NOSPACE		(1<<4)

//	Do not accept exponent for floating point types
	#define STR_NOEXP		(1<<5)

//	Accept and evaluate trailing Si prefix
//	#define STR_SI			(1<<6)	todo

//	Accept and evaluate trailing binary Si prefix
//	#define STR_SIB			(1<<7)	todo


//	These macros can be used with printf for printing str types using dynamic precision.
//	eg.  printf("name=%"PRIstr"\n", PRIstrarg(myname));
	#define PRIstr	".*s"
	#define PRIstrarg(arg)	((arg).size),((arg).data)

//	String structure.
//	This does not own the memory used to hold the string. It references data either in a string buffer, or const chars in the string pool.
	typedef struct strview_t
	{
		const char* data;
		int size;
	} strview_t;

//	Can be used instead of cstr, to avoid measuring the length of string literals at runtime
	#define cstr_SL(sl_arg) ((strview_t){.data=(sl_arg), .size=sizeof(sl_arg)-1})

//	Assign to a strview_t to make it invalid
	#define STRVIEW_INVALID		((strview_t){.data = NULL, .size = 0})

//	Generic macro for calling integer conversions based on the variable type
	#define strview_consume_value(dst, src, opt) _Generic((dst),\
		unsigned char*:			strview_consume_uchar,\
		unsigned short*:		strview_consume_ushort,\
		unsigned int*:			strview_consume_uint,\
		unsigned long*:			strview_consume_ulong,\
		unsigned long long*:	strview_consume_ullong,\
		char*:					strview_consume_char,\
		short*:					strview_consume_short,\
		int*:					strview_consume_int,\
		long*:					strview_consume_long,\
		long long*:				strview_consume_llong,\
		float*:					strview_consume_float\
		)(dst, src, opt)

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************


//	These functions consume text representing a number from src, and write the value of the number into dst
//	Return value is 0 on success, with src beginning at the character where conversion stopped
//	If src does not contain a valid value for the given type EINVALID is returned, and src is unmodified
//	If src contains a value out of range for the given type ERANGE is returned, and src is unmodified
	int strview_consume_uchar(unsigned char* dst, strview_t* src, int options);
	int strview_consume_ushort(unsigned short* dst, strview_t* src, int options);
	int strview_consume_uint(unsigned int* dst, strview_t* src, int options);
	int strview_consume_ulong(unsigned long* dst, strview_t* src, int options);
	int strview_consume_ullong(unsigned long long* dst, strview_t* src, int options);
	int strview_consume_char(char* dst, strview_t* src, int options);
	int strview_consume_short(short* dst, strview_t* src, int options);
	int strview_consume_int(int* dst, strview_t* src, int options);
	int strview_consume_long(long* dst, strview_t* src, int options);
	int strview_consume_llong(long long* dst, strview_t* src, int options);
	int strview_consume_float(float* dst, strview_t* src, int options);

//	Return a strview_t from a null terminated const char string.
//	If the argument is a string literal, cstr_SL() may be used instead, to prevent traversing the string literal to measure it's length
	strview_t cstr(const char* c_str);

//	Write a str out to a null terminated char* buffer.
//	The buffer memory and it's size must be provided by the caller
	char* strview_to_cstr(char* dst, size_t dst_size, strview_t str);
	
//	Return true if the strview_t is valid.
	bool strview_is_valid(strview_t str);

//	Swap strings a and b
	void strview_swap(strview_t* a, strview_t* b);

//	Return true if the strings match
	bool strview_is_match(strview_t str1, strview_t str2);

//	Same as strview_is_match() ignoring case
	bool strview_is_match_nocase(strview_t str1, strview_t str2);

/*	Replaces strcmp()
	returns >0 if the first non-matching character in str1 is greater (in ASCII) than that of str2.
	returns <0 if the first non-matching character in str1 is lower   (in ASCII) than that of str2.
	returns 0 if the strings are equal */
	int strview_compare(strview_t str1, strview_t str2);

/*	Return the sub string indexed by begin->end. end is non-inclusive.
	Negative values may be used, and will index from the end of the string backwards.
	The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string */
	strview_t strview_sub(strview_t str, int begin, int end);

//	Return a strview_t with the start and end trimmed of all characters present in chars_to_trim
	strview_t strview_trim(strview_t str, strview_t chars_to_trim);

//	Return a strview_t with the start trimmed of all characters present in chars_to_trim
	strview_t strview_trim_start(strview_t str, strview_t chars_to_trim);

//	Return a strview_t with the end trimmed of all characters present in chars_to_trim
	strview_t strview_trim_end(strview_t str, strview_t chars_to_trim);

/*	Return the strview_t for the first occurrence of needle in haystack.
 	If the needle is not found, strview_find_first() returns an invalid strview_t.
 	If the needle is found, the returned strview_t will match the contents of needle, only it will reference data within the haystack, and can be used with various strbuf.h functions as a means of specifying the position within the buffer. */
	strview_t strview_find_first(strview_t haystack, strview_t needle);

/*	Similar to strview_find_first(), but returns the LAST occurrence of needle in haystack.
	If needle is valid, and of length 0, it will always be found at the end of haystack.
	If needle is invalid, or if haystack is invalid, it will not be found. */
	strview_t strview_find_last(strview_t haystack, strview_t needle);

/*	Return a strview_t representing the contents of the source string up to, but not including, any of the delimiters.
	Additionally this text, and the delimeter itself is removed (popped) from the source string.
	If no delimeter is found, the returned string is the entire source string, and the source string becomes invalid */
	strview_t strview_split_first_delimeter(strview_t* strview_ptr, strview_t delimiters);

//	Same as strview_split_first_delimeter, ignoring case on the delimiters
	strview_t strview_split_first_delimiter_nocase(strview_t* strview_ptr, strview_t delimiters);

/*	Return a strview_t representing the contents of the source string from (but not including) the last delimiter found.
	Additionally this text, and the delimeter itself is removed (popped) from the end of the source string.
	If no delimeter is found the returned string is the entire source string, and the source string becomes invalid */
	strview_t strview_split_last_delimeter(strview_t* strview_ptr, strview_t delimiters);

//	Same as strview_split_last_delimeter, ignoring case on the delimiters
	strview_t strview_split_last_delimeter_nocase(strview_t* strview_ptr, strview_t delimiters);

/*	Split a strview_t at a specified index n.
	For n >= 0
		Return a strview_t representing the first n characters of the source string.
		Additionally the first n characters are removed (popped) from the start of the source string.
		If n is greater than the size of the source string ALL characters will be popped.

	For n < 0
		Return a strview_t representing the last -n characters of the source string.
		Additionally the last -n characters are removed (popped) from the end of the source string.
		If -n is greater than the size of the source string ALL characters will be popped.
*/
	strview_t strview_split_index(strview_t* strview_ptr, int index);

/*	Split a strview_t at a position specified by pos

	If pos references characters within *strview_ptr, return a strview_t representing all characters to the left of pos.
	If pos references the upper limit of *strview_ptr, the entire *strview_ptr is returned.
	If pos references the start of *strview_ptr, a valid strview_t of length 0 is returned.

	The returned characters are removed (popped) from *strview_ptr

	If strview_ptr is NULL, *strview_ptr is invalid, or pos is not a valid reference, an invalid string is returned and strview_ptr is unmodified.
*/
	strview_t strview_split_left_of_view(strview_t* strview_ptr, strview_t pos);

/*	Split a strview_t at a position specified by pos

	If pos references characters within *strview_ptr, return a strview_t representing all characters to the right of pos.
	If the upper limit of pos matches the upper limit of *strview_ptr, a valid strview_t of length 0 is returned.

	The returned characters are removed (popped) from *strview_ptr

	If strview_ptr is NULL, *strview_ptr is invalid, or pos is not a valid reference, an invalid string is returned and strview_ptr is unmodified.
*/
	strview_t strview_split_right_of_view(strview_t* strview_ptr, strview_t pos);

/*	Return the first char of str, and remove it from the str.
	Returns 0 if there are no characters in str.
	If str is known to contain at least one character, it is the equivalent of:
		strview_split_index(&str, 1).data[0]
	Only it avoids dereferencing a NULL pointer in the case where strview_split_index() returns an invalid str.
*/
	char strview_pop_first_char(strview_t* strview_ptr);


/*
	Returns a strview_t representing the first line within the source string, not including the eol terminator.
	The returned line and the terminator are removed (popped) from the source string.
	If a line terminator is not found, an invalid strview_t is returned and the source string is unmodified.

	If the source string already contains one or more lines:
		Any mixture of (CR,LF,CRLF,LFCR) can be handled, a CRLF or LFCR sequence will always be interpreted as 1 line ending.
	In this case eol may be NULL.

	If the source string is being appended to one character at a time, such as when gathering user input:
		Any type of line ending can be handled by providing variable eol.
		This variable stores the state of the eol discriminator, regarding if a future CR or LF needs to be ignored.
		It's initial value should be 0.
*/
	strview_t strview_split_line(strview_t* strview_ptr, char* eol);


#endif

