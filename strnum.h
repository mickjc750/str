/*
	strnum.h
	Functions for reading numbers from strings


	The following may be added to compiler options:

	-DSTRNUM_NOFLOAT
		Do not provide floating point conversions. Avoids the need to link with the standard maths library (-lm)
*/


#ifndef _STRNUM_H_
	#define _STRNUM_H_

	#include <stddef.h>
	#include <stdbool.h>
	#include <string.h>
	#include <errno.h>

	#include <strview.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//	Options flags for number conversions

//	Accept only binary digits, with an optional 0b or 0B prefix
	#define STRNUM_BASE_BIN	(1<<0)

//	Accept only hex digits, with an optional 0x or 0X prefix
	#define STRNUM_BASE_HEX	(1<<1)

//	Do not accept 0b 0B 0x or 0X 
	#define STRNUM_NOBX		(1<<2)

//	Do not accept a sign character, even if the destination is a signed type
	#define STRNUM_NOSIGN		(1<<3)

//	Do not accept leading whitespace
	#define STRNUM_NOSPACE		(1<<4)

//	Do not accept an exponent for floating point types
	#define STRNUM_NOEXP		(1<<5)

//	Accept and evaluate trailing Si prefix
//	#define STR_SI			(1<<6)	todo

//	Accept and evaluate trailing binary Si prefix
//	#define STR_SIB			(1<<7)	todo

	#ifndef STRNUM_NOFLOAT
//	Generic macro for calling number conversions based on the variable type
	#define strnum_value(dst, src, opt) _Generic((dst),\
		unsigned char*:			strnum_uchar,\
		unsigned short*:		strnum_ushort,\
		unsigned int*:			strnum_uint,\
		unsigned long*:			strnum_ulong,\
		unsigned long long*:	strnum_ullong,\
		char*:					strnum_char,\
		short*:					strnum_short,\
		int*:					strnum_int,\
		long*:					strnum_long,\
		long long*:				strnum_llong,\
		float*:					strnum_float,\
		double*:				strnum_double,\
		long double*:			strnum_ldouble\
		)(dst, src, opt)
	#else
//	Generic macro for calling number conversions based on the variable type
	#define strnum_value(dst, src, opt) _Generic((dst),\
		unsigned char*:			strnum_uchar,\
		unsigned short*:		strnum_ushort,\
		unsigned int*:			strnum_uint,\
		unsigned long*:			strnum_ulong,\
		unsigned long long*:	strnum_ullong,\
		char*:					strnum_char,\
		short*:					strnum_short,\
		int*:					strnum_int,\
		long*:					strnum_long,\
		long long*:				strnum_llong,\
		)(dst, src, opt)
	#endif

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	These functions consume text representing a number from src, and write the value of the number into dst
//	Return value is 0 on success, the remaining text in src begins where conversion stopped.
//	If src does not contain a valid value for the given type EINVALID is returned, and src is unmodified
//	If src contains a value out of range for the given type ERANGE is returned, and src is unmodified
	int strnum_uchar(unsigned char* dst, strview_t* src, int options);
	int strnum_ushort(unsigned short* dst, strview_t* src, int options);
	int strnum_uint(unsigned int* dst, strview_t* src, int options);
	int strnum_ulong(unsigned long* dst, strview_t* src, int options);
	int strnum_ullong(unsigned long long* dst, strview_t* src, int options);
	int strnum_char(char* dst, strview_t* src, int options);
	int strnum_short(short* dst, strview_t* src, int options);
	int strnum_int(int* dst, strview_t* src, int options);
	int strnum_long(long* dst, strview_t* src, int options);
	int strnum_llong(long long* dst, strview_t* src, int options);
#ifndef STRNUM_NOFLOAT
	int strnum_float(float* dst, strview_t* src, int options);
	int strnum_double(double* dst, strview_t* src, int options);
	int strnum_ldouble(long double* dst, strview_t* src, int options);
#endif

#endif

