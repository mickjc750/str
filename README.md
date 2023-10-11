# STR - C string handling library

C String handling library inspired by Luca Sas. https://www.youtube.com/watch?v=QpAhX-gsHMs&t=3009s

&nbsp;
# Table of Contents
1.  [Introduction](#introduction)
2.  [Features](#features)
3.  [Usage](#usage)
4.  [Understanding the separate purposes of strview.h, strbuf.h, and strnum.h](#understanding-the-separate-purposes-of-strviewh-strbufh-and-strnumh)
5.  [Use cases and good practices](#use-cases-and-good-practices)
6.  [Use C strings and string literals with functions that take a strview_t](#use-of-regular-c-strings-and-string-literals-with-functions-that-take-a-strview_t)
7.  [Comparison with SDS](#comparison-with-sds-httpsgithubcomantirezsds)
8.  [Using strbuf for things other than strings](#using-strbuf-for-things-other-than-strings)
9.  [Passing strings to printf()](#passing-strings-to-printf)
10. [strbuf.h api reference](/docs/strbuf-api.md)
11. [strview.h api reference](/docs/strview-api.md)
12. [strnum.h api reference](/docs/strnum-api.md)
13. [Contributing](#contributing)

&nbsp;
&nbsp;
# Introduction

 This project aims to implement a convenient and intuitive approach to string handling, described in a talk by Luca Sas in the above mentioned YouTube video.

 The core ideas for strview.h are:
 * Separating the ownership of a string (which can modify/build strings) from the access or view of strings (navigating/splitting/trimming).
 * Returning strings by value, to avoid pointers.
 * Ditching the requirement for null termination.

&nbsp;
# Features
 * Supports static or stack allocated buffers, for applications unable (or programmers unwilling) to use dynamic memory allocation.
 * Supports custom allocators, for applications which use temporary allocators for speed. Or can default to malloc/free for simplicity.
 * A rich set of of string splitting/trim/search functions.
 * A number parser which checks for errors, including range errors, or invalid input.
 * A test suite which uses https://github.com/silentbicycle/greatest, currently passing all 43 tests, 528 assertions.

 For an example of how useful this approach to string handling is, see the URI parser in [/examples/parse_uri/parse-uri.c](/examples/parse-uri/parse-uri.c)

&nbsp;
## Standard used
 GNU99

&nbsp;
# Usage
 Copy the source files __strview.h__/__strview.c__ and optionally __strbuf.h__/__strbuf.c__ , __strnum.h__/__strnum.c__ into your project.
 Add any desired options (described below) to your compiler flags (eg. -DSTRBUF_PROVIDE_PRINTF).
 strnum.c requires linking against the maths library for interpreting float values. So either add -lm to your linker options, or -DSTRNUM_NOFLOAT to your compiler options if you don't need float conversion.
 A list and explanation of options is included at the top of each header file for convenient copy & pasting.

&nbsp;
# Understanding the separate purposes of strview.h, strbuf.h, and strnum.h
 This project is provided in three main parts, **strview.h** which provides a **strview_t** type, and **strbuf.c** which provides a **strbuf_t** type.
 Additionally **strnum.h** is provided for interpreting numbers, and is inspired by from_chars https://github.com/Andersama/from_chars.

To understand this approach to string handling, and the purpose of each, it helps to think in terms of string ownership.

## strview_t
**strview_t** doesn't own the string. It's just a view into a string, and can't be used to free it, and shouldn't be used to change its characters. You can only change the range of the view, or split it into multiple views, or interpret a view as a number etc. **strview_t** is intended for reading and parsing strings, not building them. As there is no null terminator requirement, binary strings including the full ascii set 0-255 can safely be worked with.
**strview.h** does not depend on **strbuf.h**, and can be useful on it’s own.

&nbsp;
## strbuf_t
**strbuf_t** DOES own the string, and contains the information needed to resize it, change its contents, or free it. Dynamic memory allocation is not mandatory. The memory space can be as simple as a static buffer provided by the application. For a dynamic buffer, the application may either provide its own allocator, or for simplicity, strbuf can default to using malloc/free.

&nbsp;
# Use cases and good practices.

 Whether or not you pass a **strview_t** or a **strbuf_t** to your functions depends on the use case.

 If you want to get some data from a function, pass it a buffer, ie.
 
	strbuf_t* mybuf = strbuf_create(0, NULL);
	get_something(&mybuf);
	printf("got %s\n", mybuf->cstr);
	strbuf_destroy(&mybuf);

 As operations on a dynamic buffer may relocate it, it's important to only have one pointer to the buffer. Don't do this:

	strbuf_t* mybuf = strbuf_create(0, NULL);
	strbuf_t* yourbuf = mybuf;
	strbuf_assign(&mybuf, cstr("Hello"));	// yourbuf becomes a dangling pointer

 If you really need more than one reference to the same buffer, you can do this with a pointer to mybuf, and simply omit the & in the api calls:

	strbuf_t* mybuf = strbuf_create(0, NULL);
	strbuf_t** yourbuf = &mybuf;
	strbuf_assign(&mybuf, cstr("Hello"));
	strbuf_append(yourbuf, cstr(" Fred"));

 When passing a string to a function which doesn't need to change its characters, such as a function which might otherwise take a const char*, you should pass it a view of the string. Almost all strbuf functions will return a view of the buffer.

	strview_t title_view = strbuf_view(&title_buf);	// view the buffer
	title_set(title_view);	// pass the view to a function

 If the receiver of a strview_t wishes to guarantee the existence of the data after it returns, then it should assign it to its own buffer.

 In many cases you may wish to pass a strview_t to an stdlib function expecting a regular const char*. The content of a strview_t can be assigned to a buffer on creation. strbuf_create() is a generic macro which can also accept a strview_t instead of a size, to initialize the buffers contents.

	int myfunc(strview_t filename)
	{
		strbuf_t* mybuf= strbuf_create(filename, NULL);
		int fn = open(mybuf->cstr, O_RDWR);
		strbuf_destroy(&mybuf);
		return fn;
	}

&nbsp;
# Use of regular C strings and string literals with functions that take a strview_t

 There are two macros available for wrapping regular C strings __cstr()__ and __cstr_SL()__.
 &nbsp;

 __cstr()__ will measure the length of the string at runtime, and can wrap a char* to a regular c string. While this works fine with string literals, it is wasteful to measure the length of a string literal at runtime. Therefore __cstr_SL()__ can be used to wrap string literals, and will use sizeof()-1 instead of strlen().

	title_set(cstr("Main menu"));
	title_set(cstr_SL("Main menu"));

 * Note that many of the library functions which take a strview_t are generic macros which can also accept a C string.

&nbsp;
# Comparison with SDS https://github.com/antirez/sds

It should be noted, that __strview.h__ and __strnum.h__ do not depend on __strbuf.h__ and if desired, they may be used perfectly well in combination with SDS as a storage mechanism.

&nbsp;
## Common Advantages

* Like SDS, the buffer structure and its contents are held in a single allocation which improves cache locality.

&nbsp;
## Advantages over SDS
* STR also supports static or stack allocated buffers.
* STR supports more than one type of allocator at runtime.
* STR provides number parsing, with error and range checking.

SDS functions are of the form:

	s = sdscat(s,"Some more data");

As __sdscat()__ may relocate the buffer, you have to remember to assign the returned value back to the variable. If the variable passed to __sdscat()__ was of any use after the call, then this signature might make sense. But as that isn't the case, it makes more sense to pass the buffer by reference. So __strbuf.h__ functions are of the form:

	strbuf_append(&s, "Hello");

&nbsp;
## Disagreement on what is an advantage, and what is a hazard.

SDS buffers are handled by a pointer to the content, rather than a pointer to the buffers structure. This makes SDS buffers 'look like' regular C strings, and allows SDS buffers to be passed to functions which take a const char* without having to access the content member.

	printf("%s\n", sds_string);

Where as strbuf requires:

	printf("%s\n", str_buf->cstr);

Most projects will still need to make some use of regular c strings. As a strbuf_t is NOT a regular c string, it's important that it doesn't look like one. This helps to avoid mistaking a strbuf_t for a heap allocated C string and doing something like:

	free(str_buf);

Or forgetting to call:

	strbuf_destroy(&str_buf);

&nbsp;
# Using strbuf for things other than strings.

While this is a string api, dropping the requirement for null termination means that the full ascii set 0-255 can be handled. This makes it easy to concatenate chunks of packet data from a stream, identify header bytes, drop junk bytes, etc.. Outgoing packets can also be assembled in a strbuf_t before transmission.

&nbsp;
# Passing strings to printf()

 If the buffer holding the string is available, the buf->cstr member can be accessed and will always contain a null terminated string.

 For passing a view to printf, two macros are defined for this **PRIstr** and **PRIstrarg()**, which make use of printf's dynamic precision to limit the number of characters read. These are __PRIstr__ for the type, and __PRIstrarg__ as an argument wrapper.

	printf("The string is %"PRIstr"\n", PRIstrarg(mystring));

&nbsp;
# For more details on the usage of strbuf.h strview.h and strnum.h see the following:
 * [strbuf.h api reference](/docs/strbuf-api.md)
 * [strview.h api reference](/docs/strview-api.md)
 * [strnum.h api reference](/docs/strnum-api.md)


&nbsp;
# Contributing

Please raise issues to discuss changes or new features, or comment on existing issues.  
