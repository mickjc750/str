/*
*/
	#include <string.h>
	#include <stdint.h>
	#include "membound.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#ifdef USE_MCASSERT
		#include "mcassert.h"
		#define VIOLATION()		ASSERT(!"membound")
	#else
		#include "assert.h"
		#define VIOLATION()		assert(!"membound")
	#endif

//********************************************************************************************************
// Public functions
//********************************************************************************************************

void *memset_mb(struct membound_struct *mb,  void *dest, int c, size_t n)
{
	membound_test(mb, dest, n);
	memset(dest, c, n);
	return dest;
}

void *memcpy_mb(struct membound_struct *mb,  void *dest, const void *src, size_t n)
{
	membound_test(mb, dest, n);
	memcpy(dest, src, n);
	return dest;
}

void *memmove_mb(struct membound_struct *mb, void *dest, const void *src, size_t n)
{
	membound_test(mb, dest, n);
	memmove(dest, src, n);
	return dest;
}

char *strcat_mb(struct membound_struct *mb,  char *dest, const char *src)
{
	size_t	src_len, dest_len;

	src_len = strlen(src);
	dest_len = strlen(dest);
	memcpy_mb(mb, &dest[dest_len], src, src_len+1);
	return dest;
}

char *strcpy_mb(struct membound_struct *mb,  char *dest, const char *src)
{
	size_t	src_len;

	src_len = strlen(src);
	memcpy_mb(mb, dest, src, src_len+1);
	return dest;
}

void* membound_test(struct membound_struct *mb,  void *ptr, size_t n)
{
	if(mb)
	{
		if(!(mb->start <= ptr && (uint8_t*)ptr+n <= (uint8_t*)mb->end))
			VIOLATION();
	};
	return ptr;
}
