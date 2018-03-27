/**********************************************************************\
 * MultiTerminal
 * backend.cpp
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
#include "stdafx.h"
#include "backend.h"

#include "resource/messages.h"

wxDEFINE_EVENT(BACKEND_CMD_RESULT, backendEvent);

backend::backend(wxWindow* pParent)
	: m_pParent(pParent)
	, m_pipeFd{0}
	, m_cmd(CMD_NOP)
	, m_pCmdData(0)
	, m_pActiveInstance(0)
{
	if(pipe(m_pipeFd) < 0)
		throw(std::runtime_error("Create backend pipe failed!"));

	m_pollfds.reserve(8);
	m_pollfds.resize(1);
	m_pThread = new std::thread(&backend::Run, this);
}

backend::~backend()
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_cmd = CMD_SHUTDOWN;
	m_pCmdData = 0;
	lk.unlock();

	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);

	m_pThread->join();
	delete m_pThread;

	close(m_pipeFd[PIPE_READ]);
	close(m_pipeFd[PIPE_WRITE]);
}

void backend::Connect(connectionSettings* pSettings)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_cmd = CMD_CONNECT;
	m_pCmdData = pSettings;
	lk.unlock();
	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);
}

void backend::Disconnect()
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_cmd = CMD_DISCONNECT;
	m_pCmdData = 0;
	lk.unlock();
	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);
}

void backend::ReadPins()
{
	std::unique_lock<std::mutex> lk(m_mutex);

	m_cmd = CMD_READ_PINS;
	m_pCmdData = 0;
	lk.unlock();
	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);
}

void backend::SetPin(int pin, int val)
{
	std::unique_lock<std::mutex> lk(m_mutex);

	int* pData = new int[2];
	pData[0] = pin;
	pData[1] = val;
	m_cmd = CMD_SET_PIN;
	m_pCmdData = pData;
	lk.unlock();
	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);
}

void backend::TransmitFile(FILE* file)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_cmd = CMD_TX_FILE;
	m_pCmdData = file;
	lk.unlock();
	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);
}

void backend::SetActiveInstance(termInstance* pInstance)
{
	std::unique_lock<std::mutex> lk(m_mutex);
	m_cmd = CMD_SET_INSTANCE;
	m_pCmdData = pInstance;
	lk.unlock();
	char dummy = 0;
	dummy = write(m_pipeFd[PIPE_WRITE], &dummy, 1);
}

void backend::Run()
{
	for(;;)
	{
		int result;

		do
		{
			m_pollfds[0].fd = m_pipeFd[PIPE_READ];
			m_pollfds[0].events = POLLIN;

			size_t pos = 1;
			for(const auto& instance : m_pInstances)
			{
				if(instance.second->IsConnected())
				{
					m_pollfds[pos].fd = instance.first;
					m_pollfds[pos].events = POLLIN;
					++pos;
				}
			}

			// To timeout to poll IO pins every 100 ms
			result = poll(m_pollfds.data(), m_pollfds.size(), 100);
		}
		// EINTR == interrupted by signal, this is not an error
		while(result == -1 && errno == EINTR);
		// result < 0: error, result == 0: spurious wakeup / timeout

		if(m_pActiveInstance)
		{
			if(m_pActiveInstance->IsConnected())
				DoReadPins(CMD_READ_PINS_QUIET);
		}

		if(result == 0)
			continue;
		else if(result < 0)
		{
			backendEvent* pEvent = new backendEvent(EVT_BACKEND_ERROR);
			pEvent->SetString(MSG_FATAL_BACKEND_ERR);
			wxQueueEvent(m_pParent, pEvent);
			return;
		}

		if(m_pollfds[0].revents & POLLIN)
		{
			// dummy read the data from pipe, it's for wakeup only
			char dummy;
			dummy = read(m_pipeFd[PIPE_READ], &dummy, 1);

			std::unique_lock<std::mutex> lk(m_mutex);
			switch(m_cmd)
			{
				case CMD_SHUTDOWN:
					return;

				case CMD_CONNECT:
					DoConnect();
					break;

				case CMD_DISCONNECT:
					DoDisconnect(m_pActiveInstance);
					break;

				case CMD_READ_PINS:
					DoReadPins(CMD_READ_PINS);
					break;

				case CMD_SET_PIN:
					DoSetPin();
					break;

				case CMD_TX_FILE:
					DoTransmitFile();
					break;

				case CMD_SET_INSTANCE:
					m_pActiveInstance =
						reinterpret_cast<termInstance*>(m_pCmdData);
					break;

				case CMD_NOP:
				default:
					break;
			}
			m_cmd = CMD_NOP;
		}

		// Skip first pollfd entry which is not a connection
		for(auto it = m_pollfds.cbegin()+1; it != m_pollfds.cend();
			/*No Increment!*/)
		{
			if(it->revents & POLLERR)
			{
				DoDisconnect(m_pInstances[it->fd], true, false);
				it = m_pollfds.erase(it);
			}
			else if(it->revents & POLLIN)
			{
				DoRead(m_pInstances[it->fd]);
				++it;
			}
			else
			{
				++it;
			}
		}
	}
}

