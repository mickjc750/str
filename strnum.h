/**************************************************************************************************
 *  strnum.h
 *
 *  Fast, allocation-free numeric parsing from string views.
 *
 *  Inspired by C++17 std::from_chars, with additional validation and range checking.
 *
 *  Features:
 *      - Works directly on strview_t (no null-termination required)
 *      - Supports integer and optional floating point parsing
 *      - Fine-grained parsing control via flags
 *
 *  Build Options:
 *      STRNUM_NOFLOAT   Disable floating point support (avoids -lm)
 *
 *  Dependencies:
 *      strview.h
 *
 **************************************************************************************************/
#ifndef _STRNUM_H_
	#define _STRNUM_H_

	#include <stddef.h>
	#include <stdbool.h>
	#include <string.h>
	#include <errno.h>
	#include <stdint.h>
	#include "strview.h"


//==================================================================================================
// Option Flags
//==================================================================================================

#define STRNUM_DEFAULT	0		// 	Default behaviour.
	#define STRNUM_BASE_BIN	(1<<0)	//	Accept only binary digits, with an optional 0b or 0B prefix
	#define STRNUM_BASE_HEX	(1<<1)	//  Accept only hex digits, with an optional 0x or 0X prefix
	#define STRNUM_NOBX		(1<<2)	//	Do not accept 0b 0B 0x or 0X 
	#define STRNUM_NOSIGN	(1<<3)	//	Do not accept a sign character, even if the destination is a signed type
	#define STRNUM_NOSPACE	(1<<4)	//	Do not accept leading whitespace
	#define STRNUM_NOEXP	(1<<5)	//  Do not accept an exponent for floating point types
//	#define STR_SI			(1<<6)	// (todo) Accept and evaluate trailing Si prefix
//	#define STR_SIB			(1<<7)	// (todo) Accept and evaluate trailing binary Si prefix


// 	Generic macro. Evaluates to the appropriate strnum_consume_x function for the destination type.
// 	dst - A pointer to the destination.
// 	src - A pointer to the source view.
// 	opt - One or more STRNUM_x flags.
	#ifndef STRNUM_NOFLOAT
 	#define strnum_value(dst, src, opt) _Generic((dst),\
		unsigned char*:			strnum_consume_uchar,\
		unsigned short*:		strnum_consume_ushort,\
		unsigned int*:			strnum_consume_uint,\
		unsigned long*:			strnum_consume_ulong,\
		unsigned long long*:	strnum_consume_ullong,\
		char*:					strnum_consume_char,\
		short*:					strnum_consume_short,\
		int*:					strnum_consume_int,\
		long*:					strnum_consume_long,\
		long long*:				strnum_consume_llong,\
		float*:					strnum_consume_float,\
		double*:				strnum_consume_double,\
		long double*:			strnum_consume_ldouble\
		)(dst, src, opt)
	#else
	#define strnum_value(dst, src, opt) _Generic((dst),\
		unsigned char*:			strnum_consume_uchar,\
		unsigned short*:		strnum_consume_ushort,\
		unsigned int*:			strnum_consume_uint,\
		unsigned long*:			strnum_consume_ulong,\
		unsigned long long*:	strnum_consume_ullong,\
		char*:					strnum_consume_char,\
		short*:					strnum_consume_short,\
		int*:					strnum_consume_int,\
		long*:					strnum_consume_long,\
		long long*:				strnum_consume_llong,\
		)(dst, src, opt)
	#endif

//==================================================================================================
// CONSUMER API (mutates source)
//==================================================================================================

/**
 * Parse an unsigned integer from a string view.
 *
 * Consumes characters from *src and writes the result to *dst.
 * On success, the consumed portion is removed from src.
 * On failure, src is left unmodified.
 *
 * Default behaviour:
 *   - Skips leading whitespace
 *   - Accepts 0x/0X (hex) and 0b/0B (binary) prefixes
 *
 * Returns:
 *   0        Success
 *   EINVAL   Invalid input
 *   ERANGE   Out of range
 */
	int strnum_consume_uchar(unsigned char* dst, strview_t* src, int options);	
	int strnum_consume_ushort(unsigned short* dst, strview_t* src, int options);
	int strnum_consume_uint(unsigned int* dst, strview_t* src, int options);
	int strnum_consume_ulong(unsigned long* dst, strview_t* src, int options);
	int strnum_consume_ullong(unsigned long long* dst, strview_t* src, int options);


