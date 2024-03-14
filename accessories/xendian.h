/**
 * @file xendian.h
 * @brief Header file for converting endianess using a generic macro.
 * @author Michael Clift
 * @note Requires __BYTE_ORDER__ to be defined, which for GCC and most other compilers, it will be.
 */

/*
 Example to get the value of an int32 when the memory contents is known to be big endian:
 
 	host_i32 = xendian_unpack_BE(network_i32);
 		
 This will reverse the bytes if the host system is little endian, and leave them unchanged if the host system is big endian.



 Packing a uint32_t into a big endian is exactly the same operation (the bytes are reversed if needed).
 A different macro name is provided only to make the operation more descriptive.
 Example to pack a uint32_t value into big endian:

 	network_i32 = xendian_pack_BE(host_i32);

 uint8_t and int8_t are also supported, even though no change occurs as they don't have endianess.
 The application may still wish to wrap 8 bit variables to accommodate the future possibility that the variables get promoted to bigger ones.

 Floating point types are not supported, as there are no guarantees to their endiness across different platforms.
 Passing floats to these macros will produce a compilation error.
 
 Primitive integer types (char/short/int/long/long long) will be supported, as stdint.h simply typedefs these.
 However, as the size of primitive types varies between platforms it would be ill advised to use these to pack/unpack network data.

*/

#ifndef _XENDIAN_H_
	#define _XENDIAN_H_

	#include <stdint.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

	union xendian_pack16_union
	{
		int8_t		bytes[2];
		int16_t		word16;
	};

	union xendian_pack32_union
	{
		int16_t		word16[2];
		int32_t		word32;
	};

	union xendian_pack64_union
	{
		int32_t		word32[2];
		int64_t		word64;
	};

	#define xendian_SWAP(a)							\
	_Generic((a), 									\
	uint8_t:	(a),								\
	int8_t:		(a),								\
	uint16_t:	((uint16_t)xendian_swap16(a)),		\
	int16_t:	(xendian_swap16(a)),				\
	uint32_t:	((uint32_t)xendian_swap32(a)),		\
	int32_t:	(xendian_swap32(a)),				\
	uint64_t:	((uint64_t)xendian_swap64(a)),		\
	int64_t:	(xendian_swap64(a))	)

#ifndef __BYTE_ORDER__
	#error "__BYTE_ORDER__ is not defined" 
#else

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

	#define xendian_pack_LE(a) 		(a)
	#define xendian_unpack_LE(a) 	(a)
	#define xendian_pack_BE(a) 		xendian_SWAP(a)
	#define xendian_unpack_BE(a) 	xendian_SWAP(a)

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

	#define xendian_pack_LE(a) 		xendian_SWAP(a)
	#define xendian_unpack_LE(a) 	xendian_SWAP(a)
	#define xendian_pack_BE(a) 		(a)
	#define xendian_unpack_BE(a) 	(a)

#else
	#error "Endianess not supported"
#endif
#endif
	
//********************************************************************************************************
// Public functions
//********************************************************************************************************
	
static int16_t xendian_swap16(int16_t x)
{
	union xendian_pack16_union in, out;

	in.word16 = x;
	out.bytes[0] = in.bytes[1];
	out.bytes[1] = in.bytes[0];
	return out.word16;
}

static int32_t xendian_swap32(int32_t x)
{
	union xendian_pack32_union in, out;

	in.word32 = x;
	out.word16[0] = xendian_swap16(in.word16[1]);
	out.word16[1] = xendian_swap16(in.word16[0]);
	return out.word32;
}

static int64_t xendian_swap64(int64_t x)
{
	union xendian_pack64_union in, out;

	in.word64 = x;
	out.word32[0] = xendian_swap32(in.word32[1]);
	out.word32[1] = xendian_swap32(in.word32[0]);
	return out.word64;
}

#endif