void backend::DoConnect()
{
	int result = -1;
	bool alreadyConnected = false;
	backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
	pEvent->SetInstance(m_pActiveInstance);
	pEvent->SetId(CMD_CONNECT);

	connectionSettings* pSettings =
		reinterpret_cast<connectionSettings*>(m_pCmdData);

	for(const auto& instance: m_pInstances)
	{
		// TODO: instance.second belongs to another thread,
		// needs locking if you can change the settings from UI really fast!
		if(instance.second->m_settings.iface_name
		   == pSettings->iface_name)
		{
			alreadyConnected = true;
			break;
		}
	}

	if((m_pActiveInstance->m_settings.iface_type
	   == connectionSettings::IFACE_TYPE_SERIAL)
	   && !alreadyConnected)
	{
		serialConnection* pConnection = new serialConnection();
		pConnection->FromSettings(*pSettings);

		result = pConnection->open
			(m_pActiveInstance->m_settings.iface_name.ToStdString().c_str());
		m_pActiveInstance->m_pConnection = pConnection;
	}
	delete pSettings;

	wxString msg;
	if(alreadyConnected)
	{
		msg = wxString::Format(MSG_ALREADY_CONNECTED,
			m_pActiveInstance->m_settings.iface_name);
	}
	else if(result < 0)
	{
		if(result == serialConnection::ERR_CUSTOM_BAUD)
		{
			msg = wxString::Format(MSG_CONNECT_BAUD_ERR,
				m_pActiveInstance->m_settings.iface_name);
		}
		else
		{
			msg = wxString::Format(MSG_CONNECT_ERR,
				m_pActiveInstance->m_settings.iface_name, strerror(errno));
		}

		if(m_pActiveInstance->m_pConnection)
		{
			delete m_pActiveInstance->m_pConnection;
			m_pActiveInstance->m_pConnection = 0;
		}
	}
	else
	{
		msg = wxString::Format(MSG_STATUS_CONNECTED,
			m_pActiveInstance->m_settings.iface_name);
		m_pInstances[m_pActiveInstance->m_pConnection->fd] = m_pActiveInstance;
		m_pollfds.push_back({0});
	}

	pEvent->SetInt(result);
	pEvent->SetString(std::move(msg));
	wxQueueEvent(m_pParent, pEvent);

	if(result >= 0)
		DoReadPins(CMD_READ_PINS_TX);
}

void backend::DoDisconnect(termInstance* pInstance, bool error, bool popPollFd)
{
	int result = -1;
	backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
	pEvent->SetInstance(pInstance);
	pEvent->SetId(CMD_DISCONNECT);

	result = close(pInstance->m_pConnection->fd);
	m_pInstances.erase(pInstance->m_pConnection->fd);
	if(popPollFd)
		m_pollfds.pop_back();
	delete pInstance->m_pConnection;
	pInstance->m_pConnection = 0;

	pEvent->SetInt(result);
	pEvent->SetString(MSG_STATUS_DISCONNECTED);
	wxQueueEvent(m_pParent, pEvent);

	if(error || result < 0)
	{
		wxString msg = wxString::Format
			(MSG_DISCONNECT_ERR, strerror(errno));

		backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
		pEvent->SetInstance(pInstance);
		pEvent->SetId(EVT_DISCONNECT);
		pEvent->SetInt(result);
		pEvent->SetString(std::move(msg));
		wxQueueEvent(m_pParent, pEvent);
	}
}

void backend::DoRead(termInstance* pInstance)
{
	int result = -1;
	backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
	pEvent->SetInstance(pInstance);

	char* pBuffer = new char[BUFFER_SIZE];
	result = read(pInstance->m_pConnection->fd, pBuffer, BUFFER_SIZE);
	pEvent->SetInt(result);

	if(result < 0)
	{
		wxString msg = wxString::Format(MSG_READ_ERR, strerror(errno));
		pEvent->SetId(EVT_READ_ERR);
		pEvent->SetString(std::move(msg));
		delete[] pBuffer;
	}
	else
	{
		pEvent->SetId(EVT_READ);
		pEvent->SetPayload(pBuffer);
	}

	wxQueueEvent(m_pParent, pEvent);

	if(result < 0)
		DoDisconnect(pInstance);
}

