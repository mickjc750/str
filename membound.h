/*
	Simple wrapper functions to add memory boundary checking to string.h functions

	MEMBOUND_OF() macro can be used to assign a membound struct from a pointer to the type.
	eg.
	{
		struct membound_struct 	fred_mb;
		fred_t				 	*fred;

		fred 	= malloc(sizeof(fred_t));
		fred_mb	= MEMBOUND_OF(fred);

		//safely write somewhere into fred
		memcpy_mb(&fred_mb, etc..
	}

	Being a macro, it can also be used on arrays without pointer decay
	{
		char					name[MAXLEN_NAME+1];
		struct membound_struct	name_mb;

		name_mb	= MEMBOUND_OF(&name);

		//safely write somewhere into name
		memcpy_mb(&name_mb, etc..
	}

	When dereferencing a pointer, it may be wrapped in MBTST for testing
	{
		int 					myints[NUMBER_OF_OINT];
		int 					*myints_ptr;
		struct membound_struct	myints_mb;

		myints_mb = MEMBOUND_OF(&myints);

		myints_ptr = some_dangerous_calcuation();
		
		//instead of 
		*myint_ptr = 13;

		//use 
		*MBTST(myints_mb, myint_ptr) = 13;
	}

*/

#ifndef __MEMBOUND__
#define __MEMBOUND__
	#include <stddef.h>
	#include <stdint.h>

//********************************************************************************************************
// Public defines
//********************************************************************************************************

	struct membound_struct
	{
		void 	*start;
		void	*end;
	};

//	Use to assign membound_struct instance from a pointer to the type
	#define MEMBOUND_OF(argptr)		({struct membound_struct _mb; _mb.start=(argptr); _mb.end=(uint8_t*)(argptr)+sizeof(*(argptr)); _mb;})

//	Use to assign membound_struct from address and size
	#define MEMBOUND_SIZE(argptr, argsz)	({struct membound_struct _mb; _mb.start=(argptr); _mb.end=(uint8_t*)(argptr)+(argsz); _mb;})

//	If de-referencing a pointer, it can be wrapped in this macro for testing, the pointer type will be preserved
	#define MBTST(argmb, argptr)	((typeof(argptr))membound_test(&(argmb), (argptr), sizeof(*(argptr))))

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

	void *memset_mb(struct membound_struct *mb,  void *dest, int c, size_t n);
	void *memcpy_mb(struct membound_struct *mb,  void *dest, const void *src, size_t n);
	void *memmove_mb(struct membound_struct *mb, void *dest, const void *src, size_t n);
	char *strcat_mb(struct membound_struct *mb,  char *dest, const char *src);
	char *strcpy_mb(struct membound_struct *mb,  char *dest, const char *src);

//	test if n bytes at ptr violates mb, returns the pointer 
	void* membound_test(struct membound_struct *mb,  void *ptr, size_t n);

#endif
