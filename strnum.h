/**
 * @file strnum.h
 * @brief Interpret numbers from string views.
 * @author Michael Clift
 * 
 * Inspired by the CPP utility std::from_chars
 * Provides error and range checking.
 *
 * ## Build options
 *	-DSTRNUM_NOFLOAT
 *		Do not provide floating point conversions. Avoids the need to link with the standard maths library (-lm)
 */

#ifndef _STRNUM_H_
	#define _STRNUM_H_

	#include <stddef.h>
	#include <stdbool.h>
	#include <string.h>
	#include <errno.h>
	#include <stdint.h>
	#include "strview.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

/**
 * @def STRNUM_DEFAULT
 * @brief Default behaviour.
  **********************************************************************************/ 
	#define STRNUM_DEFAULT	0

/**
 * @def STRNUM_BASE_BIN
 * @hideinitializer
 * @brief Accept only binary digits, with an optional 0b or 0B prefix
  **********************************************************************************/ 
	#define STRNUM_BASE_BIN	(1<<0)

/**
 * @def STRNUM_BASE_HEX
 * @hideinitializer
 * @brief Accept only hex digits, with an optional 0x or 0X prefix
  **********************************************************************************/ 
	#define STRNUM_BASE_HEX	(1<<1)

/**
 * @def STRNUM_NOBX
 * @hideinitializer
 * @brief Do not accept 0b 0B 0x or 0X 
  **********************************************************************************/ 
	#define STRNUM_NOBX		(1<<2)
	
/**
 * @def STRNUM_NOSIGN
 * @hideinitializer
 * @brief Do not accept a sign character, even if the destination is a signed type
  **********************************************************************************/ 
	#define STRNUM_NOSIGN	(1<<3)

/**
 * @def STRNUM_NOSPACE
 * @hideinitializer
 * @brief Do not accept leading whitespace
  **********************************************************************************/ 
	#define STRNUM_NOSPACE	(1<<4)

/**
 * @def STRNUM_NOEXP
 * @hideinitializer
 * @brief Do not accept an exponent for floating point types
  **********************************************************************************/ 
	#define STRNUM_NOEXP	(1<<5)

//	Accept and evaluate trailing Si prefix
//	#define STR_SI			(1<<6)	todo

//	Accept and evaluate trailing binary Si prefix
//	#define STR_SIB			(1<<7)	todo


	#ifndef STRNUM_NOFLOAT
/**
 * @def strnum_value(dst, src, opt)
 * @hideinitializer
 * @brief Generic macro. Evaluates to the appropriate strnum_consume_x function for the destination type.
 * @param dst A pointer to the destination.
 * @param src A pointer to the source view.
 * @param opt One or more STRNUM_x flags.
  **********************************************************************************/ 
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

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_uchar(unsigned char* dst, strview_t* src, int options);	

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_ushort(unsigned short* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_uint(unsigned int* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_ulong(unsigned long* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_ullong(unsigned long long* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_char(char* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_short(short* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_int(int* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_long(long* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_llong(long long* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	int8_t strnum_i8(strview_t src, int8_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	int16_t strnum_i16(strview_t src, int16_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	int32_t strnum_i32(strview_t src, int32_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	int64_t strnum_i64(strview_t src, int64_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	uint8_t strnum_u8(strview_t src, uint8_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	uint16_t strnum_u16(strview_t src, uint16_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	uint32_t strnum_u32(strview_t src, uint32_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	uint64_t strnum_u64(strview_t src, uint64_t dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	unsigned char strnum_uchar(strview_t src, unsigned char dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	unsigned short strnum_ushort(strview_t src, unsigned short dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	unsigned int strnum_uint(strview_t src, unsigned int dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	unsigned long strnum_ulong(strview_t src, unsigned long dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	unsigned long long strnum_ullong(strview_t src, unsigned long long dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	char strnum_char(strview_t src, char dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	short strnum_short(strview_t src, short dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	int strnum_int(strview_t src, int dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	long strnum_long(strview_t src, long dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	long long strnum_llong(strview_t src, long long dfault, int options);

#ifndef STRNUM_NOFLOAT

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_float(float* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_double(double* dst, strview_t* src, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * Consume text representing a number from the source view, and write the value of the number into dst.
 * If successful, consumed characters are removed from the source view. Otherwise the source remains unmodified.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param dst Address of the destination variable.
 * @param src Address of the source view.
 * @param options One or more STRNUM_x flags.
 * @return 0 on success.
 * @return EINVALID If src does not contain a valid value for the given type.
 * @return ERANGE   If value is out of range for the given type.
 *************************************************************************************/
	int strnum_consume_ldouble(long double* dst, strview_t* src, int options);


/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	float strnum_float(strview_t src, float dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	double strnum_double(strview_t src, double dfault, int options);

/**
 * @brief Interpret the contents of a string view as a number.
 * 
 * If successful the converted value is returned.
 * If the source text is invalid or out of range the default value is returned.
 * 
 * The default behaviour is:
 * * Ignore leading whitespace.
 * * Accept a sign character for signed types only.
 * * Recognise 0x 0X 0b 0B prefixes to indicate hex or binary.
 * * Accept exponent E or e for floating point types.
 * * Recognise inf or infinity for floating point types. Not case sensitive.
 * * Recognise nan for floating point types. Not case sensitive.
 * @param src The source view.
 * @param dfault The value to return if the conversion fails.
 * @param options One or more STRNUM_x flags.
 * @return The converted value, or the default value on error.
 *************************************************************************************/
	long double strnum_ldouble(strview_t src, long double dfault, int options);

#endif

#endif
