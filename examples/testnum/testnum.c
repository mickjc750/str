/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>
	#include <math.h>

	#include "prnf.h"
	#include "../../strbuf.h"
	#include "../../strview.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************
 
//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) printf("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

//********************************************************************************************************
// Public variables
//********************************************************************************************************


//********************************************************************************************************
// Private variables
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void test_number_conversion(strview_t view, int options);
	static void char_handler_prnf(void* nothing, char c);

//********************************************************************************************************
// Public functions
//********************************************************************************************************


int main(int argc, const char* argv[])
{
	(void)argc;
	(void)argv;
	char* txt=NULL;
	size_t txt_size = 0;
	strview_t view;
	prnf_out_fptr = char_handler_prnf;
	prnf("Hello test\n");

	while(true)
	{
		getline(&txt, &txt_size, stdin);
		view = strview_trim_end(cstr(txt), cstr("\r\n"));
		test_number_conversion(view, 0);
	};

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

	#define numtsthelper(fmt, var, opt)																							\
	do																															\
	{																															\
		v = view;																												\
		err = strview_consume_value(&(var), &v, opt);																			\
		printf("%8s Error=%30s    value= " fmt " remainder = [%"PRIstr"]\n", fmt, strerror(err), err? 0:(var), PRIstrarg(v));	\
	}while(false)

static void test_number_conversion(strview_t view, int options)
{
	int err;
	strview_t v;

	unsigned char		iuchar  ;
	unsigned short		iushort ;
	unsigned int		iuint   ;
	unsigned long		iulong  ;
	unsigned long long	iullong ;
	char				ichar   ;
	short				ishort  ;
	int					iint    ;
	long				ilong   ;
	long long			illong  ;
	float				ifloat  ;
	double				idouble ;
	long double			ildouble ;
	printf("\n[%"PRIstr"]\n", PRIstrarg(view));
	numtsthelper("%hhu", iuchar, options);
	numtsthelper("%hu", iushort, options);
	numtsthelper("%u", iuint, options);
	numtsthelper("%lu", iulong, options);
	numtsthelper("%llu", iullong, options);
	numtsthelper("%hhi", ichar, options);
	numtsthelper("%hi", ishort, options);
	numtsthelper("%i", iint, options);
	numtsthelper("%li", ilong, options);
	numtsthelper("%lli", illong, options);
	numtsthelper("%f", ifloat, options);
	numtsthelper("%lf", idouble, options);
	numtsthelper("%Lf", ildouble, options);
}

static void char_handler_prnf(void* nothing, char c)
{
	(void)nothing;
	putc(c, stdout);
}
