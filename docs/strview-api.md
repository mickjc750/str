# strview.h

&nbsp;
&nbsp;
# About
 strview.h provides functions for navigating, splitting, and trimming portions of const char string data. It may be used standalone, and does not depend on **strbuf.h**.

It does not store the underlying data itself, and is not intended to be used for modifying the data. The string data may be stored anywhere, string literals (the string pool), a dynamic buffer provided by strbuf.h, or a static buffer etc...

strview.h defines the following strview_t type :

	typedef struct strview_t
	{
		const char* data;
		int size;
	} strview_t;

Note that this holds only:
 * A pointer to the beginning of the string
 * The size (in characters) of the string

Some operations may return an invalid strview_t, in this case .data=NULL and .size==0. This is useful not only for indicating that an operation failed, but also for indicating that the data you attempted to parse does not exist. See [/examples/parse_uri/parse-uri.c](/examples/parse-uri/parse-uri.c) for an example of how this is useful.

Note that it is valid to have a strview_t of length 0. In this case *data should never be de-referenced (as it points to something of size 0, where no characters exist).


&nbsp;
&nbsp;
# Function reference
* [strview_t cstr(const char* c_str);](#strviewt-cstrconst-char-cstr)
* [char* strview_to_cstr(char* dst, size_t dst_size, strview_t str);](#char-strviewtocstrchar-dst-sizet-dstsize-strviewt-str)
* [bool strview_is_valid(strview_t str);](#bool-strviewisvalidstrviewt-str)
* [void strview_swap(strview_t* a, strview_t* b);](#void-strviewswapstrviewt-a-strviewt-b)

&nbsp;
## Comparison

 * [bool strview_is_match(strview_t str1, strview_t str2);](#bool-strviewismatchstrviewt-str1-strviewt-str2)
 * [bool strview_is_match_nocase(strview_t str1, strview_t str2);](#bool-strviewismatchnocasestrviewt-str1-strviewt-str2)
 * [int strview_compare(strview_t str1, strview_t str2);](#int-strview_compare)
 * [bool strview_starts_with(strview_t str1, strview_t str2);](#bool-strviewstartswithstrviewt-str1-strviewt-str2)
 * [bool strview_starts_with_nocase(strview_t str1, strview_t str2);](#bool-strviewstartswithnocasestrviewt-str1-strviewt-str2)

&nbsp;
## Trimming

 * [strview_t strview_trim(strview_t str, strview_t chars_to_trim);](#strviewt-strviewtrimstrviewt-str-strviewt-charstotrim)
 * [strview_t strview_trim_start(strview_t str, strview_t chars_to_trim);](#strviewt-strviewtrimstartstrviewt-str-strviewt-charstotrim)
 * [strview_t strview_trim_end(strview_t str, strview_t chars_to_trim);](#strviewt-strviewtrimendstrviewt-str-strviewt-charstotrim)

&nbsp;
## Searching

 * [strview_t strview_find_first(strview_t haystack, strview_t needle);](#strviewt-strviewfindfirststrviewt-haystack-strviewt-needle)
 * [strview_t strview_find_last(strview_t haystack, strview_t needle);](#strviewt-strviewfindlaststrviewt-haystack-strviewt-needle)

&nbsp;
## Splitting

 * [strview_t strview_sub(strview_t str, int begin, int end);](#strviewt-strviewsubstrviewt-str-int-begin-int-end)
 * [strview_t strview_split_first_delimeter(strview_t* src, strview_t delimiters);](#strviewt-strviewsplitfirstdelimeterstrviewt-src-strviewt-delimiters)
 * [strview_t strview_split_last_delimeter(strview_t* src, strview_t delimiters);](#strviewt-strviewsplitlastdelimeterstrviewt-src-strviewt-delimiters)
 * [strview_t strview_split_first_delimiter_nocase(strview_t* src, strview_t delimiters);](#strviewt-strviewsplitfirstdelimiternocasestrviewt-src-strviewt-delimiters)
 * [strview_t strview_split_last_delimeter_nocase(strview_t* src, strview_t delimiters);](#strviewt-strviewsplitlastdelimeternocasestrviewt-src-strviewt-delimiters)
 * [strview_t strview_split_index(strview_t* src, int index);](#strviewt-strviewsplitindexstrviewt-src-int-index)
 * [strview_t strview_split_left(strview_t* src, strview_t pos);](#strviewt-strviewsplitleftstrviewt-src-strviewt-pos)
 * [strview_t strview_split_right(strview_t* src, strview_t pos);](#strviewt-strviewsplitrightstrviewt-src-strviewt-pos)
 * [char strview_pop_first_char(strview_t* src);](#char-strviewpopfirstcharstrviewt-src)
 * [strview_t strview_split_line(strview_t* src, char* eol);](#strviewt-strviewsplitlinestrviewt-src-char-eol)




&nbsp;
# strview.h functions

&nbsp;
## `strview_t cstr(const char* c_str);`
 Return a strview_t from a null terminated const char[] string. If the string provided is a string literal, then the macro **cstr_SL("mystring")** may be used instead of **cstr("mystring")** to avoid measuring the strings length at runtime (although either will work).

&nbsp;
## `char* strview_to_cstr(char* dst, size_t dst_size, strview_t str);`
 Write a strview_t out to a null terminated char* buffer. The buffer and space available (including the terminator) must be provided by the caller.

&nbsp;
## `bool strview_is_valid(strview_t str);`
 Return true if the strview_t is valid.

&nbsp;
##	`void strview_swap(strview_t* a, strview_t* b);`
 Swap strings a and b.

&nbsp;
&nbsp;
# Comparison

&nbsp;
## `bool strview_is_match(strview_t str1, strview_t str2);`
 Return true if the strings match. Also returns true if BOTH strings are invalid.

&nbsp;
## `bool strview_is_match_nocase(strview_t str1, strview_t str2);`
 Same as **strview_is_match()** ignoring case.

&nbsp;
## `int strview_compare(strview_t str1, strview_t str2);`
 A replacement for strcmp(). Used for alphabetizing strings. May also be used instead of **strview_is_match()**, although keep in mind that it will return 0 if it compares an invalid string to a valid string of length 0. (Where **strview_is_match()** would return false if only one string is invalid.)

&nbsp;
## `bool strview_starts_with(strview_t str1, strview_t str2);`
 Similar to strview_is_match() but allows for trailing data in str1. Returns true if the content of str2 is found at the beginning of str1. Also Returns true if BOTH strings are invalid.

&nbsp;
## `bool strview_starts_with_nocase(strview_t str1, strview_t str2);`
 Same as strview_starts_with(), ignoring case.

&nbsp;
&nbsp;
# Trimming

&nbsp;
## `strview_t strview_trim(strview_t str, strview_t chars_to_trim);`
 Return a strview_t with the start and end trimmed of all characters present in **chars_to_trim**.

&nbsp;
## `strview_t strview_trim_start(strview_t str, strview_t chars_to_trim);`
 Return a strview_t with the start trimmed of all characters present in **chars_to_trim**.

&nbsp;
## `strview_t strview_trim_end(strview_t str, strview_t chars_to_trim);`
 Return a strview_t with the end trimmed of all characters present in **chars_to_trim**.

&nbsp;
&nbsp;
# Searching

&nbsp;
## `strview_t strview_find_first(strview_t haystack, strview_t needle);`
 Return the **strview_t** for the first occurrence of needle in haystack.
 If the needle is not found, strview_find_first() returns an invalid strview_t.
 If the needle is found, the returned strview_t will match the contents of needle, only it will reference data within the haystack. 
 This can then be used as a position reference for further parsing with strview.h functions, or modification with strbuf.h functions.

Some special cases to consider:
 * If **needle** is valid, and of length 0, it will always be found at the start of the string.
 * If **needle** is invalid, or if **haystack** is invalid, it will not be found.
	
&nbsp;
## `strview_t strview_find_last(strview_t haystack, strview_t needle);`
 Similar to strview_find_first(), but returns the LAST occurrence of **needle** in **haystack**.

Some special cases to consider:
* If **needle** is valid, and of length 0, it will always be found at the end of **haystack**.
* If **needle** is invalid, or if **haystack** is invalid, it will not be found.

&nbsp;
&nbsp;
# Splitting

&nbsp;
## `strview_t strview_sub(strview_t str, int begin, int end);`
 Return the sub string indexed by **begin** to **end**, where **end** is non-inclusive.
 Negative values may be used, and will index from the end of the string backwards.
 The indexes are clipped to the strings length, so INT_MAX may be safely used to index the end of the string. If the requested range is entirely outside of the input string, then an invalid **strview_t** is returned.

&nbsp;
## `strview_t strview_split_first_delimeter(strview_t* src, strview_t delimiters);`
 Return a **strview_t** representing the contents of the source string up to, but not including, any of characters in **delimiters**.
 Additionally, the contents of the returned **strview_t**, and the delimiter character itself is removed (popped) from the input string.
 If no delimiter is found, the returned string is the entire source string, and the source string becomes invalid.

Example usage:

    strview_t date = cstr("2022/10/03");
    strview_t year  = strview_split_first_delimeter(&date, cstr("/"));
    strview_t month = strview_split_first_delimeter(&date, cstr("/"));
    strview_t day   = strview_split_first_delimeter(&date, cstr("/"));

&nbsp;
## `strview_t strview_split_last_delimeter(strview_t* src, strview_t delimiters);`
 Same as **strview_split_first_delimeter()** but searches from the end of the string backwards.

&nbsp;
## `strview_t strview_split_first_delimiter_nocase(strview_t* src, strview_t delimiters);`
Same as **strview_split_first_delimeter()** but ignores the case of the delimiters

&nbsp;
## `strview_t strview_split_last_delimeter_nocase(strview_t* src, strview_t delimiters);`
Same as **strview_split_last_delimeter()** but ignores the case of the delimiters

&nbsp;
## `strview_t strview_split_index(strview_t* src, int index);`
Split a strview_t at a specified index n.
* For n >= 0
 Return a strview_t representing the first n characters of the source string.
 Additionally the first n characters are removed (popped) from the start of the source string.

* For n < 0
 Return a strview_t representing the last -n characters of the source string.
 Additionally the last -n characters are removed (popped) from the end of the source string.

If the index is outside of the range of the source string, then an invalid strview_t is returned and the source is unmodified

If you do not wish to remove/pop the split string from the source, this is easily achieved:

Instead of:

	strview_t full_view = cstr("Hello World");
	strview_t first_word = strview_split_first_delimiter(&full_view, cstr(" ")); // (full view becomes "World")

Simply assign the source to your destination before splitting:

	strview_t full_view = cstr("Hello World");
	strview_t first_word = full_view;
	first_word = strview_split_first_delimiter(&first_word, cstr(" ")); (full view remains unmodified)

&nbsp;
## `strview_t strview_split_left(strview_t* src, strview_t pos);`
Given a view (pos) into src, will return a strview_t containing the content to the left of pos. The returned view will be removed (popped) from src.

&nbsp;
## `strview_t strview_split_right(strview_t* src, strview_t pos);`
Given a view (pos) into src, will return a strview_t containing the content to the right of pos. The returned view will be removed (popped) from src.

&nbsp;
## `char strview_pop_first_char(strview_t* src);`
Return the first char of *src, and remove it from *src.
Returns 0 if there are no characters in str.
 If str is known to contain at least one character, it is the equivalent of:

	strview_split_index(&str, 1).data[0]
Only it avoids dereferencing a NULL pointer in the case where strview_split_index() would return an invalid strview_t due to the string being empty.

&nbsp;
## `strview_t strview_split_line(strview_t* src, char* eol);`
Returns a strview_t representing the first line within the source string, not including the eol terminator.
The returned line and the terminator are removed (popped) from the source string.
If a line terminator is not found, an invalid strview_t is returned and the source string is unmodified.

 If the source string already contains one or more lines:
Any mixture of (CR,LF,CRLF,LFCR) can be handled, a CRLF or LFCR sequence will always be interpreted as one line ending.
In this case *eol may be NULL.

 If the source string is being appended to one character at a time, such as when gathering user input:
Any type of line ending can be handled by providing variable eol.
This variable stores the state of the eol discriminator, regarding if a future CR or LF needs to be ignored.
It's initial value should be 0. See the test suite for usage of this.
