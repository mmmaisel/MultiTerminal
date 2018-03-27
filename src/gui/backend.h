/**********************************************************************\
 * MultiTerminal
 * backend.h
 *
 * Backend
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

#include "connection.h"
#include "termInstance.h"
#include "backendEvent.h"

// Backend runs task in another thread to keep
// gui responsive (eg. for progress/cancel)
class backend
//	: public IBackendTask
{
	public:
		backend(wxWindow* pParent);
		backend(const backend&) = delete;
		backend(backend&&) = delete;
		~backend();

		enum commandID
		{
			CMD_NOP, // Spurious wakeup
			CMD_SHUTDOWN,
			CMD_CONNECT,
			CMD_DISCONNECT,
			CMD_READ_PINS,
			CMD_READ_PINS_TX,
			CMD_READ_PINS_QUIET,
			CMD_SET_PIN,
			CMD_TX_FILE,
			CMD_SET_INSTANCE,

			EVT_READ,
			EVT_WRITE,
			EVT_READ_ERR,
			EVT_WRITE_ERR,
			EVT_DISCONNECT,
			EVT_TX_FILE,
			EVT_BACKEND_ERROR // Critical failure
		};

		void Connect(connectionSettings* pSettings);
		void Disconnect();
		void Read();
		void Write(const char* pData, size_t len);
		void ReadPins();
		void SetPin(int pin, int val);
		void TransmitFile(FILE* file);

		void SetActiveInstance(termInstance* pInstance);

	private:
		wxWindow* m_pParent;
		std::thread* m_pThread;
		mutable std::mutex m_mutex;

		enum { PIPE_READ, PIPE_WRITE };
		int m_pipeFd[2];
		std::vector<pollfd> m_pollfds;

		int m_cmd;
		termInstance* m_pCmdInstance;
		void* m_pCmdData;

		termInstance* m_pActiveInstance;
		std::map<int, termInstance*> m_pInstances;
		const int BUFFER_SIZE = 4096;

		void Run();

		void DoConnect();
		void DoDisconnect(termInstance* pInstance,
			bool error = false, bool popPollFd = true);
		void DoRead(termInstance* pInstance);
		void DoWrite();
		void DoReadPins(int idWithCause);
		void DoSetPin();
		void DoTransmitFile();
};
