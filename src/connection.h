/**********************************************************************\
 * MultiTerminal
 * connection.h
 *
 * Connection classes
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

#include "connectionSettings.h"

struct connection
{
	connection();
	connection(const connection&) = delete;
	connection(connection&&) = delete;
	virtual ~connection();

	int fd;
};

class serialConnection
	: public connection
{
	public:
		serialConnection();
		serialConnection(const serialConnection&) = delete;
		serialConnection(serialConnection&&) = delete;
		virtual ~serialConnection();

		enum
		{
			NOERROR			= 0,
			ERR_OPEN_PORT	= -1,
			ERR_GET_ATTR	= -2,
			ERR_SET_ATTR	= -3,
			ERR_CUSTOM_BAUD	= -4
		};

		int baudrate;
		int databits;
		int parity;
		int stopbits;
		int flowctrl;

		int open(const char* dev);

		inline void FromSettings(connectionSettings& settings)
		{
			baudrate = settings.param;
			databits = settings.databits;
			parity   = settings.parity;
			stopbits = settings.stopbits;
			flowctrl = settings.flowctrl;
		}

		// From Linux ioctl-types.h
		// TIOCMGET, TIOCMSET, TIOCMBIC, TIOCMBIS
		//
		// TIOCM_DSR       DSR (data set ready)
		// TIOCM_DTR       DTR (data terminal ready)
		// TIOCM_RTS       RTS (request to send)
		// TIOCM_CTS       CTS (clear to send)
		//
		// TIOCM_CAR       DCD (data carrier detect)
		// TIOCM_RNG       RNG (ring)


		inline int SetPinValue(int pin, int val)
		{
			int ioctl_id = val ? TIOCMBIS : TIOCMBIC;
			return ioctl(fd, ioctl_id, &pin);
		}

		inline int GetPinValues(int& val)
		{
			return ioctl(fd, TIOCMGET, &val);
		}

	private:
		inline int BaudrateToConstant()
		{
			switch(baudrate)
			{
				case 50: return B50;
				case 75: return B75;
				case 110: return B110;
				case 134: return B134;
				case 150: return B150;
				case 200: return B200;
				case 300: return B300;
				case 600: return B600;
				case 1200: return B1200;
				case 1800: return B1800;
				case 2400: return B2400;
				case 4800: return B4800;
				case 9600: return B9600;
				case 19200: return B19200;
				case 38400: return B38400;
				case 57600: return B57600;
				case 115200: return B115200;
				case 230400: return B230400;
				case 460800: return B460800;
				case 500000: return B500000;
				case 576000: return B576000;
				case 921600: return B921600;
				case 1000000: return B1000000;
				case 1152000: return B1152000;
				case 1500000: return B1500000;
				case 2000000: return B2000000;
				case 2500000: return B2500000;
				case 3000000: return B3000000;
				case 3500000: return B3500000;
				case 4000000: return B4000000;
				default: return 0;
			}
		}
};

