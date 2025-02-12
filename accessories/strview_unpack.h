/**
 * @file strview_unpack.h
 * @brief An accessory to strview.h to provide unpacking integer data types from a view.
 * @author Michael Clift
 * 
 */

#ifndef _STRVIEW_UNPACK_H_
	#define _STRVIEW_UNPACK_H_

	#include "string.h"
	#include "strview.h"
	#include "xendian.h"

//********************************************************************************************************
// Public defines
//********************************************************************************************************

	#define STRVIEW_UNPACK_LE(dst, view) 						\
	do															\
	{															\
		if((view).size >= sizeof(dst))							\
		{														\
			memcpy(&(dst), (view).data, sizeof(dst));			\
			(dst) = xendian_unpack_LE(dst);						\
			(view) = strview_sub((view), sizeof(dst), INT_MAX);	\
		};														\
	}while(0)

	#define STRVIEW_UNPACK_BE(dst, view) 						\
	do															\
	{															\
		if((view).size >= sizeof(dst))							\
		{														\
			memcpy(&(dst), (view).data, sizeof(dst));			\
			(dst) = xendian_unpack_BE(dst);						\
			(view) = strview_sub((view), sizeof(dst), INT_MAX);	\
		};														\
	}while(0)

#endif
