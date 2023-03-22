# strnum.h

&nbsp;
&nbsp;
# About
 __strnum.h__ provides a number parser which is inspired by from_chars https://github.com/Andersama/from_chars

__strnum.h__ requires linking against the standard maths library (-lm in linker options) for conversion of floating point types.
 If you do not require this, you can define the symbol STRNUM_NOFLOAT.


&nbsp;
### The generic macro is available:
## `int strnum_value(*dst, strview_t* src, int options)`

&nbsp;
 The behavior of strnum_value depends on the destination type.
 
 __strnum_value()__ will convert as many characters as possible into a value. If the value is valid, and in range of the destination type, it will be written to *dst, any unconsumed characters will remain in src. 

The return value is 0 on success, ERANGE if the value is out of range for the destination type, or EINVAL if the source contains no meaningful representation of a value.

A test application is provided [/examples/testnum/testnum.c](/examples/testnum/testnum.c) , which allows typing in a number, and seeing the outcome for each number type.


&nbsp;
# Options

The options parameter may be 0, or one or more of the following flags


&nbsp;
## __STRNUM_NOBX__

 By default, strnum_value() will accept base prefixes 0b 0B 0x 0X to indicate binary or hex digits.
 If you do not with to accept 0b 0X as part of the number, add this option. Binary and hex digits can still be converted by using options __STRNUM_BASE_BIN__ or 
 __STRNM_BASE_HEX__ in combination with this option.


&nbsp;
&nbsp;
## __STRNUM_NOSPACE__

 By default, strnum_value() will ignore leading whitespace. If you do not wish to accept leading whitespace add this flag to options.

&nbsp;
&nbsp;
## __STRNUM_NOSIGN__

 Normally strnum_value() will accept a sign character (+/-) for signed integer types and float types only. If you do not with to accept a sign character for any type, add this flag to options.

&nbsp;
&nbsp;
## __STRNUM_BASE_BIN__

 Expect binary digits in the source. The digits may still be preceded by 0b or 0B, but not 0x or 0X. If you do not wish to accept the base prefix, add __STRNUM_NOBX__.
 
&nbsp;
&nbsp;
## __STRNUM_BASE_HEX__

 Expect hex digits in the source. The digits may still be preceded by 0x or 0X, but not 0b or 0B. If you do not wish to accept the base prefix, add __STRNUM_NOBX__.

&nbsp;
&nbsp;
## __STRNUM_NOEXP__

 For floating point conversions, do not accept an exponent such as E-4 e+12 e9 etc..

&nbsp;
&nbsp;
# Floating point conversions

__strnum_value()__ can compute floating point values using either float, double, or long double precision, depending on the destination type.

Long strings can be handled such as 45986643598673456876456498675643789.23485734657324923845765467892348756

By default, a trailing exponent is accepted.

Floating point arithmetic comes with the usual caveats regarding rounding errors. Unlike from_chars, strnum does not guarantee that the returned value will be the closest one possible to the value represented by the text. However __strnum_value()__ does attempt to minimize rounding errors, by converting as many digits as possible integer arithmetic, applying this value to the result, and then repeating this until all digits are converted.

 For special cases, the text "infinity", "inf" or "nan" will write the corresponding value to the destination, and will return success (0). Floating point conversions only return an error if the given text overflows during conversion, or if the given text does not represent a number. In these cases the destination is not modified.
