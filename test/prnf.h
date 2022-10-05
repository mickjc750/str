/*
# PRNF
 A lightweight printf implementation.
 With some reasonable limitations, and non-standard behavior suited to microcontrollers.

  If you need a full featured printf(), I strongly recommend checking out eyalroz's fork
 of mpaland's printf(), which can be found [here](https://github.com/eyalroz/printf).
  After some involvement in that project, I realized it would never quite be exactly what I wanted.
 So I wrote this. Some of the ideas used in this are from the authors of that project.
 Particularly the function pointer output from mpaland, and the output wrapping used in this was suggested by eyalroz.
 
 * Thread and re-enterant safe.
 * Low stack & ram usage, zero heap usage.
 * Full support for AVR's PROGMEM requirements, with almost no cost to non-AVR targets.
 * Compatible enough to make use of GCC's format and argument checking (even for AVR).
 * no double or long long arithmetic, (doubles get demoted to float internally).
 
 * NO exponential form, %e provides SI units (y z a f p n u m - k M G T P E Z Y).
 * NO Octal, %o outputs binary instead, (who wants octal?)
 * NO adaptive %g %G
 * NO support for long long 
 
 Build size on avr with -Os is ~5kb with float support, and ~3kb without float



 Standard placeholder syntax:

 	%[flags][width][.precision][length]type



 Supported [flags]:

	- 		left align the output
	 
	+		prepends a + for positive numeric types
	 
	(space)		prepends a space for positive numeric type

	0 		prepends 0's instead of spaces to numeric types to satisfy [width]


 Unsupported [flags]:

	#		If you want 0x it needs to be in your format string.

	' (apostrophe)	No 1000's separator is available


 [width]
 
 	The minimum number of characters to output.
	If width is specified as * A dynamic width must be provided as an int argument preceding the argument to be formatted.
	A non-standard method of centering text is also provided (see below).


 [.precision]

 	For float %f, this is the number of fractional digits after the '.', valid range is .0 - .8
	For decimal integers, this will prepend 0's (if needed) until the total number of digits equals .precision
	For binary and hex, this specifies the *exact* number of digits to print, default is based on the argument size.
	For strings %s %S, This is the maximum number of characters to read from the source.

	If precision is specified as .* A dynamic precision must be provided as int argument preceding the argument to be formatted.

	Centering strings: If [width] is specified and precision is .0 %s arguments will be centered.
	Example to center within a 16 character LCD would be "%16.0s"
	**Caution - If you are generating formatting strings at runtime, and generate a %[width].0s, you will NOT get 0 characters.
	A dynamic precision of 0 provided to .* will not centre the string.

 Supported [length]:
 
 	Used to specify the size of the argument. The following is supported:
	hh 		Expect an int-sized argument which was promoted from a char.
	h 		Expect an int-sized argument which was promoted from a short.
	l		Expect a long-sized argument.
	z		Expect a size_t sized argument (size_t must not be larger than long).
	t		Expect a ptrdiff_t sized argument (ptrdiff_t must not be larger than long).

 Unsupported [length]:

	ll		(long long) not supported
	j		intmax_t not supported


 Supported types:

  	d,i		Signed decimal integer
  	u		Unsigned decimal integer
  	x,X  		Hexadecimal. Always uppercase, .precision defaults to argument size [length]
  	o		NOT Octal. Actually binary, .precision defaults to argument size [length]
  	s		null-terminated string in ram, or NULL. Outputs nothing for NULL.
	S		For AVR targets, read string from PROGMEM, otherwise same as %s
  	c		character 

	f,F		Floating point (not double). NAN & INF are always uppercase.
			Default precision is 3 (not 6).
			Digits printed must be able to be represented by an unsigned long,
			ie. with a precision of 3, maximum range is +/- 4294967.296 
			Values outside this range will produce "OVER".
			A value of 0.0 is always positive. 

	e		NOT exponential. Floating point with engineering notation (y z a f p n u m - k M G T P E Z Y).
			Number is postpended with the SI prefix. Default precision is 0.
 

 Unsupported types:

	g,G 		Adaptive floats not available
	a,A		Double in hex notation not available
	p		Pointer not available
	n		classic %n is not available, but %n may be repurposed for extensions if enabled (see below)


 # AVR's PROGMEM support:
The symbol PLATFORM_AVR must be defined, either before including prnf.h, or preferably by adding -DPLATFORM_AVR to compiler options.
prnf.c will #include itself for a second pass, to build the _P versions of the functions.
If you wish to write code which is cross compatible with AVR and non-AVR, use the _SL (String Literal) macros provided,
these will place string literals in PROGMEM for AVR targets and produce normal string literals for 'normal' von-newman targets.


On AVR, both the the format string, and string arguments, may be in either ram or program memory. See the header for more detailed usage.
   


   
# Default output for prnf()

Create a function which handles a single character, in the following form:

	void my_character_handler(void* x, char c)  
	{  
		(void)x;	// As x is not used, this line may be needed to avoid a compiler warning.  
		// Do stuff here to output character c, ie. write to uart ect.  
	}  

  
Then at the start of your program assign the default prnf() output to the above handler with:

	prnf_out_fptr = my_character_handler;




# Specifying the output handler when calling prnf()

A non-standard function is available which can be used for stream-like printing to different destinations.

	int fptrprnf(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, ...);

So to print to the above example of my_character_handler:

	fptrprnf(mycharacter_handler, NULL, "Hello Fred is %i years old\n", freds_age);

If you have specific information to pass to your character handler, you can pass the address of it instead of NULL, and it will be passed to the void* x in my_character_handler.




# Replacing printf()

Firstly, I would advise NOT doing this. Another reader of your code may see printf() and expect it to behave exactly like the standard printf(). If you return to your code to make changes in 5 years, that other programmer may be you.

If you still intend to override printf() this can be done with macros. Be aware that after you define 'printf' you will break anything which tries to add the format checking attribute __attribute__((format(printf, 1, 2)))

Another approach is to use GCC's --wrap feature in the compiler flags, which is probably better.




# Printing to text buffers

The usual functions are available (with print shortened to prn), and return a character count (disregarding any truncation).

	int sprnf(char* buffer, const char* format, ...) 
	int snprnf(char* buffer, size_t buffer_size, const char* format, ...) 




# Example debug macro:
The following is useful for debug/diagnostic and cross platform friendly with AVR:

	#define DBG(_fmtarg, ...) prnf_SL("%S:%.4i - "_fmtarg , PRNF_ARG_SL(__FILE__), __LINE__ ,##__VA_ARGS__)

Example usage:

	DBG("value is %i\n", 51);
The above will output something like "main.c:0113 - value is 51"

Note that even if you use this on many lines, the __FILE__ string literal will only occur once in the string pool. So it won't eat up all your program memory space.
The PRNF_ARG_SL() macro simply puts string literals into program memory on AVR targets or RAM on normal targets.



# Adding prnf() functionality to other IO modules:

This can easily be achieved by writing your own character handler, and variadic function for the module.
Then the non-variadic version of fptrprnf (vfptrprnf) can be used to print to your character handler.

An example for lcd_prnf() may look something like:

	#include <stdarg.h>

	// LCD prnf character handler
	static void prnf_write_char(void* nothing, char x)
	{
		lcd_data(x); // (function or code to write a single character to the LCD)
	}

	// formatted print to LCD
	void lcd_prnf(const char* fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		vfptrprnf(prnf_write_char, NULL, fmt, va);
		va_end(va);
	}




# snappf()
Safely APPEND to a string in a buffer of known size.

	int snappf(char* dst, size_t dst_size, const char* fmtstr, ...);

In addition to the regular snprnf(), which is used for safely printing to a buffer of n bytes. snappf() can safely append to the buffer.
This is useful in situations where you may need to iterate through a number of fields in a loop. The return value is the number of characters appended (ignoring truncation).





# Column alignment


This feature is enabled by default, but can be disabled in prnf_conf.h

It is possible to advance output to a specific column, with respect to the start of the output, or the last line ending. To achieve this prnf hijacks the \v (vertical tab) character.
The required format is:

	\v<col><pad character>


\v should be followed by a decimal number indicating the column on which the following text will start on (with 0 being the first column). The character used for padding is the first non-numeric character after this number. Note that due to this, digits cannot be used as the padding character. This feature is useful if you have output which contains fields of uncontrolled length, and then wish to align further output. If the current column is already at or past the column specified, then no padding will be applied. If \v occurs in your string without being followed by digits, then a regular \v character will be output.
Example:

	prnf("%s:%.4i(%s)\v30 %s\n", __FILE__, __LINE__, __func__, "This text starts on column 30");

Will yield something like:

	myfile.c:0041(main)         This text starts on column 30


It can also be used as an easy way to create banners, for example:

	prnf("\v30*\n Main Menu\n\v30*\n");

Will yield:

	******************************
	 Main Menu
	******************************





# Extending prnf

__THE FOLLOWING FEATURE IS DISABLED BY DEFAULT DUE TO HEAP INTERACTION__

 Most applications need to print things beyond what is offered by the standard placeholder types. This feature provides an easy way of doing that, but it requires the heap (or some other dynamic memory allocator) to work.

 When enabled, prnf() will accept a %n as a C string (like %s), only it will free() the address after printing. The argument type is int*, even though it points to a string. This allows the user to create functions which produce whatever strings they need (coordinates, timestamps, etc..), and then use these functions as arguments to prnf(). Example:

	static int* prext_bananas(int bananas)
	{
		int txt_size = 25;
		char* txt = malloc(txt_size);
		if(bananas > 10)
			snprnf(txt, txt_size, "LOTS OF BANANAS!");
		else if(bananas > 1)
			snprnf(txt, txt_size, "A few bananas");
		else if (bananas)
			snprnf(txt, txt_size, "Only one banana");
		else 
			snprnf(txt, txt_size, "NO BANANAS! :-(");
		return (int*)txt;
	}

	prnf("Gorilla Fred has %n and Uncle Bob has %n\n", prext_bananas(freds_bananas), prext_bananas(bobs_bananas));

As these functions need to return an int* to a string on the heap, their name should indicate that their sole purpose is
 to provide arguments to prnf() for %n placeholders.
Note that if you mistakenly mix up %s and %n, you will get a compilation warning (a good thing), as %s expects a char* and %n expects an int*



*/

