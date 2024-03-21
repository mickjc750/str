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

	#define ENCRYPT	true
	#define DECRYPT false

//	Matching signatures for enc256cbc/dec256cbc allow us to unify the encrypt/decrypt callers with a function pointer
	typedef int (*encdec_fptr_t)(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16], aes256_ctx_t* ctx);

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static int enc256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16], aes256_ctx_t* ctx);
	static int dec256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16], aes256_ctx_t* ctx);
	static int cypher_size(int content_size);
	static int encdec_with_ctx_on_stack(encdec_fptr_t fptr, uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16]);
	static strview_t strbuf_crypt(encdec_fptr_t fptr, strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16]);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

strview_t strbuf_encrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16])
{
	return strbuf_crypt(enc256cbc, buf_ptr, key, ivec);
}

strview_t strbuf_decrypt(strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16])
{
	return strbuf_crypt(dec256cbc, buf_ptr, key, ivec);
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static strview_t strbuf_crypt(encdec_fptr_t fptr, strbuf_t** buf_ptr, uint8_t key[32], uint8_t ivec[16])
{
	strview_t retval = STRVIEW_INVALID;
	aes256_ctx_t* ctx_ptr;
	strbuf_t* buf;

	if(buf_ptr && *buf_ptr)
	{
		buf = *buf_ptr;
		if(buf->allocator.allocator)
		{
			strbuf_grow(&buf, cypher_size(buf->size));
			ctx_ptr = buf->allocator.allocator(&buf->allocator, NULL, sizeof(*ctx_ptr));
			buf->size = fptr((uint8_t*)buf->cstr, buf->size, key, ivec, ctx_ptr);
			buf->allocator.allocator(&buf->allocator, ctx_ptr, 0);
			buf->cstr[buf->size] = 0;
		}
		else if(buf->capacity >= cypher_size(buf->size))
		{
			buf->size = encdec_with_ctx_on_stack(fptr, (uint8_t*)buf->cstr, buf->size, key, ivec);
			buf->cstr[buf->size] = 0;
		};
		*buf_ptr = buf;	
	};

	return retval;
}

static int encdec_with_ctx_on_stack(encdec_fptr_t fptr, uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16])
{
	aes256_ctx_t ctx;
	return fptr(buf, len, key, ivec, &ctx);
}

// encrypt buf, using 256bit key and 128bit init vector.
// buf must be a minimum of 16 bytes larger than len, as up to 16 bytes of padding may occur.
// returns the size of the encoded output in bytes
static int enc256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16], aes256_ctx_t* ctx)
{
	int block_count = len/AES_BLOCK_SIZE + 1;
	int out_len = block_count * AES_BLOCK_SIZE;
	int pad_len = out_len - len;
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
	return out_len;
}

// decrypt buf, using 256bit key and 128bit init vector.
// len must be a multiple of 16
// returns the size of the decoded output in bytes
static int dec256cbc(uint8_t *buf, int len, uint8_t key[32], uint8_t ivec[16], aes256_ctx_t* ctx)
{
	int block_count = len/AES_BLOCK_SIZE;
	int out_len;
	int pad_len;
	uint8_t buf1[AES_BLOCK_SIZE];
	uint8_t buf2[AES_BLOCK_SIZE];
	uint8_t (*this_xor)[AES_BLOCK_SIZE] = &buf1;
	uint8_t (*next_xor)[AES_BLOCK_SIZE] = &buf2;

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

	return out_len;
}

static int cypher_size(int content_size)
{
	int block_count = content_size/AES_BLOCK_SIZE + 1;
	return block_count * AES_BLOCK_SIZE;
}

