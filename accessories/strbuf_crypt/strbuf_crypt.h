/**
 * @file strbuf_io.h
 * @brief An additional layer to strbuf.h to provide read/write to/from a linux file descriptor.
 * @author Michael Clift
 * 
 */

#ifndef _STRBUF_CRYPT_H_
	#define _STRBUF_CRYPT_H_

	#include <stdint.h>
	#include "strbuf.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Encrypt the contents of the buffer with AES256 CBC and PKCS#7 padding
 * @param buf_ptr The address of a pointer to the buffer.
 * @return A view of the encrypted buffer.
 * @note 1-16 bytes of padding is always added, and the buffer must be either dynamic or appropriately sized.
 * @note For static buffers, the AES context of 240 bytes will be allcoated on the stack.
 * @note For dynamic buffer, the buffers own allocator will be used to create the 240byte AES context.
    **********************************************************************************/
	strview_t strbuf_encrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16]);

/**
 * @brief Decrypt the contents of the buffer with AES256 CBC and PKCS#7 padding
 * @param buf_ptr The address of a pointer to the buffer.
 * @return A view of the decrypted buffer.
 * @note For static buffers, the AES context of 240 bytes will be allcoated on the stack.
 * @note For dynamic buffer, the buffers own allocator will be used to create the 240byte AES context.
    **********************************************************************************/
	strview_t strbuf_decrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16]);

#endif
