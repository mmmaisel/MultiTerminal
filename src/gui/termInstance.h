/**********************************************************************\
 * MultiTerminal
 * termInstance.h
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
#pragma once

#include "connection.h"
#include "cmdHistory.h"
#include "termChar.h"
#include "backendEvent.h"

class mainframe;
class backend;
class termWindow;

class termInstance
{
	public:
		termInstance();
		termInstance(const termInstance&) = delete;
		termInstance(termInstance&&) = delete;
		~termInstance();

		enum selection
		{
			// Bases must be in same order as in multiBaseCommon !
			VIEW_SHOW_ASC,
			VIEW_SHOW_BIN,
			VIEW_SHOW_OCT,
			VIEW_SHOW_DEC,
			VIEW_SHOW_HEX,
			// Char classes must be in same order as in multiBaseCommon !
			VIEW_SHOW_RX,
			VIEW_SHOW_TX,
			VIEW_SHOW_ERR,
			VIEW_EN_AUTOSCROLL,
			VIEW_EN_NEWLINE,
			VIEW_EN_NEWLINE_EVERY_N,
			TERM_EN_HISTORY,
			LOG_APPEND_TO_FILE
		};

		enum onEnterStr
		{
			TERM_SEND_NONE,
			TERM_SEND_LF,
			TERM_SEND_CR,
			TERM_SEND_CRLF,
			TERM_SEND_NULL,

			MAX_ON_ENTER_STR_SIZE = 2
		};

		inline bool IsConnected() const
			{ return m_state & STATE_CONNECTED; }
		inline const connectionSettings& GetSettings() const
			{ return m_settings;}
		inline const wxString& GetStatusText() const
			{ return m_statusText; }
		inline int GetPinValue(int pin) const
			{ return m_pinValues & pin; }
		inline bool IsLogging() const
			{ return m_state & STATE_LOGGING; }
		inline bool AppendToFile() const
			{ return m_state & LOG_APPEND_TO_FILE; }
		inline int HistorySize() const
			{ return m_history.GetMaxSize(); }
		inline bool IsSelected(selection sel) const
			{ return m_selection & (1 << sel); }
		inline size_t GetRxCount() const
			{ return m_rxCount; }
		inline size_t GetTxCount() const
			{ return m_txCount; }

		void Select();
		void Deselect();
		void Write(char* pData, size_t len);

		void Connect();
		void Disconnect();
		void ClearTerminal();
		void TransmitFile(const wxString& path);
		void SaveOutput(const wxString& path, bool append);
		void StartLogging(const wxString& path, bool append);
		void StopLogging();
		void ReadInputPins();
		void ToggleOutputPin(int pin);

		void SetOnEnterStr(int id);
		void SetInputMode(int mode);
		void EnableOutputBase(int base, bool enable);
		void EnableCharClass(int charClass, bool enable);
		void EnableHistory(bool enable);
		void SetHistorySize(int size);
		void EnableAutoscroll(bool enable);
		void SetNewlineChar(uint8_t newlineChar, bool enable);
		void SetMaxLineLength(int len, bool enable);

		void ClearRxCount();
		void ClearTxCount();

		void AddToHistory(std::string&& str);
		std::string GetNextFromHistory();
		std::string GetPrevFromHistory();

		void LoadYaml(const YAML::Node& nInstance);
		void SaveYaml(YAML::Node& nInstance);
		static void WriteFormattedYAML
			(YAML::Emitter& ymlEmitter, const YAML::Node& node);

		wxString m_name;
		int m_deleted;
		wxString m_logDir;
		connectionSettings m_settings;
		int m_onEnterSelection;
		int m_inputMode;
		int m_selection;
		uint8_t m_newlineChar;
		int m_maxLineLen;
		connection* m_pConnection;

		static bool m_historyChanged;
		static mainframe* m_pMainframe;
		static termWindow* m_pTermWnd;
		static backend* m_pBackend;

		void OnBackendEvent(backendEvent& evt);

	private:
		size_t m_rxCount;
		size_t m_txCount;
		std::string m_inputBuffer;
		boost::circular_buffer<term_char> m_outputBuffer;
		uint8_t m_pinValues;
		int m_state;
		FILE* m_logfile;
		wxString m_statusText;
		std::vector<char> m_onEnterStr;

		/*mutable*/ cmdHistory m_history;
		bool m_newDataSent;

		enum state
		{
			STATE_CONNECTED	= (1 << 0),
			STATE_CONNECTING= (1 << 1),
			STATE_LOGGING	= (1 << 2)
		};

		void InsertData(const char* pData, size_t len, int colId);
};
