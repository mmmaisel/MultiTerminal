/**********************************************************************\
 * MultiTerminal
 * termInstance.cpp
 *
 * Terminal instance
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
#include "mainframe.h"
#include "backend.h"

#include "termInstance.h"
#include "resource/messages.h"
#include "resource/configFile.h"

bool termInstance::m_historyChanged = false;
mainframe* termInstance::m_pMainframe = 0;
termWindow* termInstance::m_pTermWnd = 0;
backend* termInstance::m_pBackend = 0;

termInstance::termInstance()
	: m_settings{0}
	, m_newDataSent(true)
	, m_pConnection(0)
	, m_rxCount(0)
	, m_txCount(0)
	, m_pinValues(0)
	, m_state(0)
	, m_logfile(0)
	, m_deleted(0)
	, m_onEnterSelection(0)
	, m_inputMode(0)
	, m_selection(0)
{
	m_outputBuffer.set_capacity(65535);

	m_settings.param		= 38400;
	m_settings.databits	= connectionSettings::DATABITS_8;
	m_settings.parity		= connectionSettings::PAR_NONE;
	m_settings.stopbits	= connectionSettings::STOPBITS_1;
	m_settings.flowctrl	= connectionSettings::FLOWCTRL_NONE;
	m_settings.iface_name	= "/dev/ttyACM0";
	m_settings.iface_type	= connectionSettings::IFACE_TYPE_SERIAL;

	m_onEnterSelection = 0;
	m_inputMode = 0;
	m_selection =
		(1 << VIEW_SHOW_ASC) |
		(1 << VIEW_SHOW_RX) |
		(1 << VIEW_SHOW_TX) |
		(1 << VIEW_SHOW_ERR) |
		(1 << VIEW_EN_AUTOSCROLL) |
		(1 << VIEW_EN_NEWLINE) |
		(1 << TERM_EN_HISTORY);
	m_newlineChar= '\n';
	m_maxLineLen = 16;
}

termInstance::~termInstance()
{
	if(m_pConnection)
	{
		delete m_pConnection;
		m_pConnection = 0;
	}
}

void termInstance::Select()
{
	m_pBackend->SetActiveInstance(this);

	m_pMainframe->SetInputValue(m_inputBuffer);
	m_pMainframe->SetOutputBuffer(&m_outputBuffer);

	m_pMainframe->UpdateConnectState(this);
	m_pMainframe->UpdateConnectionSettings(this);
	m_pMainframe->UpdateLogState(this);
	m_pMainframe->UpdateTerminalSettings(this);
	m_pMainframe->UpdatePinValue(this, -1);
	m_pMainframe->UpdateStatistics(this);
	m_pMainframe->UpdateViewSettings(this);
	m_pMainframe->UpdateStatusText(this);
}

void termInstance::Deselect()
{
	m_pBackend->SetActiveInstance(0);
	m_inputBuffer = m_pMainframe->GetInputValue();
}

void termInstance::Write(char* pData, size_t len)
{
	m_newDataSent = true;
	memcpy(pData + len, m_onEnterStr.data(), m_onEnterStr.size());
	len += m_onEnterStr.size();
	m_txCount += len;
	m_pBackend->Write(pData, len);
	InsertData(pData, len, multiBaseCommon::COLOR_TX);
	delete[] pData;
	m_pMainframe->UpdateStatistics(this);
}

void termInstance::Connect()
{
	m_state |= STATE_CONNECTING;
	connectionSettings* pSettings = new connectionSettings;
	*pSettings = m_settings;
	m_pBackend->Connect(pSettings);
	// Continues in OnBackendEvent()
}

void termInstance::Disconnect()
{
	m_pBackend->Disconnect();
	// Continues in OnCmdComplete()
}

void termInstance::ClearTerminal()
{
	m_outputBuffer.clear();
}

void termInstance::TransmitFile(const wxString& path)
{
	FILE* file = fopen(path.ToUTF8().data(), "rb");

	if(!file)
	{
		m_statusText = wxString::Format(MSG_FILE_NOT_FOUND, path);
		wxMessageBox(m_statusText, MSG_ERROR, wxICON_ERROR);
	}
	else
	{
		m_statusText = wxString::Format(MSG_STATUS_TX_FILE, path);
		m_pBackend->TransmitFile(file);
	}
	m_pMainframe->UpdateStatusText(this);
}

void termInstance::SaveOutput(const wxString& path, bool append)
{
	FILE* pFile;

	const char* mode;
	if(append)
	{
		mode = "ab";
		m_state |= LOG_APPEND_TO_FILE;
	}
	else
	{
		mode = "wb";
		m_state &= ~LOG_APPEND_TO_FILE;
	}

	pFile = fopen(path.ToUTF8().data(), mode);

	if(!pFile)
	{
		m_statusText = wxString::Format(MSG_SAVE_LOG_ERR, path);
		wxMessageBox(m_statusText, MSG_ERROR, wxICON_ERROR);
	}
	else
	{
		m_statusText = wxString::Format(MSG_STATUS_SAVE_LOG, path);
	}

	for(const auto& it : m_outputBuffer)
		fputc(it.c, pFile);

	fflush(pFile);
	fclose(pFile);
	pFile = 0;
	m_pMainframe->UpdateStatusText(this);
}

void termInstance::StartLogging(const wxString& path, bool append)
{
	// Abort if already logging
	if(m_state & STATE_LOGGING)
		return;

	const char* mode;
	if(append)
	{
		mode = "ab";
		m_state |= LOG_APPEND_TO_FILE;
	}
	else
	{
		mode = "wb";
		m_state &= ~LOG_APPEND_TO_FILE;
	}

	m_logfile = fopen(path.ToUTF8().data(), mode);

	if(!m_logfile)
	{
		m_statusText = wxString::Format(MSG_START_LOG_ERR, path);
		wxMessageBox(m_statusText, MSG_ERROR, wxICON_ERROR);
	}
	else
	{
		m_state |= STATE_LOGGING;
		m_statusText = wxString::Format(MSG_STATUS_START_LOG, path);
	}
	m_pMainframe->UpdateLogState(this);
	m_pMainframe->UpdateStatusText(this);
}

void termInstance::StopLogging()
{
	// Abort if not logging
	if(!(m_state & STATE_LOGGING))
		return;

	fflush(m_logfile);
	fclose(m_logfile);
	m_logfile = 0;

	m_state &= ~STATE_LOGGING;
	m_statusText = MSG_STATUS_STOP_LOG;
	m_pMainframe->UpdateLogState(this);
	m_pMainframe->UpdateStatusText(this);
}

void termInstance::ReadInputPins()
{
	if(!(m_state & STATE_CONNECTED))
		return;
	m_pBackend->ReadPins();
}

void termInstance::ToggleOutputPin(int pin)
{
	if(!(m_state & STATE_CONNECTED))
		return;

	if(pin == TIOCM_DTR)
		m_pBackend->SetPin(TIOCM_DTR, m_pinValues & TIOCM_DTR ? 0 : 1);
	else if(pin == TIOCM_RTS)
		m_pBackend->SetPin(TIOCM_RTS, m_pinValues & TIOCM_RTS ? 0 : 1);
}

void termInstance::SetOnEnterStr(int id)
{
	m_onEnterSelection = id;

	switch(id)
	{
		default:
		case TERM_SEND_NONE:
			m_onEnterStr.clear();
			break;
		case TERM_SEND_LF:
			m_onEnterStr.clear();
			m_onEnterStr.push_back('\n');
			break;
		case TERM_SEND_CR:
			m_onEnterStr.clear();
			m_onEnterStr.push_back('\r');
			break;
		case TERM_SEND_CRLF:
			m_onEnterStr.clear();
			m_onEnterStr.push_back('\r');
			m_onEnterStr.push_back('\n');
			break;
		case TERM_SEND_NULL:
			m_onEnterStr.clear();
			m_onEnterStr.push_back(0x00);
			break;
	}
}

void termInstance::SetInputMode(int mode)
{
	m_inputMode = mode;
}

void termInstance::EnableOutputBase(int base, bool enable)
{
	if(enable)
		m_selection |= (1 << (VIEW_SHOW_ASC + base));
	else
		m_selection &= ~(1 << (VIEW_SHOW_ASC + base));
}

void termInstance::EnableCharClass(int charClass, bool enable)
{
	if(enable)
		m_selection |= (1 << (VIEW_SHOW_RX + charClass));
	else
		m_selection &= ~(1 << (VIEW_SHOW_RX + charClass));
}

void termInstance::EnableHistory(bool enable)
{
	if(enable)
		m_selection |=  (1 << TERM_EN_HISTORY);
	else
		m_selection &= ~(1 << TERM_EN_HISTORY);
}

void termInstance::SetHistorySize(int size)
{
	m_history.SetMaxSize(size);
}

void termInstance::EnableAutoscroll(bool enable)
{
	if(enable)
		m_selection |=  (1 << VIEW_EN_AUTOSCROLL);
	else
		m_selection &= ~(1 << VIEW_EN_AUTOSCROLL);
}

void termInstance::SetNewlineChar(uint8_t newlineChar, bool enable)
{
	m_newlineChar = newlineChar;
	if(enable)
		m_selection |= (1 << VIEW_EN_NEWLINE);
	else
		m_selection &= ~(1 << VIEW_EN_NEWLINE);
}

void termInstance::SetMaxLineLength(int len, bool enable)
{
	m_maxLineLen = len;
	if(enable)
		m_selection |= (1 << VIEW_EN_NEWLINE_EVERY_N);
	else
		m_selection &= ~(1 << VIEW_EN_NEWLINE_EVERY_N);
}

void termInstance::ClearRxCount()
{
	m_rxCount = 0;
	m_pMainframe->UpdateStatistics(this);
}

void termInstance::ClearTxCount()
{
	m_txCount = 0;
	m_pMainframe->UpdateStatistics(this);
}

void termInstance::AddToHistory(std::string&& str)
{
	m_history.Insert(str);
	m_historyChanged = true;
}

std::string termInstance::GetNextFromHistory()
{
	if(m_newDataSent)
		m_newDataSent = false;
	else
		m_history.Next();
	return m_history.GetCurrent();
}

std::string termInstance::GetPrevFromHistory()
{
	if(m_newDataSent)
		m_newDataSent = false;
	else
		m_history.Previous();
	return m_history.GetCurrent();
}

void termInstance::LoadYaml(const YAML::Node& nInstance)
{
	std::string utf8str;
	wxString wxstr;

	utf8str = nInstance[NODE_PRESET_NAME].as<std::string>();
	wxstr = wxString(utf8str.c_str(), wxConvUTF8);
	m_name = wxstr;

	utf8str = nInstance[NODE_PRESET_IFTYPE].as<std::string>();
	if(utf8str == NODE_PRESET_IFTYPE_SERIAL)
	{
		m_settings.iface_type = connectionSettings::IFACE_TYPE_SERIAL;
	}
	else if(utf8str == NODE_PRESET_IFTYPE_TCP)
	{
		m_settings.iface_type = connectionSettings::IFACE_TYPE_TCP;
	}
	else
	{
		m_settings.iface_type = connectionSettings::IFACE_TYPE_UNKNOWN;
	}

	utf8str = nInstance[NODE_PRESET_IFNAME].as<std::string>();
	wxstr = wxString(utf8str.c_str(), wxConvUTF8);
	m_settings.iface_name = wxstr;

	m_settings.param = nInstance[NODE_PRESET_PARAM].as<int>();
	utf8str = nInstance[NODE_PRESET_FFORMAT].as<std::string>();
	m_settings.FromString(utf8str.c_str());

	SetOnEnterStr(nInstance[NODE_PRESET_ON_ENTER].as<int>());
	m_inputMode =
		nInstance[NODE_PRESET_INPUT_MODE].as<int>();
	m_selection =
		nInstance[NODE_PRESET_SELECTION].as<int>();
	m_history.SetMaxSize(
		nInstance[NODE_PRESET_HISTORY_LEN].as<int>());
	m_newlineChar =
		nInstance[NODE_PRESET_NEWLINE].as<int>();
	m_maxLineLen =
		nInstance[NODE_PRESET_LINESIZE].as<int>();

	utf8str = nInstance[NODE_PRESET_LOGDIR].as<std::string>();
	m_logDir = wxString(utf8str.c_str(), wxConvUTF8);

	// Read input history
	const YAML::Node& nHistory = nInstance[NODE_PRESET_HISTORY];
	for(size_t i = nHistory.size(); i > 0; --i)
	{
		std::string str(nHistory[i - 1].as<std::string>());
		m_history.Insert(std::move(str));
	}
}

void termInstance::SaveYaml(YAML::Node& nInstance)
{
	nInstance[NODE_PRESET_NAME] = m_name.ToUTF8().data();

	if(m_settings.iface_type == connectionSettings::IFACE_TYPE_SERIAL)
		nInstance[NODE_PRESET_IFTYPE] = NODE_PRESET_IFTYPE_SERIAL;
	else if(m_settings.iface_type == connectionSettings::IFACE_TYPE_TCP)
		nInstance[NODE_PRESET_IFTYPE] = NODE_PRESET_IFTYPE_TCP;
	else
		nInstance[NODE_PRESET_IFTYPE] = NODE_PRESET_IFTYPE_UNKNOWN;

	nInstance[NODE_PRESET_IFNAME]	= m_settings.iface_name.ToUTF8().data();
	nInstance[NODE_PRESET_PARAM]	= m_settings.param;

	char buffer[5];
	m_settings.ToString(buffer);
	nInstance[NODE_PRESET_FFORMAT]		= buffer;

	nInstance[NODE_PRESET_ON_ENTER]		= m_onEnterSelection;
	nInstance[NODE_PRESET_INPUT_MODE]	= m_inputMode;
	nInstance[NODE_PRESET_SELECTION]	= m_selection;
	nInstance[NODE_PRESET_HISTORY_LEN]	= m_history.GetMaxSize();
	nInstance[NODE_PRESET_LOGDIR]		= m_logDir.ToUTF8().data();
	nInstance[NODE_PRESET_NEWLINE]		= static_cast<int>(m_newlineChar);
	nInstance[NODE_PRESET_LINESIZE]		= m_maxLineLen;

	// Write input history, history is destroyed in this process
	m_history.First();
	for(size_t x = 0;
		x < m_history.size();
		++x, m_history.Next())
	{
		nInstance[NODE_PRESET_HISTORY].push_back
			(m_history.GetCurrent());
	}
}

void termInstance::WriteFormattedYAML
	(YAML::Emitter& ymlEmitter, const YAML::Node& node)
{
	ymlEmitter << YAML::BeginMap;
	ymlEmitter << YAML::Key << NODE_PRESET_NAME <<
		YAML::Value << node[NODE_PRESET_NAME];
	ymlEmitter << YAML::Key << NODE_PRESET_IFTYPE <<
		YAML::Value << node[NODE_PRESET_IFTYPE];
	ymlEmitter << YAML::Key << NODE_PRESET_IFNAME <<
		YAML::Value << node[NODE_PRESET_IFNAME];
	ymlEmitter << YAML::Key << NODE_PRESET_PARAM <<
		YAML::Value << node[NODE_PRESET_PARAM];
	ymlEmitter << YAML::Key << NODE_PRESET_FFORMAT <<
		YAML::Value << node[NODE_PRESET_FFORMAT];
	ymlEmitter << YAML::Key << NODE_PRESET_ON_ENTER <<
		YAML::Value << node[NODE_PRESET_ON_ENTER];
	ymlEmitter << YAML::Key << NODE_PRESET_INPUT_MODE <<
		YAML::Value << node[NODE_PRESET_INPUT_MODE];
	ymlEmitter << YAML::Key << NODE_PRESET_SELECTION <<
		YAML::Value << node[NODE_PRESET_SELECTION];
	ymlEmitter << YAML::Key << NODE_PRESET_HISTORY_LEN <<
		YAML::Value << node[NODE_PRESET_HISTORY_LEN];
	ymlEmitter << YAML::Key << NODE_PRESET_NEWLINE <<
		YAML::Value << node[NODE_PRESET_NEWLINE];
	ymlEmitter << YAML::Key << NODE_PRESET_LINESIZE <<
		YAML::Value << node[NODE_PRESET_LINESIZE];

	// Write logdir
	ymlEmitter << YAML::Key << NODE_PRESET_LOGDIR <<
		YAML::Value << node[NODE_PRESET_LOGDIR];

	// Write input history
	ymlEmitter << YAML::Key << NODE_PRESET_HISTORY <<
		YAML::Value << YAML::BeginSeq;
	for(const auto& hist : node[NODE_PRESET_HISTORY])
		ymlEmitter << hist;
	ymlEmitter << YAML::EndSeq;

	ymlEmitter << YAML::EndMap;
}

void termInstance::OnBackendEvent(backendEvent& evt)
{
	int id = evt.GetId();

	switch(id)
	{
		case backend::EVT_BACKEND_ERROR:
			m_statusText = std::move(evt.GetString());
			wxMessageBox(m_statusText, MSG_ERROR, wxICON_ERROR);
			m_pMainframe->Close();
			break;

		case backend::CMD_CONNECT:
			m_statusText = std::move(evt.GetString());
			if(evt.GetInt() < 0)
			{
				wxMessageBox(m_statusText,
					wxString::Format(MSG_INST_ERROR, m_name), wxICON_ERROR);
				m_state &= ~(STATE_CONNECTED | STATE_CONNECTING);
			}
			else
			{
				m_state &= ~STATE_CONNECTING;
				m_state |= STATE_CONNECTED;
			}
			m_pMainframe->UpdateConnectState(this);
			m_pMainframe->UpdateStatusText(this);
			break;

		case backend::CMD_DISCONNECT:
			m_statusText = std::move(evt.GetString());
			m_state &= ~(STATE_CONNECTED | STATE_CONNECTING);
			m_pMainframe->UpdateConnectState(this);
			m_pMainframe->UpdateStatusText(this);
			break;

		case backend::CMD_SET_PIN:
		{
			int* pData = evt.GetPayload<int*>();
			int pinMask = pData[0];
			int buttonID;

			m_statusText = std::move(evt.GetString());
			if(evt.GetInt() < 0)
			{
				wxMessageBox(m_statusText,
					wxString::Format(MSG_INST_ERROR, m_name), wxICON_ERROR);
				delete[] pData;
				break;
			}

			if(pData[1])
				m_pinValues |= pData[0];
			else
				m_pinValues &= ~pData[0];

			m_pMainframe->UpdatePinValue(this, pinMask);
			m_pMainframe->UpdateStatusText(this);

			delete[] pData;
			break;
		}

		case backend::CMD_READ_PINS:
		case backend::CMD_READ_PINS_TX:
		case backend::CMD_READ_PINS_QUIET:
		{
			if(evt.GetInt() < 0)
			{
				m_statusText = std::move(evt.GetString());
				wxMessageBox(m_statusText,
					wxString::Format(MSG_INST_ERROR, m_name), wxICON_ERROR);
				break;
			}

			int values = evt.GetPayload<int>();

			if(values != m_pinValues)
			{
				m_pinValues = values;

				// Event caused by user input: update status text
				if(id == backend::CMD_READ_PINS)
					m_statusText = std::move(evt.GetString());

				int pinMask = TIOCM_CTS | TIOCM_DSR | TIOCM_CAR | TIOCM_RNG;

				// Event caused by connect: update output pins as well
				if(id == backend::CMD_READ_PINS_TX)
					pinMask |= TIOCM_RTS | TIOCM_DTR;

				m_pMainframe->UpdatePinValue(this, pinMask);
				m_pMainframe->UpdateStatusText(this);
			}
			break;
		}

		case backend::EVT_READ:
		{
			int len = evt.GetInt();
			char* pData = evt.GetPayload<char*>();

			m_rxCount += len;
			InsertData(pData, len, multiBaseCommon::COLOR_RX);
			delete[] pData;
			m_pMainframe->UpdateStatistics(this);
			break;
		}

		case backend::EVT_WRITE:
			// unused
			break;

		case backend::EVT_READ_ERR:
		case backend::EVT_WRITE_ERR:
		case backend::EVT_DISCONNECT:
		{
			const wxString& wxmsg = evt.GetString();
			std::string stdmsg = wxmsg.ToStdString();

			wxMessageBox(wxmsg,
				wxString::Format(MSG_INST_ERROR, m_name), wxICON_ERROR);
			InsertData(stdmsg.c_str(), stdmsg.size(), multiBaseCommon::COLOR_ERR);
			break;
		}

		case backend::EVT_TX_FILE:
		{
			wxMessageBox(evt.GetString(), MSG_INFO, wxICON_INFORMATION);
			break;
		}

		default:
			wxMessageBox(MSG_UNKNOWN_CMDEVT_ERR);
			break;
	}
}

void termInstance::InsertData(const char* pData, size_t len, int colId)
{
	for(size_t x = 0; x < len; ++x)
		m_outputBuffer.push_back(term_char(pData[x], colId));

	if(m_state & STATE_LOGGING)
		fwrite(pData, 1, len, m_logfile);

	m_pMainframe->UpdateTerminalOutput
		(this, IsSelected(VIEW_EN_AUTOSCROLL));
}

