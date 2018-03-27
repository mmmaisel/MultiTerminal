/**********************************************************************\
 * MultiTerminal
 * multiBaseCommon.h
 *
 * Multibase IO common settings
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
\**********************************************************************/
#pragma once

struct multiBaseCommon
{
	inline multiBaseCommon()
		: charColors
		  {
			{0x55,0xAD,0xEB},	// TX
			{0xEB,0xD0,0x55},	// RX
			{0xEA,0x56,0x56}	// ERR
		  }
		, baseColors
		  {
			{0x0F,0x7D,0x12},	// ASCII
			{0x76,0x7D,0x0F},	// Bin
			{0x0F,0x77,0x7D},	// Oct
			{0x7d,0x0F,0x0F},	// Dec
			{0x11,0x11,0x8F}	// Hex
		  }
		, selCharColors
		  {
			{0x15,0x72,0xB3},	// TX
			{0xB3,0x97,0x15},	// RX
			{0xB2,0x16,0x16}	// ERR
		  }
		, selBaseColors
		  {
			{0x31,0xE7,0x38},	// ASCII
			{0xDC,0xE7,0x31},	// Bin
			{0x31,0xDD,0xE7},	// Oct
			{0xE7,0x31,0x31},	// Dec
			{0x43,0x43,0xE9}	// Hex
		  }
	{
	}

	enum eInputBase : uint8_t
	{
		BASE_ASC,
		BASE_BIN,
		BASE_OCT,
		BASE_DEC,
		BASE_HEX,
		BASE_COUNT
	};

	enum : uint8_t
	{
		COLOR_TX,
		COLOR_RX,
		COLOR_ERR,
		COLOR_COUNT
	};

	static inline uint8_t BASE_RADIX(int x)
	{
		const uint8_t _BASE_RADIX[] =
		{
			0, 2, 8, 10, 16
		};
		return _BASE_RADIX[x];
	}

	static inline uint8_t BASE_MAX_DIGITS(int x)
	{
		const uint8_t _BASE_MAX_DIGITS[] =
		{
			1, 8, 3, 3, 2
		};
		return _BASE_MAX_DIGITS[x];
	}

	wxColour charColors[COLOR_COUNT];
	wxColour baseColors[BASE_COUNT];

	wxColour selCharColors[COLOR_COUNT];
	wxColour selBaseColors[BASE_COUNT];
};
