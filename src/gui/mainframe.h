/**********************************************************************\
 * MultiTerminal
 * mainframe.h
 *
 * Mainframe
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

#include "multiBaseInput.h"
#include "multiBaseOutput.h"
#include "backend.h"

class mainframe
	: public wxFrame
{
	public:
		mainframe();
		mainframe(const mainframe&) = delete;
		mainframe(mainframe&&) = delete;
		~mainframe();

		enum eIDs
		{
			// Home ribbon page
			ID_MAIN_MENU = wxID_HIGHEST + 1,
			ID_ADD_PRESET,
			ID_RENAME_PRESET,
			ID_DEL_PRESET,
			ID_PRESET_SELECT,

			ID_CONNECTION_TYPE,
			ID_IFACE_NAME,
			ID_PARAM,
			ID_DATABITS,
			ID_PARITY,
			ID_STOPBITS,
			ID_FLOWCTRL,

			// Terminal ribbon page
			ID_CONNECT_DISCONNECT,
			ID_CLRTERM,
			ID_TX_FILE,
			ID_SAVE_OUTPUT,
			ID_START_STOP_LOG,

			ID_ON_ENTER_SEL,
			ID_INPUT_FORMAT,
			ID_SHOW_HEX,
			ID_SHOW_ASC,
			ID_SHOW_DEC,
			ID_SHOW_OCT,
			ID_SHOW_BIN,
			ID_SHOW_RX,
			ID_SHOW_TX,
			ID_SHOW_ERR,
			ID_ENABLE_HISTORY,
			ID_HISTORY_LENGTH,
			ID_ENABLE_AUTOSCROLL,
			ID_NEWLINE_SEL,
			ID_NEWLINE_EVERY_N_ENABLE,
			ID_NEWLINE_EVERY_N_SEL,

			ID_LINECTRL_DSR,
			ID_LINECTRL_CTS,
			ID_LINECTRL_DCD,
			ID_LINECTRL_RNG,

			ID_LINECTRL_DTR,
			ID_LINECTRL_RTS,

			ID_CLEAR_STAT_RX,
			ID_CLEAR_STAT_TX,
			ID_STAT_RX,
			ID_STAT_TX,

			ID_TERM_OUTPUT,
			ID_TERM_INPUT,
			ID_REDRAW_LIMIT_TIMER,

			ID_ACCEL_UNDO,
			ID_ACCEL_ASCII,
			ID_ACCEL_BIN,
			ID_ACCEL_OCT,
			ID_ACCEL_DEC,
			ID_ACCEL_HEX
		};

		// Called if termInstance changes to update GUI.
		void UpdateStatusText(const termInstance* pSrc);
		void UpdateTerminalOutput(const termInstance* pSrc, bool scrollDown);
		void UpdateConnectState(const termInstance* pSrc);
		void UpdateConnectionSettings(const termInstance* pSrc);
		void UpdateLogState(const termInstance* pSrc);
		void UpdateTerminalSettings(const termInstance* pSrc);
		void UpdatePinValue(const termInstance* pSrc, int pins);
		void UpdateStatistics(const termInstance* pSrc);
		void UpdateViewSettings(const termInstance* pSrc);

		inline void SetInputValue(const std::string& val)
			{ m_pTermInputBox->SetValue(val); }
		inline void SetOutputBuffer(
			const boost::circular_buffer<term_char>* pBuffer)
		{
			m_pTermOutputBox->SetBuffer(pBuffer);
			m_pTermOutputBox->Redraw();
		}
		inline std::string GetInputValue() const
			{ return std::move(m_pTermInputBox->GetValue()); }

	private:
		void OnClose(wxCloseEvent& event);

		// Ribbon events
		void OnAbout(wxRibbonBarEvent& evt);

		void OnAddPreset(wxRibbonButtonBarEvent& evt);
		void OnRenamePreset(wxRibbonButtonBarEvent& evt);
		void OnDeletePreset(wxRibbonButtonBarEvent& evt);

		void OnConnectDisconnect(wxRibbonButtonBarEvent& evt);
		void OnClrTerm(wxRibbonButtonBarEvent& evt);
		void OnTxFile(wxRibbonButtonBarEvent& evt);

		void OnPresetSelect(wxCommandEvent& evt);
		void OnConnectionSettingsChanged(wxCommandEvent& evt);

		void OnSaveTerm(wxRibbonButtonBarEvent& evt);
		void OnStartStopLog(wxRibbonButtonBarEvent& evt);

		void OnOnEnterSelect(wxCommandEvent& evt);
		void OnInputFormatSelect(wxCommandEvent& evt);
		void OnEnableHistory(wxCommandEvent& evt);
		void OnHistorySize(wxCommandEvent& evt);

		void OnReadInputPins(wxRibbonButtonBarEvent& evt);
		void OnToggleDTR(wxRibbonButtonBarEvent& evt);
		void OnToggleRTS(wxRibbonButtonBarEvent& evt);

		void OnClearStatRx(wxRibbonButtonBarEvent& evt);
		void OnClearStatTx(wxRibbonButtonBarEvent& evt);

		void OnShowBase(wxCommandEvent& evt);
		void OnShowCharClass(wxCommandEvent& evt);
		void OnEnableAutoscroll(wxCommandEvent& evt);
		void OnNewlineSelect(wxCommandEvent& evt);
		void OnMaxLineLen(wxCommandEvent& evt);

		void OnEnter(wxCommandEvent& evt);
		void OnUp(wxCommandEvent& evt);
		void OnDown(wxCommandEvent& evt);

		void OnRedrawLimitTimer(wxTimerEvent& evt);
		void OnAccelerator(wxCommandEvent& evt);
		void OnBackendEvent(backendEvent& evt);

		wxDECLARE_EVENT_TABLE();

		// Helper Functions
		void UpdatePresetList();
		void CreateInstance(wxString name);
		void SelectInstance(wxString name);
		void DeletePresetFromList();
		void UndoDelete();
		void LoadPresets();
		void SavePresets();
		void WriteFormattedYAML(YAML::Node& m_yamlDoc) const;
		bool IsPositiveNumber(wxString& str, long int* num);

		//
		// Variables
		//

		// GUI Elements
		wxStatusBar* m_pStatusBar;
		wxRibbonBar* m_pMenu;
		wxRibbonButtonBar* m_pPresetBar;
		wxRibbonButtonBar* m_pTermConnectBar;
		wxRibbonButtonBar* m_pTermLogBar;
		wxRibbonButtonBar* m_pLineCtrlInBar;
		wxRibbonButtonBar* m_pLineCtrlOutBar;
		wxRibbonButtonBar* m_pStatisticsBar;

		wxComboBox*   m_pPresetSelect;

		wxStaticText* m_pTextConnType;
		wxComboBox*   m_pIFaceTypeSelect;
		wxStaticText* m_pTextIFaceName;
		wxTextCtrl*   m_pIFaceNameInput;
		wxStaticText* m_pTextBaudRate;
		wxComboBox*   m_pParamSelect;
		wxStaticText* m_pTextFrameFormat;
		wxComboBox*   m_pDatabitsSelect;
		wxComboBox*   m_pParitySelect;
		wxComboBox*   m_pStopbitsSelect;
		wxComboBox*   m_pFlowctrlSelect;

		wxComboBox* m_pTermOnEnterSel;
		wxComboBox* m_pTermInputSel;
		wxCheckBox* m_pTermEnHistory;
		wxTextCtrl* m_pHistoryLength;

		wxCheckBox* m_pTermShowAsc;
		wxCheckBox* m_pTermShowHex;
		wxCheckBox* m_pTermShowDec;
		wxCheckBox* m_pTermShowOct;
		wxCheckBox* m_pTermShowBin;
		wxCheckBox* m_pTermEnAutoscroll;
		wxCheckBox* m_pTermShowRX;
		wxCheckBox* m_pTermShowTX;
		wxCheckBox* m_pTermShowERR;
		wxComboBox* m_pTermNewlineSel;
		wxCheckBox* m_pTermEnNewlineEveryN;
		wxTextCtrl* m_pTermNewlineEveryNSelect;

		wxTextCtrl* m_pStatRxCounter;
		wxTextCtrl* m_pStatTxCounter;

		multiBaseCommon m_colors;
		multiBaseOutput* m_pTermOutputBox;
		multiBaseInput* m_pTermInputBox;

		// Cached bitmaps
		wxImage m_imgConnect;
		wxImage m_imgDisconnect;
		wxImage m_imgStartLog;
		wxImage m_imgStopLog;
		wxImage m_imgPinActive;
		wxImage m_imgPinInactive;

		// Status Variables
		backend* m_pBackend;
		bool m_presetChanged;
		wxString m_configFilename;
		int m_undoCounter;
		termInstance* m_pCurrentInstance;
		std::list<termInstance> m_instances;

		wxTimer* m_pRedrawLimitTimer;
		bool m_redrawSuppressed;
		const int MAX_UPDATE_RATE_MS = 50;
};
