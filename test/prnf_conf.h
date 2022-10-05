//********************************************************************************************************
// PRNF Configuration
//********************************************************************************************************

//	If defined, %f and %e will be available
	#define PRNF_SUPPORT_FLOAT


//	Default precision for %e (engineering) notation
	#define PRNF_ENG_PREC_DEFAULT 	0


//	Default precision for %f (floats)
	#define PRNF_FLOAT_PREC_DEFAULT 	3


//	Provide column alignment using \v (see readme.md)
//	Remove this if you wish to output normal vertical line feeds (\v) from your format string
	#define PRNF_COL_ALIGNMENT


//	To enable extensions (%n), uncomment this #define, and include your memory allocator,
//	define prnf_free() to be your memory allocators free() function.
//	define prnf_malloc() to be your memory allocators malloc() function
//	#define PRNF_SUPPORT_EXTENSIONS
//	#include <stdlib.h>
//	#define prnf_free(arg) 		free(arg)
//	#define prnf_malloc(arg) 	malloc(arg)


//	If you have a runtime warning handler, include it here and define WARN to be your handler.
//  A 'true' argument is expected to generate a warning.
//	Otherwise define PRNF_WARN() as ((void)0) (which does nothing).
//	#include "warn.h"
	#define PRNF_WARN(arg) ((void)(0))


//	If you have an assertion handler, include it here and define ASSERT to be your handler.
//  A 'false' argument is expected to generate an error.
//	Otherwise define PRNF_ASSERT() as ((void)0) (which does nothing).
//	#include <assert.h>
//	#define PRNF_ASSERT(arg) assert(arg)
	#define PRNF_ASSERT(arg) ((void)(0))