/**
 * Parse a signed integer from a string view.
 *
 * Consumes characters from *src and writes the result to *dst.
 * On success, the consumed portion is removed from src.
 * On failure, src is left unmodified.
 *
 * Default behaviour:
 *   - Skips leading whitespace
 *   - Accepts an optional sign (+ or -)
 *   - Accepts 0x/0X (hex) and 0b/0B (binary) prefixes
 *
 * Returns:
 *   0        Success
 *   EINVAL   Invalid input
 *   ERANGE   Out of range
 */
	int strnum_consume_char(char* dst, strview_t* src, int options);
	int strnum_consume_short(short* dst, strview_t* src, int options);
	int strnum_consume_int(int* dst, strview_t* src, int options);
	int strnum_consume_long(long* dst, strview_t* src, int options);
	int strnum_consume_llong(long long* dst, strview_t* src, int options);


#ifndef STRNUM_NOFLOAT
/**
 * Parse a floating point value from a string view.
 *
 * Consumes characters from *src and writes the result to *dst.
 * On success, the consumed portion is removed from src.
 * On failure, src is left unmodified.
 *
 * Default behaviour:
 *   - Skips leading whitespace
 *   - Accepts an optional sign (+ or -)
 *   - Accepts decimal notation with optional exponent (e or E)
 *   - Recognises "inf" and "infinity" (case insensitive)
 *   - Recognises "nan" (case insensitive)
 *
 * Returns:
 *   0        Success
 *   EINVAL   Invalid input
 *   ERANGE   Out of range
 */
	int strnum_consume_float(float* dst, strview_t* src, int options);
	int strnum_consume_double(double* dst, strview_t* src, int options);
	int strnum_consume_ldouble(long double* dst, strview_t* src, int options);
#endif


//==================================================================================================
// VALUE API (non-mutating)
//==================================================================================================

/**
 * Parse an unsigned integer from a string view.
 *
 * Does not modify the source.
 * If parsing succeeds, the converted value is returned.
 * If parsing fails, the provided default value is returned.
 *
 * Default behaviour:
 *   - Skips leading whitespace
 *   - Accepts 0x/0X (hex) and 0b/0B (binary) prefixes
 *
 * Parameters:
 *   src      Source string view
 *   default  Value to return on failure
 *
 * Returns:
 *   Parsed value on success
 *   default value on failure
 */
	unsigned char strnum_uchar(strview_t src, unsigned char dfault, int options);
	unsigned short strnum_ushort(strview_t src, unsigned short dfault, int options);
	unsigned int strnum_uint(strview_t src, unsigned int dfault, int options);
	uint8_t strnum_u8(strview_t src, uint8_t dfault, int options);
	uint16_t strnum_u16(strview_t src, uint16_t dfault, int options);
	uint32_t strnum_u32(strview_t src, uint32_t dfault, int options);
	uint64_t strnum_u64(strview_t src, uint64_t dfault, int options);
	unsigned long strnum_ulong(strview_t src, unsigned long dfault, int options);
	unsigned long long strnum_ullong(strview_t src, unsigned long long dfault, int options);

/**
 * Parse a signed integer from a string view.
 *
 * Does not modify the source.
 * If parsing succeeds, the converted value is returned.
 * If parsing fails, the provided default value is returned.
 *
 * Default behaviour:
 *   - Skips leading whitespace
 *   - Accepts an optional sign (+ or -)
 *   - Accepts 0x/0X (hex) and 0b/0B (binary) prefixes
 *
 * Parameters:
 *   src      Source string view
 *   default  Value to return on failure
 *
 * Returns:
 *   Parsed value on success
 *   default value on failure
 */
	char strnum_char(strview_t src, char dfault, int options);
	short strnum_short(strview_t src, short dfault, int options);
	int strnum_int(strview_t src, int dfault, int options);
	long strnum_long(strview_t src, long dfault, int options);
	long long strnum_llong(strview_t src, long long dfault, int options);
	int8_t strnum_i8(strview_t src, int8_t dfault, int options);
	int16_t strnum_i16(strview_t src, int16_t dfault, int options);
	int32_t strnum_i32(strview_t src, int32_t dfault, int options);
	int64_t strnum_i64(strview_t src, int64_t dfault, int options);



#ifndef STRNUM_NOFLOAT
/**
 * Parse a floating point value from a string view.
 *
 * Does not modify the source.
 * If parsing succeeds, the converted value is returned.
 * If parsing fails, the provided default value is returned.
 *
 * Default behaviour:
 *   - Skips leading whitespace
 *   - Accepts an optional sign (+ or -)
 *   - Accepts decimal notation with optional exponent (e or E)
 *   - Recognises "inf" and "infinity" (case insensitive)
 *   - Recognises "nan" (case insensitive)
 *
 * Parameters:
 *   src      Source string view
 *   default  Value to return on failure
 *
 * Returns:
 *   Parsed value on success
 *   default value on failure
 */
	float strnum_float(strview_t src, float dfault, int options);
	double strnum_double(strview_t src, double dfault, int options);
	long double strnum_ldouble(strview_t src, long double dfault, int options);
#endif


#endif
