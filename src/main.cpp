/**********************************************************************\
 * MultiTerminal
 * main.cpp
 *
 * Program Entry Point
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
#include "gui/mainframe.h"

// Define a new application type, each program should derive a class from wxApp
class TerminalMain
	: public wxApp
{
public:
	// override base class virtuals
	// ----------------------------

	// this one is called on application startup and is a good place for the app
	// initialization (doing it here and not in the ctor allows to have an error
	// return: if OnInit() returns false, the application terminates)
	virtual bool OnInit() override;
};

bool TerminalMain::OnInit()
{
	mainframe* frame;
	try
	{
		frame = new mainframe();
	}
	catch(...)
	{
		return false;
	}
	SetTopWindow(frame);
	frame->Show(true);
	return true;
}

// program entry point (main())
IMPLEMENT_APP(TerminalMain)
