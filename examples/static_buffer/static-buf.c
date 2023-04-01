/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <stdbool.h>

	#include "strbuf.h"
	#include "strview.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

	#define BUF_CAPACITY	200

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void ex1(strview_t v);

// 	For global static buffers the macro STRBUF_FIXED_CAP() may be used
	static strbuf_t* static_buf = STRBUF_FIXED_CAP(BUF_CAPACITY);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	printf("\n");
	ex1(cstr("1234"));
	ex1(cstr("5678"));
	ex1(cstr("0591"));
	printf("\n");
	strbuf_assign(&static_buf, cstr("This global static buffer can be created with the STRBUF_FIXED_CAP() macro"));
	printf("%s\n\n", static_buf->cstr);
	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static void ex1(strview_t v)
{
	static strbuf_space_t(BUF_CAPACITY) buf_space = STRBUF_STATIC_INIT(BUF_CAPACITY);
	strbuf_t* buf = (strbuf_t*)&buf_space;

	printf("local static buffer assignment was \"%s\" new buffer assignment is %"PRIstr"\n", buf->cstr, PRIstrarg(v));
	strbuf_assign(&buf, v);
}
