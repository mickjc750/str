/**
 * @file strbuf_crypt.h
 * @brief An additional layer to strbuf.h to provide AES256 encryption
 * @brief To use, simply include this header in the c file containing #define STRBUF_IMPLEMENTATION
 * @author Michael Clift
 * 
 */

#ifndef _STRBUF_CRYPT_H_
	#define _STRBUF_CRYPT_H_

	#include <stdint.h>

#ifdef STRBUF_IMPLEMENTATION
	#undef STRBUF_IMPLEMENTATION
	#include "strbuf.h"
	#define STRBUF_IMPLEMENTATION
#endif
	
//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

/**
 * @brief Encrypt the contents of the buffer with AES256 CBC and PKCS#7 padding
 * @param buf_ptr The address of a pointer to the buffer.
 * @return A view of the encrypted buffer.
 * @note 1-16 bytes of padding is always added.
 * @note The allocator configured for strbuf will be used to create the 240byte AES context.
    **********************************************************************************/
	strview_t strbuf_encrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16]);

/**
 * @brief Decrypt the contents of the buffer with AES256 CBC and PKCS#7 padding
 * @param buf_ptr The address of a pointer to the buffer.
 * @return A view of the decrypted buffer.
 * @note The allocator configured for strbuf will be used to create the 240byte AES context.
    **********************************************************************************/
	strview_t strbuf_decrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16]);

#endif





//********************************************************************************************************
// STRBUF_IMPLEMENTATION
//********************************************************************************************************
#ifdef STRBUF_IMPLEMENTATION

/*
	Ressources:
	https://www.devglan.com/online-tools/aes-encryption-decryption
	https://www.javainuse.com/aesgenerator
	https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Cipher_block_chaining_(CBC)
*/
	#include <stdint.h>

	#include "strbuf.h"
	#include "strview.h"
	#include "aes/aes.h"
	#include "aes/memxor.h"

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define swap(a, b) \
		do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

	#define AES_BLOCK_SIZE	16

//	Matching signatures for enc256cbc/dec256cbc allow us to unify the encrypt/decrypt callers with a function pointer
	typedef int (*encdec_fptr_t)(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16]);

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static int enc256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16]);
	static int dec256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16]);
	static int cypher_size(int content_size);
	static strview_t crypt(encdec_fptr_t fptr, strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16]);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strview_t strbuf_encrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16])
{
	return crypt(enc256cbc, buf_ptr, key, ivec);
}

strview_t strbuf_decrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16])
{
	return crypt(dec256cbc, buf_ptr, key, ivec);
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static strview_t crypt(encdec_fptr_t fptr, strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16])
{
	strview_t retval = STRVIEW_INVALID;
	aes256_ctx_t* ctx_ptr;
	strbuf_t* buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		strbuf_grow(&buf, cypher_size(buf->size));
		buf->size = fptr((uint8_t*)buf->cstr, buf->size, key, ivec);
		buf->cstr[buf->size] = 0;
		*buf_ptr = buf;	
	};

	return retval;
}

// encrypt buf, using 256bit key and 128bit init vector.
// buf must be a minimum of 16 bytes larger than len, as up to 16 bytes of padding may occur.
// returns the size of the encoded output in bytes
static int enc256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16])
{
	int block_count = len/AES_BLOCK_SIZE + 1;
	int out_len = block_count * AES_BLOCK_SIZE;
	int pad_len = out_len - len;
	aes256_ctx_t *ctx_ptr = strbuf_alloc(sizeof(aes256_ctx_t));
	aes256_init(key, ctx);

	//pad last block
	memset(&buf[len], pad_len, pad_len);
	memxor(buf, ivec, AES_BLOCK_SIZE);
	while(block_count--)
	{
		aes256_enc(buf, ctx);
		if(block_count)
			memxor(&buf[AES_BLOCK_SIZE], buf, AES_BLOCK_SIZE);
		buf += AES_BLOCK_SIZE;
	};

	strbuf_free(ctx_ptr);
	return out_len;
}

// decrypt buf, using 256bit key and 128bit init vector.
// len must be a multiple of 16
// returns the size of the decoded output in bytes
static int dec256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16])
{
	int block_count = len/AES_BLOCK_SIZE;
	int out_len;
	int pad_len;
	uint8_t buf1[AES_BLOCK_SIZE];
	uint8_t buf2[AES_BLOCK_SIZE];
	uint8_t (*this_xor)[AES_BLOCK_SIZE] = &buf1;
	uint8_t (*next_xor)[AES_BLOCK_SIZE] = &buf2;
	aes256_ctx_t *ctx_ptr = strbuf_alloc(sizeof(aes256_ctx_t));

	aes256_init(key, ctx);
	memcpy(this_xor, ivec, AES_BLOCK_SIZE);
	while(block_count--)
	{
		memcpy(next_xor, buf, AES_BLOCK_SIZE);
		aes256_dec(buf, ctx);
		memxor(buf, this_xor, AES_BLOCK_SIZE);
		buf += AES_BLOCK_SIZE;
		swap(this_xor, next_xor);
	};

	pad_len = buf[-1];
	out_len = len - pad_len;

	strbuf_free(ctx_ptr);
	return out_len;
}

static int cypher_size(int content_size)
{
	int block_count = content_size/AES_BLOCK_SIZE + 1;
	return block_count * AES_BLOCK_SIZE;
}

#endif