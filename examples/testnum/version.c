/*
*/

	#include <stdint.h>
	#include "version.h"

//********************************************************************************************************
// Public variables
//********************************************************************************************************

	const char version_name[] = "str test";

	const uint32_t version_build_number = 
	#include "build_number.inc"
	;

	const char version_build_date[] =
	#include "build_date.inc"
	;

	const char version_gcc_version[] =
	#include "gcc_version.inc"
	;

	const uint8_t version_numbers[3] = {0,0,0};	// 0:major 1:minor 2:patch
	const char version_string[] = "0,0,0";
