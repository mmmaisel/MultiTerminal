/**********************************************************************\
 * MultiTerminal
 * itoa.h
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
#pragma once

/// Converts an integer to its ASCII representation.
extern "C" char* itoa(int val, char* dst, int base);

/// Converts an integer to its ASCII representation.
/// A decimal seperator is inserted so that the resulting string
/// has "decimals" decimals. The base is always 10.
extern "C" char* itoa_fixed(int val, char* dst, int decimals);
