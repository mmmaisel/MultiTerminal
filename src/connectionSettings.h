/**********************************************************************\
 * MultiTerminal
 * connectionSettings.h
 *
 * Connection settings struct
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

struct connectionSettings
{
	enum eIfaceType
	{
		IFACE_TYPE_UNKNOWN,
		IFACE_TYPE_SERIAL,
		IFACE_TYPE_TCP
	};

	enum eDatabits
	{
		DATABITS_5,
		DATABITS_6,
		DATABITS_7,
		DATABITS_8,
	};

	enum eParity
	{
		PAR_NONE,
		PAR_EVEN,
		PAR_ODD,
		PAR_MARK,
		PAR_SPACE
	};

	enum eStopbits
	{
		STOPBITS_1,
		STOPBITS_2
	};

	enum eFlowCtrl
	{
		FLOWCTRL_NONE,
		FLOWCTRL_RTSCTS,
		FLOWCTRL_DSRDTR
	};

	int iface_type;
	wxString iface_name;
	int param;
	int databits;
	int parity;
	int stopbits;
	int flowctrl;

	inline void FromString(const char* pStr)
	{
		if(pStr[0] >= '5' && pStr[0] <= '8')
			databits = pStr[0] - '5';
		else
			databits = DATABITS_8;

		if(pStr[2] >= '1' && pStr[2] <= '2')
			stopbits = pStr[2] - '1';
		else
			stopbits = STOPBITS_1;

		switch(pStr[1])
		{
			case 'N': parity = PAR_NONE; break;
			case 'E': parity = PAR_EVEN; break;
			case 'O': parity = PAR_ODD; break;
			case 'M': parity = PAR_MARK; break;
			case 'S': parity = PAR_SPACE; break;
			default:  parity = PAR_NONE; break;
		}
		switch(pStr[3])
		{
			case 'N': flowctrl = FLOWCTRL_NONE; break;
			case 'R': flowctrl = FLOWCTRL_RTSCTS; break;
			case 'D': flowctrl = FLOWCTRL_DSRDTR; break;
			default:  flowctrl = FLOWCTRL_NONE; break;
		}
	}

	inline void ToString(char* pStr) const
	{
		const char PARITY_VALUES[] = "NEOMS";
		const char FLOWCTRL_VALUES[] = "NRD";

		if(databits >= DATABITS_5 && databits <= DATABITS_8)
			pStr[0] = '5' + databits;
		else
			pStr[0] = '8';

		if(parity >= PAR_NONE && parity <= PAR_SPACE)
			pStr[1] = PARITY_VALUES[parity];
		else
			pStr[1] = 'N';

		if(stopbits >= STOPBITS_1 && stopbits <= STOPBITS_2)
			pStr[2] = '1' + stopbits;
		else
			pStr[2] = '1';

		if(flowctrl >= FLOWCTRL_NONE && flowctrl <= FLOWCTRL_DSRDTR)
			pStr[3] = FLOWCTRL_VALUES[flowctrl];
		else
			pStr[3] = 'N';

		pStr[4] = 0;
	}
};