void backend::Write(const char* pData, size_t len)
{
	bool isConnected = false;
	int result = -1;
	if(m_pActiveInstance)
	{
		if(m_pActiveInstance->IsConnected())
		{
			isConnected = true;
			result = write(m_pActiveInstance->m_pConnection->fd, pData, len);
		}
	}

	if(result != len)
	{
		wxString msg;
		if(!isConnected)
		{
			msg = wxString::Format
				(MSG_WRITE_ERR, MSG_NOT_CONNECTED, 0);
		}
		else
		{
			msg = wxString::Format
				(MSG_WRITE_ERR, strerror(errno), result);
		}

		backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
		pEvent->SetInstance(m_pActiveInstance);
		pEvent->SetId(EVT_WRITE_ERR);
		pEvent->SetInt(result);
		pEvent->SetString(std::move(msg));
		wxQueueEvent(m_pParent, pEvent);

		DoDisconnect(m_pActiveInstance);
	}
}

void backend::DoReadPins(int idWithCause)
{
	bool isConnected = false;
	int result = -1;
	int value = 0;
	if(m_pActiveInstance->m_settings.iface_type
	   == connectionSettings::IFACE_TYPE_SERIAL)
	{
		serialConnection* pConnection =
			static_cast<serialConnection*>(m_pActiveInstance->m_pConnection);
		if(pConnection)
		{
			if(pConnection->fd)
				isConnected = true;
			result = pConnection->GetPinValues(value);
		}
	}

	if(!(idWithCause == CMD_READ_PINS_QUIET && !isConnected))
	{
		wxString msg;
		if(result < 0)
			msg = wxString::Format(MSG_READ_PIN_ERR, strerror(errno));
		else
			msg = MSG_STATUS_READ_PIN;

		backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
		pEvent->SetInstance(m_pActiveInstance);
		pEvent->SetId(idWithCause);
		pEvent->SetInt(result);
		pEvent->SetString(std::move(msg));
		pEvent->SetPayload(value);
		wxQueueEvent(m_pParent, pEvent);
	}
	// else: connection is disconnecting but
	//       Disconnect event has not propagated yet -> ignore this
}

void backend::DoSetPin()
{
	int* pData = reinterpret_cast<int*>(m_pCmdData);
	int result = -1;
	if(m_pActiveInstance->m_settings.iface_type
	   == connectionSettings::IFACE_TYPE_SERIAL)
	{
		serialConnection* pConnection =
			static_cast<serialConnection*>(m_pActiveInstance->m_pConnection);
		if(pConnection)
		{
			int pin = pData[0];
			int val = pData[1];
			result = pConnection->SetPinValue(pin, val);
		}
	}

	wxString msg;
	const char* pinName = "unknown";
	if(pData[0] == TIOCM_DTR)
		pinName = "DTR";
	else if(pData[0] == TIOCM_RTS)
		pinName = "RTS";

	if(result < 0)
		msg = wxString::Format(MSG_SET_PIN_ERR, pinName, strerror(errno));
	else
		msg = wxString::Format(MSG_STATUS_SET_PIN, pinName);

	backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
	pEvent->SetInstance(m_pActiveInstance);
	pEvent->SetId(CMD_SET_PIN);
	pEvent->SetInt(result);
	pEvent->SetString(std::move(msg));
	pEvent->SetPayload(pData);
	wxQueueEvent(m_pParent, pEvent);
}

void backend::DoTransmitFile()
{
	bool isConnected = false;
	int result = 0;
	FILE* file = reinterpret_cast<FILE*>(m_pCmdData);

	if(!m_pActiveInstance)
	{
		fclose(file);
		result = -1;
	}
	// Only dereference m_pActiveInstance if it is non NULL
	else if(!m_pActiveInstance->IsConnected())
	{
		fclose(file);
		result = -1;
	}
	else
	{
		int fd = m_pActiveInstance->m_pConnection->fd;
		const int BUFFER_SIZE = 65536;
		uint8_t* pBuffer = new uint8_t[BUFFER_SIZE];
		size_t readCount;
		isConnected = true;

		do
		{
			readCount = fread(pBuffer, 1, BUFFER_SIZE, file);
			if(readCount != BUFFER_SIZE)
			{
				if(ferror(file))
				{
					result = -1;
					break;
				}
			}

			result = write(fd, pBuffer, readCount);
			if(result != readCount)
			{
				result = -1;
				break;
			}
		}
		while(readCount == BUFFER_SIZE);

		delete[] pBuffer;
		fclose(file);
	}

	backendEvent* pEvent = new backendEvent(BACKEND_CMD_RESULT);
	pEvent->SetInstance(m_pActiveInstance);
	pEvent->SetInt(result);

	wxString msg;
	if(result < 0)
	{
		if(!isConnected)
		{
			msg = wxString::Format
				(MSG_TX_FILE_ERR, MSG_NOT_CONNECTED);
		}
		else
		{
			msg = wxString::Format
				(MSG_TX_FILE_ERR, strerror(errno));
		}

		pEvent->SetId(EVT_WRITE_ERR);
	}
	else
	{
		msg = MSG_TX_FILE_COMPLETE;
		pEvent->SetId(EVT_TX_FILE);
	}

	pEvent->SetString(std::move(msg));
	wxQueueEvent(m_pParent, pEvent);

	if(result < 0)
		DoDisconnect(m_pActiveInstance);
}
