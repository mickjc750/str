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

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void ex1(strview_t v);
	static void ex2(strview_t v);
	static void ex3(strview_t v);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	printf("\n");
	ex1(cstr("1234-5678-3456-7890"));
	ex2(cstr("4720-ABCD-9999-1234"));
	ex3(cstr("Hello Fred"));
	printf("\n");
	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static void ex1(strview_t v)
{
	strbuf_t* buf = STRBUF_FIXED_CAP(200);	// stack allocated buffer with capacity not less than 200

	strbuf_assign(&buf, v);
	printf("Example 1: stripping '-' from \"%s\" in a fixed length stack buffer created using STRBUF_FIXED_CAP().\n", buf->cstr);
	strbuf_strip(&buf, cstr("-"));
	printf("%s\n\n", buf->cstr);
}

static void ex2(strview_t v)
{
	strbuf_space_t(v.size) bufspace;
	strbuf_t* buf = strbuf_create_fixed(&bufspace, sizeof(bufspace));

	strbuf_assign(&buf, v);
	printf("Example 2: stripping '-' from \"%s\" in a stack buffer with size determined at runtime.\n", buf->cstr);
	printf("This should be used with caution, as a caller may overflow the stack with a large enough view.\n");
	strbuf_strip(&buf, cstr("-"));
	printf("%s\n\n", buf->cstr);
}

static void ex3(strview_t v)
{
	char cbuf[v.size + 1];
	strview_to_cstr(cbuf, sizeof(cbuf), v);

	printf("Example 3: assigning contents of passed view \"%s\" to a regular char[] with size determined at runtime\n", cbuf);
	printf("This should be used with caution, as a caller may overflow the stack with a large enough view.\n");
}