#ifndef _PRNF_H_
#define _PRNF_H_

//	For va_list
	#include <stdarg.h>

//	size_t
	#include <stddef.h>

//	AVR's PSTR
	#ifdef PLATFORM_AVR
	#include <avr/pgmspace.h>
	#endif

//********************************************************************************************************
// Public defines
//********************************************************************************************************

/*
	PLATFORM_AVR should be defined for AVR targets.

	If you have modules that you wish to compile for both AVR and non-AVR targets, you can use the _SL (String Literal) macro wrappers.
	These will put string literals in PROGMEM for AVR targets only.
	The argument list will still be tested using a duiplicate format string in ram,
	 but optimisation (any level) must be enabled to remove the ram duplicate.

	PRNF_ARG_SL() is for passing string literals as arguments to %S (upper case) placeholders.

	Arguments can safely use operators ie. prnf_SL("%i", i++);

	The following two examples will put all string literals in PROGMEM for AVR targets, and ram for non-avr targets.
		prnf_SL("%-50\n", PRNF_ARG_SL("LEFT"));
		prnf_SL("%50\n", PRNF_ARG_SL("RIGHT"));
*/

#ifdef PLATFORM_AVR
//	Compiler will first test argument types based on format string, then remove the empty function during optimization.
	static inline void fmttst_optout(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
	static inline void fmttst_optout(const char* fmt, ...)
	{
	}

//	_SL macros for AVR
	#define prnf_SL(_fmtarg, ...) 						({int _prv; _prv = prnf_P(PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define sprnf_SL(_dst, _fmtarg, ...) 				({int _prv; _prv = sprnf_P(_dst, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define snprnf_SL(_dst, _dst_size, _fmtarg, ...) 	({int _prv; _prv = snprnf_P(_dst, _dst_size, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define snappf_SL(_dst, _dst_size, _fmtarg, ...) 	({int _prv; _prv = snappf_P(_dst, _dst_size, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define fptrprnf_SL(_fptr, _fargs, _fmtarg, ...) 	({int _prv; _prv = fptrprnf_P(_fptr, _fargs, PSTR(_fmtarg) ,##__VA_ARGS__); while(0) fmttst_optout(_fmtarg ,##__VA_ARGS__); _prv;})
	#define PRNF_ARG_SL(_arg)							((wchar_t*)PSTR(_arg))
#else
	#define prnf_SL(_fmtarg, ...) 						prnf(_fmtarg ,##__VA_ARGS__)
	#define sprnf_SL(_dst, _fmtarg, ...) 				sprnf(_dst, _fmtarg ,##__VA_ARGS__)
	#define snprnf_SL(_dst, _dst_size, _fmtarg, ...) 	snprnf(_dst, _dst_size, _fmtarg ,##__VA_ARGS__)
	#define snappf_SL(_dst, _dst_size, _fmtarg, ...) 	snappf(_dst, _dst_size, _fmtarg ,##__VA_ARGS__)
	#define fptrprnf_SL(_fptr, _fargs, _fmtarg, ...)	fptrprnf(_fptr, _fargs, _fmtarg ,##__VA_ARGS__)
	#define PRNF_ARG_SL(_arg)							((wchar_t*)(_arg))
#endif

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//	Point this at applications character handler for prnf(). The void* parameter is always passed NULL.
//	This function pointer is not called for terminating null characters.
//	It may be safely be assigned NULL, if no output from prnf() is required.
	extern void(*prnf_out_fptr)(void*, char);

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	Print, sending characters to prnf_out_fptr() (see above).
	int prnf(const char* fmtstr, ...) __attribute__((format(printf, 1, 2)));

//	Print to a char* buffer, with no size limit.
	int sprnf(char* dst, const char* fmtstr, ...) __attribute__((format(printf, 2, 3)));

//	Print safely to a char[] buffer of known size.
	int snprnf(char* dst, size_t dst_size, const char* fmtstr, ...) __attribute__((format(printf, 3, 4)));

//	*Append* safely to a char[] buffer of known size, returns the number of characters appended (ignoring truncation).
	int snappf(char* dst, size_t dst_size, const char* fmtstr, ...) __attribute__((format(printf, 3, 4)));

//	Print. Sending characters to specified character handler, not including a terminating null.
//	The character handler may be NULL if no output is required.
//	void* out_vars is also passed to the void* parameter if the character handler.
	int fptrprnf(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, ...) __attribute__((format(printf, 3, 4)));


//	non-variadic versions of the above, accepting va_list
//	The variadic functions above are quite small and call these. 
//	If you want to implement prnf() functionality in other modules,
//	 like lcd_prnf() or uart_prnf() you can write your own variadic functions which call these
	int vprnf(const char* fmtstr, va_list va);
	int vsprnf(char* dst, const char* fmtstr, va_list va);
	int vsnprnf(char* dst, size_t dst_size, const char* fmtstr, va_list va);
    int vsnappf(char* dst, size_t dst_size, const char* fmtstr, va_list va);
	int vfptrprnf(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, va_list va);

#ifdef PLATFORM_AVR
	int prnf_P(const char* fmtstr, ...);
	int sprnf_P(char* dst, const char* fmtstr, ...);
	int snprnf_P(char* dst, size_t dst_size, const char* fmtstr, ...);
	int snappf_P(char* dst, size_t dst_size, const char* fmtstr, ...);
	int fptrprnf_P(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, ...);
	int vprnf_P(const char* fmtstr, va_list va);
	int vsprnf_P(char* dst, const char* fmtstr, va_list va);
	int vsnprnf_P(char* dst, size_t dst_size, const char* fmtstr, va_list va);
    int vsnappf_P(char* dst, size_t dst_size, const char* fmtstr, va_list va);
	int vfptrprnf_P(void(*out_fptr)(void*, char), void* out_vars, const char* fmtstr, va_list va);
#endif

#endif // _PRNF_H_
