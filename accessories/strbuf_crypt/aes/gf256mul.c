/* gf256mul.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2010  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>

uint8_t gf256mul(uint8_t a, uint8_t b, uint8_t reducer){
	uint8_t r=0;
	while(a&0xFE){
		if(a&1){
			r ^= b;
		}
		a >>= 1;
		if(b&0x80){
			b <<= 1;
			b ^= reducer;
		}else{
			b <<= 1;
		}
	}
	if(a&1){
		r ^= b;
	}
	return r;
}
