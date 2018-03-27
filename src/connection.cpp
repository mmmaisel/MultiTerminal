/**********************************************************************\
 * MultiTerminal
 * connection.cpp
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
#include "stdafx.h"
#include "connection.h"

connection::connection()
	: fd(0)
{
}

connection::~connection()
{
	if(fd)
	{
		close(fd);
		fd = 0;
	}
}

serialConnection::serialConnection()
	: baudrate(38400)
	, databits(connectionSettings::DATABITS_8)
	, parity(connectionSettings::PAR_NONE)
	, stopbits(connectionSettings::STOPBITS_1)
	, flowctrl(connectionSettings::FLOWCTRL_NONE)
{
}

serialConnection::~serialConnection()
{
}

// After https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
int serialConnection::open(const char* dev)
{
	fd = ::open(dev, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0)
	{
		return ERR_OPEN_PORT;
	}

	termios ttyinfo = {0};
	serial_struct serinfo = {0};

	if(tcgetattr(fd, &ttyinfo) != 0)
	{
		::close(fd);
		fd = 0;
		return ERR_GET_ATTR;
	}

	if(!BaudrateToConstant())
	{
		// Custom divisor
		serinfo.reserved_char[0] = 0;
		if(ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
			return ERR_CUSTOM_BAUD;

		serinfo.flags &= ~ASYNC_SPD_MASK;
		serinfo.flags |=  ASYNC_SPD_CUST;
		serinfo.custom_divisor = (serinfo.baud_base + (baudrate / 2)) / baudrate;

		if(serinfo.custom_divisor < 1)
			serinfo.custom_divisor = 1;

		if(ioctl(fd, TIOCSSERIAL, &serinfo) < 0)
			return ERR_CUSTOM_BAUD;

		if(ioctl(fd, TIOCGSERIAL, &serinfo) < 0)
			return ERR_CUSTOM_BAUD;

		if(serinfo.custom_divisor * baudrate != serinfo.baud_base)
			return ERR_CUSTOM_BAUD;

		// B38400 is treated differently if ASYNC_SPD_CUST is set
		cfsetispeed(&ttyinfo, B38400);
		cfsetospeed(&ttyinfo, B38400);
	}
	else
	{
		cfsetispeed(&ttyinfo, BaudrateToConstant());
		cfsetospeed(&ttyinfo, BaudrateToConstant());
	}

	const int DATABIT_CFLAGS[] = {CS5, CS6, CS7, CS8};
	ttyinfo.c_cflag &= ~CSIZE;
	ttyinfo.c_cflag |= DATABIT_CFLAGS[databits];

	// disable IGNBRK for mismatched speed tests;
	// otherwise receive break as \000 chars

	// disable break processing
	ttyinfo.c_iflag &= ~IGNBRK;

	// no signaling chars, no echo, no canonical processing
	ttyinfo.c_lflag = 0;

	// no remapping, no delays
	ttyinfo.c_oflag = 0;

	// read does block
	ttyinfo.c_cc[VMIN]  = 1;
	// 0.5 seconds read timeout
	ttyinfo.c_cc[VTIME] = 5;

	// shut off xon/xoff ctrl
	ttyinfo.c_iflag &= ~(IXON | IXOFF | IXANY);

	// ignore modem controls, enable reading
	ttyinfo.c_cflag |= (CLOCAL | CREAD);

	const int PARITY_CFLAGS[] =
	{
		0,
		PARENB,
		PARENB | PARODD,
		PARENB | PARODD | CMSPAR,
		PARENB | CMSPAR
	};

	// shut off parity
	ttyinfo.c_cflag &= ~(PARENB | PARODD | CMSPAR);
	// select parity
	ttyinfo.c_cflag |= PARITY_CFLAGS[parity];

	if(stopbits == connectionSettings::STOPBITS_2)
		ttyinfo.c_cflag |= CSTOPB;
	else // 1 stopbit
		ttyinfo.c_cflag &= ~CSTOPB;

	if(flowctrl == connectionSettings::FLOWCTRL_RTSCTS)
		ttyinfo.c_cflag |= CRTSCTS;
	else // no flow control
		ttyinfo.c_cflag &= ~CRTSCTS;

	if(tcsetattr(fd, TCSANOW, &ttyinfo) != 0)
	{
		::close(fd);
		fd = 0;
		return ERR_SET_ATTR;
	}
	return NOERROR;
}


