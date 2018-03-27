/**********************************************************************\
 * MultiTerminal
 * itoa.cpp
 *
 * Integer to ASCII conversion function
 **********************************************************************
 * Copyright (C) 2018 - Max Maisel
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 \*********************************************************************/
#include "stdafx.h"
#include "itoa.h"

extern "C"
{
	static char* _itoa(int val, char* buffer, int base)
	{
		uint8_t sign = 0;
		register int tmp;
		char* bufpos = buffer;

		// Store the sign.
		if(val < 0)
			sign = 1;

		// Convert to ASCII
		do
		{
			if(base == 10)
			{
				tmp = val % 10;
				if(tmp < 0)
					tmp = -tmp;
				val /= 10;
			}
			else
			{
				tmp = (unsigned int)val % base;
				val = (unsigned int)val / base;
			}

			if(tmp > 9)
				*bufpos = tmp + 'A' - 10;
			else
				*bufpos = tmp + '0';
			++bufpos;
		}
		while(val);

		// Add sign
		if(sign && base == 10)
			*bufpos = '-';
		else
			bufpos--;

		return bufpos;
	}

	char* itoa(int val, char* dst, int base)
	{
		char buffer[sizeof(int)*8];
		char* bufpos = _itoa(val, buffer, base);

		// Reverse order and copy to dst.
		for(; bufpos >= buffer; --bufpos, ++dst)
			*dst = *bufpos;

		// Insert terminating 0 byte.
		*dst = 0;
		return dst;
	}

	char* itoa_fixed(int val, char* dst, int decimals)
	{
		char buffer[sizeof(int)*8];
		char* bufpos = _itoa(val, buffer, 10);
		char* dstStart = dst;

		if(*bufpos == '-')
			++dstStart;

		// Leading zeros after point ?
		if(bufpos - buffer < decimals - 1)
		{
			*(dst++) = '0';
			*(dst++) = '.';
			for(uint8_t x = 1; x < decimals - (bufpos - buffer); ++x)
				*(dst++) = '0';

			// Reverse order and copy to dst.
			for(; bufpos >= buffer; --bufpos, ++dst)
				*dst = *bufpos;
		}
		else
		{
			// Reverse order and copy to dst.
			for(; bufpos >= buffer; --bufpos, ++dst)
			{
				// Insert decimal seperator.
				// this is never reached if decimals == 0
				// or if leading zeros were inserted
				if(bufpos == buffer + decimals - 1)
				{
					// Insert leading zero
					if(dst == dstStart)
						*(dst++) = '0';
					*(dst++) = '.';
				}
				*dst = *bufpos;
			}
		}

		// Insert terminating 0 byte.
		*dst = 0;
		return dst;
	}
}
