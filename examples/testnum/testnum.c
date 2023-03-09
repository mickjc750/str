/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>
	#include <math.h>

	#include "strbuf.h"
	#include "strview.h"
	#include "strnum.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************
 
//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) print("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

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
	int options = 0;
	int i = 1;
	while(i != argc)
	{
		if(strview_is_match_nocase(cstr(argv[i]), cstr("base-bin")))
			options |= 1;
		if(strview_is_match_nocase(cstr(argv[i]), cstr("base-hex")))
			options |= 2;
		if(strview_is_match_nocase(cstr(argv[i]), cstr("nobx")))
			options |= 4;
		if(strview_is_match_nocase(cstr(argv[i]), cstr("nosign")))
			options |= 8;
		if(strview_is_match_nocase(cstr(argv[i]), cstr("nospace")))
			options |= 16;
		if(strview_is_match_nocase(cstr(argv[i]), cstr("noexp")))
			options |= 32;
		i++;
	};


	printf("\n\
strview number parser tester\n\
Command line options are:\n\
	base-bin	- accept only binary digits with an optional 0b or 0B prefix\n\
	base-hex	- accept only hex digits with an optional 0x or 0X prefix\n\
	nobx		- Do not accept a 0b 0B 0x 0X base prefix\n\
	nosign		- Do not accept a sign character, even for signed types\n\
	nospace		- Do not accept leading whitespace\n\
	noexp		- Do not accept exponent (ie. 12E-4)\n\
\n\
Currently enabled options : %s %s %s %s %s %s \n\nTypes number lines or ctrl-c to quit\n"
, options&1 ?  "BASE_BIN":""
, options&2 ?  "BASE_HEX":""
, options&4 ?  "NOBX":""
, options&8 ?  "NOSIGN":""
, options&16 ? "NOSPACE":""
, options&32 ? "NOEXP":"" );

	while(true)
	{
		getline(&txt, &txt_size, stdin);
		view = strview_trim_end(cstr(txt), cstr("\r\n"));
		test_number_conversion(view, options);
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
		err = strnum_value(&(var), &v, opt);																			\
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
