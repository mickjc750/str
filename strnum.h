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
 * @brief Generic macro. Evaluates to the appropriate strnum_x function for the destination type.
 * @param dst A pointer to the destination.
 * @param src A pointer to the source view.
 * @param opt One or more STRNUM_x flags.
  **********************************************************************************/ 
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
	int strnum_uchar(unsigned char* dst, strview_t* src, int options);	

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
	int strnum_ushort(unsigned short* dst, strview_t* src, int options);

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
	int strnum_uint(unsigned int* dst, strview_t* src, int options);

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
	int strnum_ulong(unsigned long* dst, strview_t* src, int options);

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
	int strnum_ullong(unsigned long long* dst, strview_t* src, int options);

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
	int strnum_char(char* dst, strview_t* src, int options);

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
	int strnum_short(short* dst, strview_t* src, int options);

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
	int strnum_int(int* dst, strview_t* src, int options);

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
	int strnum_long(long* dst, strview_t* src, int options);

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
	int strnum_llong(long long* dst, strview_t* src, int options);

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
	int strnum_float(float* dst, strview_t* src, int options);

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
	int strnum_double(double* dst, strview_t* src, int options);

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
	int strnum_ldouble(long double* dst, strview_t* src, int options);
#endif

#endif
