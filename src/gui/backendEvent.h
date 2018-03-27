/**********************************************************************\
 * MultiTerminal
 * backendEvent.h
 *
 * Backend event class
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

class termInstance;

class backendEvent;
wxDECLARE_EVENT(BACKEND_CMD_RESULT, backendEvent);

class backendEvent
	: public wxThreadEvent
{
	public:
		backendEvent(wxEventType commandType = BACKEND_CMD_RESULT, int id = 0)
			: wxThreadEvent(commandType, id) {}

		// You *must* copy here the data to be transported
		backendEvent(const backendEvent& event)
			: wxThreadEvent(event) { m_pInstance = event.m_pInstance; }

		// Required for sending with wxPostEvent()
		wxEvent* Clone() const { return new backendEvent(*this); }

		termInstance* GetInstance() { return m_pInstance; }
		void SetInstance(termInstance* pInst) { m_pInstance = pInst; }

	private:
		termInstance* m_pInstance;
};

// define an event table entry
typedef void (wxEvtHandler::*backendEventFunc)(backendEvent&);
#define BackendEventHandler(func) wxEVENT_HANDLER_CAST(backendEventFunc, func)
#define EVT_BACKEND_THREAD(id, type, func) \
	wx__DECLARE_EVT1(type, id, BackendEventHandler(func))
