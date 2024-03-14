/**
 * @file strbuf_pack.h
 * @brief An accessory to strbuf.h to provide appending packed integer types to a buffer.
 * @author Michael Clift
 * 
 */

#ifndef _STRBUF_PACK_H_
	#define _STRBUF_PACK_H_

	#include "xendian.h"
	#include "strbuf.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//	Pack integer types little endian into buffer
	#define STRBUF_PACK_LE(dst, src) do {typeof(src) tmp = xendian_pack_LE(src); strbuf_append_strview((dst), (strview_t){.data = (const char*)&tmp, .size=sizeof(tmp)});}while(0)

//	Pack integer types big endian into buffer
	#define STRBUF_PACK_BE(dst, src) do {typeof(src) tmp = xendian_pack_BE(src); strbuf_append_strview((dst), (strview_t){.data = (const char*)&tmp, .size=sizeof(tmp)});}while(0)

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

#endif
