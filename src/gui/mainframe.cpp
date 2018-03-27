/**********************************************************************\
 * MultiTerminal
 * mainframe.cpp
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
#include "stdafx.h"
#include "termInstance.h"

#include "mainframe.h"
#include "termInstance.h"
#include "aboutDlg.h"
#include "termFileDlgExtra.h"

#include "resource/messages.h"
#include "resource/icons.h"
#include "resource/configFile.h"

/**********************************************************************\
 * Performs all initializations and creates GUI
\**********************************************************************/

mainframe::mainframe()
	: wxFrame(0, wxID_ANY, WINDOW_NAME, wxDefaultPosition,
		wxSize(1024, 720), wxDEFAULT_FRAME_STYLE)
	, m_pBackend(0)
	, m_presetChanged(false)
	, m_undoCounter(0)
	, m_pCurrentInstance(0)
	, m_redrawSuppressed(false)
{
	wxImage::AddHandler(new wxPNGHandler);

	/**********************************************************************\
	 * GUI Creation
	\**********************************************************************/

	// Load and set program icon
	SetIcon(wxIcon(ICON_MULTITERM, wxBITMAP_TYPE_PNG));

	// Create status bar
	m_pStatusBar = new wxStatusBar(this);
	SetStatusBar(m_pStatusBar);

	// Load mutable button images
	m_imgConnect    .LoadFile(ICON_TERM_CONNECT,    wxBITMAP_TYPE_PNG);
	m_imgDisconnect .LoadFile(ICON_TERM_DISCONNECT, wxBITMAP_TYPE_PNG);
	m_imgStartLog   .LoadFile(ICON_START_LOG,       wxBITMAP_TYPE_PNG);
	m_imgStopLog    .LoadFile(ICON_STOP_LOG,        wxBITMAP_TYPE_PNG);
	m_imgPinActive  .LoadFile(ICON_PIN_ACTIVE,      wxBITMAP_TYPE_PNG);
	m_imgPinInactive.LoadFile(ICON_PIN_INACTIVE,    wxBITMAP_TYPE_PNG);

	//
	// Create ribbon menu
	//
	m_pMenu = new wxRibbonBar(this, ID_MAIN_MENU, wxDefaultPosition,
		wxDefaultSize, wxRIBBON_BAR_DEFAULT_STYLE
		& ~(wxRIBBON_BAR_SHOW_TOGGLE_BUTTON));
	m_pMenu->GetArtProvider()->SetColourScheme
		(wxColour(200,200,200), wxColour(255,223,114), wxColour(0,0,0));
	{
		// first page
		wxRibbonPage* page = new wxRibbonPage(m_pMenu, wxID_ANY, RIBBON_SETUP);
		{
			wxRibbonPanel* toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_PRESET, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);

				m_pPresetBar = new wxRibbonButtonBar
					(toolbar_panel, -1, wxDefaultPosition, wxDefaultSize,
					wxRIBBON_BUTTONBAR_BUTTON_LARGE);
				{
					m_pPresetBar->AddButton(ID_ADD_PRESET, RIBBON_ADD_PRESET,
						wxImage(ICON_ADD_PRESET, wxBITMAP_TYPE_PNG));
					m_pPresetBar->AddButton(ID_RENAME_PRESET, RIBBON_RENAME_PRESET,
						wxImage(ICON_EDIT_PRESET, wxBITMAP_TYPE_PNG));
					m_pPresetBar->AddButton(ID_DEL_PRESET, RIBBON_DEL_PRESET,
						wxImage(ICON_DEL_PRESET, wxBITMAP_TYPE_PNG));
					m_pPresetBar->SetButtonMaxSizeClass
						(ID_ADD_PRESET, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
					m_pPresetBar->SetButtonMaxSizeClass
						(ID_RENAME_PRESET, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);
					m_pPresetBar->SetButtonMaxSizeClass
						(ID_DEL_PRESET, wxRIBBON_BUTTONBAR_BUTTON_MEDIUM);

					// Add left to right, then top to bottom
					wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2, 1, 5, 5);
					panelSizer->AddGrowableCol(0, 1);

					{
						wxStaticText* pText = new wxStaticText
							(toolbar_panel, -1, RIBBON_PRESET, wxDefaultPosition,
							wxSize(150, 16), wxALIGN_CENTER_HORIZONTAL);

						m_pPresetSelect = new wxComboBox
							(toolbar_panel, ID_PRESET_SELECT, wxEmptyString,
							wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);

						panelSizer->Add(pText, 0, wxTOP, 10);
						panelSizer->Add(m_pPresetSelect, 0, wxTOP | wxBOTTOM | wxEXPAND, 5);
					}
					boxSizer->Add(m_pPresetBar, 0, wxRIGHT | wxEXPAND, 5);
					boxSizer->Add(panelSizer, 0, wxLEFT | wxEXPAND, 5);
					toolbar_panel->SetSizer(boxSizer);
				}
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_SETTINGS, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				// Add left to right, then top to bottom
				wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2, 4, 5, 5);
				panelSizer->AddGrowableCol(1, 1);
				panelSizer->AddGrowableCol(3, 1);

				{
					m_pTextConnType = new wxStaticText
						(toolbar_panel, -1, RIBBON_CONN_TYPE, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

					wxString choises[] =
					{
						"Serial/UART", "TCP/IPv4"
					};

					m_pIFaceTypeSelect = new wxComboBox
						(toolbar_panel, ID_CONNECTION_TYPE, wxEmptyString,
						wxDefaultPosition, wxDefaultSize,
						sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);

					m_pIFaceTypeSelect->Enable(false);

					panelSizer->Add(m_pTextConnType, 0, wxTOP | wxLEFT, 10);
					panelSizer->Add(m_pIFaceTypeSelect, 0, wxTOP | wxBOTTOM | wxEXPAND, 5);
				}
				{
					m_pTextIFaceName = new wxStaticText
						(toolbar_panel, -1, RIBBON_DEV_NAME, wxDefaultPosition,
							wxDefaultSize, wxALIGN_CENTER_VERTICAL);

					m_pIFaceNameInput = new wxTextCtrl
						(toolbar_panel, ID_IFACE_NAME, wxEmptyString);

					panelSizer->Add(m_pTextIFaceName, 0, wxTOP, 10);
					panelSizer->Add(m_pIFaceNameInput, 0, wxTOP | wxBOTTOM | wxEXPAND, 5);
				}
				{
					m_pTextBaudRate = new wxStaticText
						(toolbar_panel, -1, RIBBON_BAUDRATE, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

					wxString choises[] =
					{
						"9600",
						"19200",
						"38400",
						"57600",
						"115200",
						"230400",
						"460800",
						"500000",
						"576000",
						"921600"
					};

					m_pParamSelect = new wxComboBox
						(toolbar_panel, ID_PARAM, wxEmptyString,
						wxDefaultPosition, wxDefaultSize,
						sizeof(choises)/sizeof(choises[0]), choises);

					panelSizer->Add(m_pTextBaudRate, 0, wxTOP | wxLEFT, 10);
					panelSizer->Add(m_pParamSelect, 0, wxTOP | wxBOTTOM | wxEXPAND, 5);
				}
				{
					m_pTextFrameFormat = new wxStaticText
						(toolbar_panel, -1, RIBBON_FFORMAT, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

					wxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
					{
						wxString choises[] =
						{
							"5", "6", "7", "8"
						};
						m_pDatabitsSelect = new wxComboBox
							(toolbar_panel, ID_DATABITS, wxEmptyString,
							wxDefaultPosition, wxDefaultSize,
							sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);

						m_pDatabitsSelect->SetToolTip(new wxToolTip(TOOLTIP_DATABITS));

						rowSizer->Add(m_pDatabitsSelect, 0, wxLEFT | wxEXPAND, 0);
					}
					{
						wxString choises[] =
						{
							"N", "E", "O", "M", "S"
						};
						m_pParitySelect = new wxComboBox
							(toolbar_panel, ID_PARITY, wxEmptyString,
							wxDefaultPosition, wxDefaultSize,
							sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);

						m_pParitySelect->SetToolTip(new wxToolTip(TOOLTIP_PARITY));

						rowSizer->Add(m_pParitySelect, 0, wxLEFT | wxEXPAND, 5);
					}
					{
						wxString choises[] =
						{
							"1", "2"
						};
						m_pStopbitsSelect = new wxComboBox
							(toolbar_panel, ID_STOPBITS, wxEmptyString,
							wxDefaultPosition, wxDefaultSize,
							sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);

						m_pStopbitsSelect->SetToolTip(new wxToolTip(TOOLTIP_STOPBITS));

						rowSizer->Add(m_pStopbitsSelect, 0, wxLEFT | wxEXPAND, 5);
					}
					{
						wxString choises[] =
						{
							"None", "RTS/CTS"
						};
						m_pFlowctrlSelect = new wxComboBox
							(toolbar_panel, ID_FLOWCTRL, wxEmptyString,
							wxDefaultPosition, wxDefaultSize,
							sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);

						m_pFlowctrlSelect->SetToolTip(new wxToolTip(TOOLTIP_FLOWCTRL));

						rowSizer->Add(m_pFlowctrlSelect, 0, wxLEFT | wxEXPAND, 5);
					}

					panelSizer->Add(m_pTextFrameFormat, 0, wxTOP, 10);
					panelSizer->Add(rowSizer, 0, wxTOP | wxBOTTOM | wxEXPAND, 5);
				}
				toolbar_panel->SetSizer(panelSizer);
			}
		}

		// second page
		page = new wxRibbonPage(m_pMenu, wxID_ANY, RIBBON_TERMINAL);
		{
			wxRibbonPanel* toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_CONNECTION, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				m_pTermConnectBar = new wxRibbonButtonBar
					(toolbar_panel, -1, wxDefaultPosition, wxDefaultSize);
				{
					m_pTermConnectBar->AddButton(ID_CONNECT_DISCONNECT,
						RIBBON_CONNECT, m_imgConnect);
					m_pTermConnectBar->SetButtonTextMinWidth
						(ID_CONNECT_DISCONNECT, RIBBON_DISCONNECT);
					m_pTermConnectBar->AddButton(ID_CLRTERM, RIBBON_CLR_TERM,
						wxImage(ICON_TERM_CLEAR, wxBITMAP_TYPE_PNG));
					m_pTermConnectBar->AddButton(ID_TX_FILE, RIBBON_TX_FILE,
						wxImage(ICON_TX_FILE, wxBITMAP_TYPE_PNG));
					m_pTermConnectBar->EnableButton(ID_TX_FILE, false);
				}
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_LOGGING, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				m_pTermLogBar = new wxRibbonButtonBar(toolbar_panel);
				{
					m_pTermLogBar->AddButton(ID_SAVE_OUTPUT, RIBBON_LOG_SAVE,
						wxImage(ICON_TERM_SAVE, wxBITMAP_TYPE_PNG),
						TOOLTIP_SAVE_OUTPUT);
					m_pTermLogBar->AddButton(ID_START_STOP_LOG,
						RIBBON_LOG_START, m_imgStartLog);
					m_pTermLogBar->SetButtonTextMinWidth
						(ID_START_STOP_LOG, RIBBON_LOG_START);
				}
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_SETTINGS, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				// Add left to right, then top to bottom
				wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 4, 5, 5);
				gridSizer->AddGrowableCol(1);
				gridSizer->AddGrowableCol(3);
				{
					wxString choises[] =
					{
						"None", "LF (\\n)", "CR (\\r)", "CR+LF", "Null (0x00)"
					};

					wxStaticText* pText = new wxStaticText
						(toolbar_panel, -1, RIBBON_SEND_ON_ENTER, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_VERTICAL);

					m_pTermOnEnterSel = new wxComboBox
						(toolbar_panel, ID_ON_ENTER_SEL, wxEmptyString,
						wxDefaultPosition, wxDefaultSize,
						sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);
					m_pTermOnEnterSel->SetSelection(0);

					gridSizer->Add(pText, 0, wxTOP | wxLEFT, 10);
					gridSizer->Add(m_pTermOnEnterSel, 0, wxTOP | wxRIGHT | wxEXPAND, 5);
				}
				{
					wxStaticText* pText = new wxStaticText
						(toolbar_panel, -1, RIBBON_ENABLE_HISTORY, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_VERTICAL);

					m_pTermEnHistory = new wxCheckBox
						(toolbar_panel, ID_ENABLE_HISTORY, wxEmptyString);
					m_pTermEnHistory->SetToolTip
						(new wxToolTip(TOOLTIP_ENABLE_HISTORY));
					m_pTermEnHistory->SetValue(true);

					gridSizer->Add(pText, 0, wxTOP | wxLEFT, 10);
					gridSizer->Add(m_pTermEnHistory, 0, wxTOP | wxRIGHT | wxEXPAND, 5);
				}
				{
					wxString choises[] =
					{
						"ASCII (F6)", "Binary (F7)", "Octal (F8)",
						"Decimal (F9)", "Hex (F10)"
					};

					wxStaticText* pText = new wxStaticText
						(toolbar_panel, -1, RIBBON_INPUT_FORMAT, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_VERTICAL);

					m_pTermInputSel = new wxComboBox
						(toolbar_panel, ID_INPUT_FORMAT, wxEmptyString,
						wxDefaultPosition, wxDefaultSize,
						sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);
					m_pTermInputSel->SetSelection(0);

					gridSizer->Add(pText, 0, wxTOP | wxLEFT, 10);
					gridSizer->Add(m_pTermInputSel, 0, wxTOP | wxRIGHT | wxEXPAND, 5);
				}
				{
					wxStaticText* pText = new wxStaticText
						(toolbar_panel, -1, RIBBON_HISTORY_LEN, wxDefaultPosition,
						wxDefaultSize, wxALIGN_CENTER_VERTICAL);

					m_pHistoryLength = new wxTextCtrl
						(toolbar_panel, ID_HISTORY_LENGTH, "10",
						wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

					gridSizer->Add(pText, 0, wxTOP | wxLEFT, 10);
					gridSizer->Add(m_pHistoryLength, 0, wxTOP | wxRIGHT | wxEXPAND, 5);
				}
				toolbar_panel->SetSizer(gridSizer);
			}
		}

		// third page
		page = new wxRibbonPage(m_pMenu, wxID_ANY, RIBBON_ADVANCED);
		{
			wxRibbonPanel* toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_INPUT_PINS, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				m_pLineCtrlInBar = new wxRibbonButtonBar(toolbar_panel);
				{
					m_pLineCtrlInBar->AddButton(ID_LINECTRL_DSR, wxT("DSR"),
						m_imgPinInactive);
					m_pLineCtrlInBar->AddButton(ID_LINECTRL_CTS, wxT("CTS"),
						m_imgPinInactive);
					m_pLineCtrlInBar->AddButton(ID_LINECTRL_DCD, wxT("DCD"),
						m_imgPinInactive);
					m_pLineCtrlInBar->AddButton(ID_LINECTRL_RNG, wxT("RNG"),
						m_imgPinInactive);

				}
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_OUTPUT_PINS, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				m_pLineCtrlOutBar = new wxRibbonButtonBar(toolbar_panel);
				{
					m_pLineCtrlOutBar->AddButton(ID_LINECTRL_DTR, wxT("DTR"),
						m_imgPinInactive);
					m_pLineCtrlOutBar->AddButton(ID_LINECTRL_RTS, wxT("RTS"),
						m_imgPinInactive);
				}
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_STATISTICS, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				m_pStatisticsBar = new wxRibbonButtonBar(toolbar_panel);
				{
					m_pStatisticsBar->AddButton(ID_CLEAR_STAT_RX, RIBBON_STAT_CLEAR_RX,
						wxImage(ICON_CLEAR_STAT_RX, wxBITMAP_TYPE_PNG));
					m_pStatisticsBar->AddButton(ID_CLEAR_STAT_TX, RIBBON_STAT_CLEAR_TX,
						wxImage(ICON_CLEAR_STAT_TX, wxBITMAP_TYPE_PNG));

					m_pStatisticsBar->SetButtonMinSizeClass
						(ID_CLEAR_STAT_RX, wxRIBBON_BUTTONBAR_BUTTON_LARGE);
					m_pStatisticsBar->SetButtonMinSizeClass
						(ID_CLEAR_STAT_TX, wxRIBBON_BUTTONBAR_BUTTON_LARGE);

					wxBoxSizer* panel_sizer = new wxBoxSizer(wxHORIZONTAL);
					wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 2, 0, 0);

					wxStaticText* pTextRX =  new wxStaticText
						(toolbar_panel, -1, RIBBON_STAT_COUNT_RX);
					m_pStatRxCounter = new wxTextCtrl
						(toolbar_panel, ID_STAT_RX, wxT("0"));
					m_pStatRxCounter->SetEditable(false);

					wxStaticText* pTextTX =  new wxStaticText
						(toolbar_panel, -1, RIBBON_STAT_COUNT_TX);
					m_pStatTxCounter = new wxTextCtrl
						(toolbar_panel, ID_STAT_TX, wxT("0"));
					m_pStatTxCounter->SetEditable(false);

					// Add left to right, then top to bottom
					gridSizer->Add(pTextRX, 0, wxTOP | wxRIGHT | wxEXPAND, 10);
					gridSizer->Add(m_pStatRxCounter, 0, wxTOP | wxRIGHT | wxEXPAND, 5);
					gridSizer->Add(pTextTX, 0, wxTOP | wxRIGHT | wxEXPAND, 10);
					gridSizer->Add(m_pStatTxCounter, 0, wxTOP | wxRIGHT | wxEXPAND, 5);

					panel_sizer->Add(m_pStatisticsBar, 0, wxRIGHT | wxEXPAND, 5);
					panel_sizer->Add(gridSizer, 0, wxLEFT | wxEXPAND, 5);

					toolbar_panel->SetSizer(panel_sizer);
				}
			}
		}
		// fourth page
		page = new wxRibbonPage(m_pMenu, wxID_ANY, RIBBON_VIEW);
		{
			wxRibbonPanel* toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_NUMSYS, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 3, 5, 5);

				m_pTermShowAsc = new wxCheckBox
					(toolbar_panel, ID_SHOW_ASC, RIBBON_SHOW_ASC);
				m_pTermShowAsc->SetValue(true);
				m_pTermShowHex = new wxCheckBox
					(toolbar_panel, ID_SHOW_HEX, RIBBON_SHOW_HEX);
				m_pTermShowDec = new wxCheckBox
					(toolbar_panel, ID_SHOW_DEC, RIBBON_SHOW_DEC);
				m_pTermShowOct = new wxCheckBox
					(toolbar_panel, ID_SHOW_OCT, RIBBON_SHOW_OCT);
				m_pTermShowBin = new wxCheckBox
					(toolbar_panel, ID_SHOW_BIN, RIBBON_SHOW_BIN);

				// Add left to right, then top to bottom
				gridSizer->Add(m_pTermShowAsc, 0, wxTOP | wxLEFT | wxEXPAND, 5);
				gridSizer->Add(m_pTermShowDec, 0, wxTOP | wxLEFT | wxEXPAND, 5);
				gridSizer->Add(m_pTermShowBin, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 5);

				gridSizer->Add(m_pTermShowHex, 0, wxTOP | wxLEFT | wxEXPAND, 5);
				gridSizer->Add(m_pTermShowOct, 0, wxTOP | wxLEFT | wxEXPAND, 5);

				toolbar_panel->SetSizer(gridSizer);
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_TERM_WND, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 2, 5, 5);
				m_pTermEnAutoscroll = new wxCheckBox
					(toolbar_panel, ID_ENABLE_AUTOSCROLL, RIBBON_AUTOSCROLL);
				m_pTermEnAutoscroll->SetValue(true);

				m_pTermShowRX = new wxCheckBox
					(toolbar_panel, ID_SHOW_RX, RIBBON_SHOW_RX);
				m_pTermShowRX->SetValue(true);
				m_pTermShowTX = new wxCheckBox
					(toolbar_panel, ID_SHOW_TX, RIBBON_SHOW_TX);
				m_pTermShowTX->SetValue(true);
				m_pTermShowERR = new wxCheckBox
					(toolbar_panel, ID_SHOW_ERR, RIBBON_SHOW_ERR);
				m_pTermShowERR->SetValue(true);

				// Add left to right, then top to bottom
				gridSizer->Add(m_pTermShowRX, 0, wxTOP | wxLEFT | wxEXPAND, 5);
				gridSizer->Add(m_pTermShowERR, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 5);

				gridSizer->Add(m_pTermShowTX, 0, wxTOP | wxLEFT | wxEXPAND, 5);
				gridSizer->Add(m_pTermEnAutoscroll, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 5);

				toolbar_panel->SetSizer(gridSizer);
			}
			toolbar_panel = new wxRibbonPanel
				(page, wxID_ANY, RIBBON_NEWLINES, wxNullBitmap, wxDefaultPosition,
				wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
			{
				wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 3, 5, 5);
				wxStaticText* pText1 = new wxStaticText
					(toolbar_panel, -1, RIBBON_NEWLINE_AT, wxDefaultPosition,
					wxDefaultSize, wxALIGN_CENTER_VERTICAL);

				wxString choises[] =
				{
					"None", "LF (\\n)", "CR (\\r)", "Null (0x00)"
				};

				m_pTermNewlineSel = new wxComboBox
					(toolbar_panel, ID_NEWLINE_SEL, wxEmptyString,
					wxDefaultPosition, wxDefaultSize,
					sizeof(choises)/sizeof(choises[0]), choises, wxCB_READONLY);
				m_pTermNewlineSel->SetSelection(1);

				m_pTermEnNewlineEveryN = new wxCheckBox
					(toolbar_panel, ID_NEWLINE_EVERY_N_ENABLE, wxEmptyString);
				m_pTermEnNewlineEveryN->SetValue(false);

				wxStaticText* pText2 = new wxStaticText
					(toolbar_panel, -1, RIBBON_NEWLINE_EVERY_N, wxDefaultPosition,
					wxDefaultSize, wxALIGN_CENTER_VERTICAL);

				m_pTermNewlineEveryNSelect = new wxTextCtrl
					(toolbar_panel, ID_NEWLINE_EVERY_N_SEL, wxEmptyString,
					wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
				m_pTermNewlineEveryNSelect->ChangeValue("8");

				// Add left to right, then top to bottom
				gridSizer->AddStretchSpacer();
				gridSizer->Add(pText1, 0, wxTOP | wxEXPAND, 10);
				gridSizer->Add(m_pTermNewlineSel, 0, wxTOP | wxLEFT | wxEXPAND, 5);

				gridSizer->Add(m_pTermEnNewlineEveryN, 0, wxTOP | wxLEFT | wxEXPAND, 5);
				gridSizer->Add(pText2, 0, wxTOP | wxEXPAND, 10);
				gridSizer->Add(m_pTermNewlineEveryNSelect, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 5);

				toolbar_panel->SetSizer(gridSizer);
			}
		}
	}
	m_pMenu->Realize();

	//
	// Set window layout
	//

	// Create terminal box
	m_pTermOutputBox = new multiBaseOutput(this, ID_TERM_OUTPUT, &m_colors);
	m_pTermInputBox = new multiBaseInput(this, ID_TERM_INPUT, &m_colors);
	m_pRedrawLimitTimer = new wxTimer(this);

	try
	{
		m_pBackend = new backend(this);
	}
	catch(std::runtime_error& x)
	{
		wxMessageBox(x.what(), MSG_ERROR, wxICON_ERROR);
		throw(x);
	}

	termInstance::m_pMainframe = this;
	termInstance::m_pBackend = m_pBackend;

	// Create ribbon <-> windows sizer
	wxSizer* s = new wxBoxSizer(wxVERTICAL);
	s->Add(m_pMenu, 0, wxEXPAND);
	s->Add(m_pTermOutputBox, 1, wxEXPAND | wxBOTTOM, 5);
	s->Add(m_pTermInputBox, 0, wxEXPAND, 0);
	SetSizer(s);

	SetMinSize(wxSize(480, 360));

	// Setup keyboard shortcuts
	wxAcceleratorEntry entries[] =
	{
		{wxACCEL_CTRL, 'Z', ID_ACCEL_UNDO, 0},
		{wxACCEL_NORMAL, WXK_F6, ID_ACCEL_ASCII, 0},
		{wxACCEL_NORMAL, WXK_F7, ID_ACCEL_BIN, 0},
		{wxACCEL_NORMAL, WXK_F8, ID_ACCEL_OCT, 0},
		{wxACCEL_NORMAL, WXK_F9, ID_ACCEL_DEC, 0},
		{wxACCEL_NORMAL, WXK_F10, ID_ACCEL_HEX, 0}
	};
	wxAcceleratorTable accel(sizeof(entries)/sizeof(entries[0]), entries);
	SetAcceleratorTable(accel);

	/**********************************************************************\
	 * Data Initialization
	\**********************************************************************/

	m_configFilename = wxStandardPaths::Get().GetUserConfigDir();
	m_configFilename += L"/.local/";
	m_configFilename.append(CONFIG_FILENAME);

	LoadPresets();

	if(m_instances.size() == 0)
	{
		CreateInstance("default");
		SelectInstance("default");
	}
}

mainframe::~mainframe()
{
	if(m_pBackend)
	{
		delete m_pBackend;
		m_pBackend = 0;
	}

	if(m_pRedrawLimitTimer)
	{
		delete m_pRedrawLimitTimer;
		m_pRedrawLimitTimer = 0;
	}

	// Detach sizer manually to prevent some assertion
	// issues on close.
	GetSizer()->Detach(this);
}

void mainframe::UpdateStatusText(const termInstance* pSrc)
{
	if(pSrc != m_pCurrentInstance)
		return;

	const wxString& str = m_pCurrentInstance->GetStatusText();
	if(!str.IsEmpty())
		SetStatusText(str);
}

void mainframe::UpdateTerminalOutput(const termInstance* pSrc, bool scrollDown)
{
	if(pSrc != m_pCurrentInstance)
		return;

	// Terminal redraw rate has to be limited to keep
	// GUI responsive at very high input data rates.
	if(m_pRedrawLimitTimer->IsRunning())
		m_redrawSuppressed = true;
	else
	{
		if(scrollDown)
		{
			int range = m_pTermOutputBox->GetScrollRange(wxVERTICAL);
			m_pTermOutputBox->Scroll(0, range);
		}
		m_pRedrawLimitTimer->StartOnce(MAX_UPDATE_RATE_MS);
		m_pTermOutputBox->Redraw();
	}
}

void mainframe::UpdateConnectState(const termInstance* pSrc)
{
	if(pSrc != m_pCurrentInstance)
		return;

	if(m_pCurrentInstance->IsConnected())
	{
		m_pTermConnectBar->SetButtonIcon(ID_CONNECT_DISCONNECT, m_imgDisconnect);
		m_pTermConnectBar->SetButtonText(ID_CONNECT_DISCONNECT, RIBBON_DISCONNECT);
		m_pTermConnectBar->EnableButton(ID_TX_FILE, true);
	}
	else
	{
		m_pTermConnectBar->SetButtonIcon(ID_CONNECT_DISCONNECT, m_imgConnect);
		m_pTermConnectBar->SetButtonText(ID_CONNECT_DISCONNECT, RIBBON_CONNECT);
		m_pTermConnectBar->EnableButton(ID_TX_FILE, false);
	}
}

void mainframe::UpdateConnectionSettings(const termInstance* pSrc)
{
	if(pSrc != m_pCurrentInstance)
		return;

	// -1: skip unknown from enum
	m_pIFaceTypeSelect->SetSelection(m_pCurrentInstance->m_settings.iface_type - 1);
	m_pIFaceNameInput->ChangeValue(m_pCurrentInstance->m_settings.iface_name);
	m_pParamSelect->ChangeValue(std::to_wstring(m_pCurrentInstance->m_settings.param));

	m_pDatabitsSelect->SetSelection(m_pCurrentInstance->m_settings.databits);
	m_pParitySelect->SetSelection(m_pCurrentInstance->m_settings.parity);
	m_pStopbitsSelect->SetSelection(m_pCurrentInstance->m_settings.stopbits);
	m_pFlowctrlSelect->SetSelection(m_pCurrentInstance->m_settings.flowctrl);
}

void mainframe::UpdateLogState(const termInstance* pSrc)
{
	if(pSrc != m_pCurrentInstance)
		return;

	if(m_pCurrentInstance->IsLogging())
	{
		m_pTermLogBar->SetButtonIcon(ID_START_STOP_LOG, m_imgStopLog);
		m_pTermLogBar->SetButtonText(ID_START_STOP_LOG, RIBBON_LOG_STOP);
	}
	else
	{
		m_pTermLogBar->SetButtonIcon(ID_START_STOP_LOG, m_imgStartLog);
		m_pTermLogBar->SetButtonText(ID_START_STOP_LOG, RIBBON_LOG_START);
	}
}

void mainframe::UpdateTerminalSettings(const termInstance* pSrc)
{
	if(pSrc != m_pCurrentInstance)
		return;

	m_pTermOnEnterSel->SetSelection(m_pCurrentInstance->m_onEnterSelection);

	m_pTermInputSel->SetSelection(m_pCurrentInstance->m_inputMode);
	m_pTermInputBox->SetInputMode(m_pCurrentInstance->m_inputMode);

	bool checked = m_pCurrentInstance->IsSelected(termInstance::TERM_EN_HISTORY);
	m_pTermEnHistory->SetValue(checked);

	m_pHistoryLength->ChangeValue(
		std::to_wstring(m_pCurrentInstance->HistorySize()));
}

void mainframe::UpdatePinValue(const termInstance* pSrc, int pins)
{
	if(pSrc != m_pCurrentInstance)
		return;

	if(pins & TIOCM_RTS)
	{
		if(m_pCurrentInstance->GetPinValue(TIOCM_RTS))
			m_pLineCtrlOutBar->SetButtonIcon(ID_LINECTRL_RTS, m_imgPinActive);
		else
			m_pLineCtrlOutBar->SetButtonIcon(ID_LINECTRL_RTS, m_imgPinInactive);
	}
	if(pins & TIOCM_DTR)
	{
		if(m_pCurrentInstance->GetPinValue(TIOCM_DTR))
			m_pLineCtrlOutBar->SetButtonIcon(ID_LINECTRL_DTR, m_imgPinActive);
		else
			m_pLineCtrlOutBar->SetButtonIcon(ID_LINECTRL_DTR, m_imgPinInactive);
	}

	if(pins & TIOCM_CTS)
	{
		if(m_pCurrentInstance->GetPinValue(TIOCM_CTS))
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_CTS, m_imgPinActive);
		else
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_CTS, m_imgPinInactive);
	}
	if(pins & TIOCM_DSR)
	{
		if(m_pCurrentInstance->GetPinValue(TIOCM_DSR))
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_DSR, m_imgPinActive);
		else
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_DSR, m_imgPinInactive);
	}
	if(pins & TIOCM_CAR)
	{
		if(m_pCurrentInstance->GetPinValue(TIOCM_CAR))
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_DCD, m_imgPinActive);
		else
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_DCD, m_imgPinInactive);
	}
	if(pins & TIOCM_RNG)
	{
		if(m_pCurrentInstance->GetPinValue(TIOCM_RNG))
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_RNG, m_imgPinActive);
		else
			m_pLineCtrlInBar->SetButtonIcon(ID_LINECTRL_RNG, m_imgPinInactive);
	}
}

void mainframe::UpdateStatistics(const termInstance* pSrc)
{
	m_pStatRxCounter->ChangeValue(
		std::to_wstring(m_pCurrentInstance->GetRxCount()));
	m_pStatTxCounter->ChangeValue(
		std::to_wstring(m_pCurrentInstance->GetTxCount()));
}

void mainframe::UpdateViewSettings(const termInstance* pSrc)
{
	if(pSrc != m_pCurrentInstance)
		return;

	bool checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_ASC);
	m_pTermShowAsc->SetValue(checked);
	m_pTermOutputBox->EnableBase(multiBaseCommon::BASE_ASC, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_HEX);
	m_pTermShowHex->SetValue(checked);
	m_pTermOutputBox->EnableBase(multiBaseCommon::BASE_HEX, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_DEC);
	m_pTermShowDec->SetValue(checked);
	m_pTermOutputBox->EnableBase(multiBaseCommon::BASE_DEC, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_OCT);
	m_pTermShowOct->SetValue(checked);
	m_pTermOutputBox->EnableBase(multiBaseCommon::BASE_OCT, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_BIN);
	m_pTermShowBin->SetValue(checked);
	m_pTermOutputBox->EnableBase(multiBaseCommon::BASE_BIN, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_RX);
	m_pTermShowRX->SetValue(checked);
	m_pTermOutputBox->EnableCharClass(multiBaseCommon::COLOR_RX, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_TX);
	m_pTermShowTX->SetValue(checked);
	m_pTermOutputBox->EnableCharClass(multiBaseCommon::COLOR_TX, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_SHOW_ERR);
	m_pTermShowERR->SetValue(checked);
	m_pTermOutputBox->EnableCharClass(multiBaseCommon::COLOR_ERR, checked);

	checked = m_pCurrentInstance->IsSelected(termInstance::VIEW_EN_AUTOSCROLL);
	m_pTermEnAutoscroll->SetValue(checked);
	m_pTermOutputBox->EnableAutoscroll(checked);

	if(m_pCurrentInstance->IsSelected(termInstance::VIEW_EN_NEWLINE))
	{
		if(m_pCurrentInstance->m_newlineChar == '\n')
			m_pTermNewlineSel->SetSelection(1);
		else if(m_pCurrentInstance->m_newlineChar == '\r')
			m_pTermNewlineSel->SetSelection(2);
		else if(m_pCurrentInstance->m_newlineChar == 0)
			m_pTermNewlineSel->SetSelection(3);
		m_pTermOutputBox->SetNewlineChar(m_pCurrentInstance->m_newlineChar, true);
	}
	else
	{
		m_pTermNewlineSel->SetSelection(0);
		m_pTermOutputBox->SetNewlineChar(m_pCurrentInstance->m_newlineChar, false);
	}

	checked = m_pCurrentInstance->IsSelected
		(termInstance::VIEW_EN_NEWLINE_EVERY_N);
	m_pTermEnNewlineEveryN->SetValue(checked);
	m_pTermNewlineEveryNSelect->ChangeValue
		(std::to_wstring(m_pCurrentInstance->m_maxLineLen));
	m_pTermOutputBox->SetMaxLineLength
		(checked ? m_pCurrentInstance->m_maxLineLen : INT_MAX);

	m_pTermOutputBox->Redraw();
}

/**********************************************************************\
 * GUI events - home
\**********************************************************************/

void mainframe::OnClose(wxCloseEvent& event)
{
	if(m_presetChanged || termInstance::m_historyChanged)
		SavePresets();

	for(auto& instance : m_instances)
	{
		if(instance.IsConnected())
			instance.Disconnect();
	}

	Destroy();
}

void mainframe::OnAbout(wxRibbonBarEvent& WXUNUSED(evt))
{
	aboutDialog aboutdlg(this, wxID_ANY, MSG_ABOUT);
	aboutdlg.ShowModal();
}

void mainframe::OnAddPreset(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	wxTextEntryDialog dlg(this, ADD_PRESET_PROMPT, RIBBON_ADD_PRESET);
	dlg.ShowModal();

	wxString name = dlg.GetValue();

	if(name.Len() == 0)
	{
		wxMessageBox(ERR_PRESET_NAME_EMPTY, MSG_ERROR, wxICON_ERROR);
		return;
	}

	for(const auto& instance : m_instances)
	{
		if(instance.m_name == name)
		{
			wxMessageBox(ERR_PRESET_NAME_EXISTS, MSG_ERROR, wxICON_ERROR);
			return;
		}
	}

	CreateInstance(name);
	SelectInstance(name);
}

void mainframe::OnRenamePreset(wxRibbonButtonBarEvent& evt)
{
	termInstance* pInstance = 0;
	wxString oldName = m_pPresetSelect->GetValue();
	wxTextEntryDialog dlg(this, ADD_PRESET_PROMPT,
		RIBBON_RENAME_PRESET, oldName);
	dlg.ShowModal();

	wxString newName = dlg.GetValue();

	if(newName.Len() == 0)
	{
		wxMessageBox(ERR_PRESET_NAME_EMPTY, MSG_ERROR, wxICON_ERROR);
		return;
	}

	for(auto& instance : m_instances)
	{
		if(instance.m_name == newName)
		{
			wxMessageBox(ERR_PRESET_NAME_EXISTS, MSG_ERROR, wxICON_ERROR);
			return;
		}

		if(instance.m_name == oldName)
			pInstance = &instance;
	}

	pInstance->m_name = newName;
	UpdatePresetList();
}

void mainframe::OnDeletePreset(wxRibbonButtonBarEvent& evt)
{
	wxString name = m_pPresetSelect->GetValue();

	if(m_instances.size() == 1)
	{
		wxMessageBox(ERR_PRESET_DELETE_LAST, MSG_ERROR, wxICON_ERROR);
		return;
	}

	int result = wxMessageBox(wxString::Format(MSG_CONFIRM_DEL_PRESET, name),
		RIBBON_DEL_PRESET, wxYES_NO | wxICON_QUESTION);

	if(result == wxYES)
	{
		m_pCurrentInstance->m_deleted = ++m_undoCounter;
		UpdatePresetList();
		SelectInstance(m_pPresetSelect->GetValue());
	}
}

void mainframe::OnPresetSelect(wxCommandEvent& evt)
{
	SelectInstance(m_pPresetSelect->GetValue());
	m_presetChanged = true;
}

void mainframe::OnConnectionSettingsChanged(wxCommandEvent& evt)
{
	// Ignore during init (create default instance)
	if(!m_pCurrentInstance)
		return;

	wxString str = m_pParamSelect->GetValue();
	long int num;
	if(!IsPositiveNumber(str, &num))
		return;

	// +1: skip unknown IF type in enum
	m_pCurrentInstance->m_settings.iface_type =
		m_pIFaceTypeSelect->GetSelection() + 1;

	m_pCurrentInstance->m_settings.param = num;
	m_pCurrentInstance->m_settings.iface_name = m_pIFaceNameInput->GetValue();
	m_pCurrentInstance->m_settings.databits = m_pDatabitsSelect->GetSelection();
	m_pCurrentInstance->m_settings.parity	= m_pParitySelect->GetSelection();
	m_pCurrentInstance->m_settings.stopbits	= m_pStopbitsSelect->GetSelection();
	m_pCurrentInstance->m_settings.flowctrl	= m_pFlowctrlSelect->GetSelection();

	m_presetChanged = true;
}

/**********************************************************************\
 * GUI events - terminal
\**********************************************************************/

void mainframe::OnConnectDisconnect(wxRibbonButtonBarEvent& evt)
{
	if(!m_pCurrentInstance->IsConnected())
	{
		SetStatusText(wxString::Format
			(MSG_STATUS_CONNECTING, m_pCurrentInstance->GetSettings().iface_name));
		m_pCurrentInstance->Connect();
	}
	else
	{
		SetStatusText(MSG_STATUS_DISCONNECTING);
		m_pCurrentInstance->Disconnect();
	}
}

void mainframe::OnClrTerm(wxRibbonButtonBarEvent& evt)
{
	m_pCurrentInstance->ClearTerminal();
	m_pTermOutputBox->Redraw();
}

void mainframe::OnTxFile(wxRibbonButtonBarEvent& evt)
{
	wxFileDialog filedlg(this, wxFileSelectorPromptStr,
		wxEmptyString, wxEmptyString,
		SELECTOR_ALL_FILES, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if(filedlg.ShowModal() == wxID_CANCEL)
		return;

	m_pCurrentInstance->TransmitFile(filedlg.GetPath());
}

void mainframe::OnSaveTerm(wxRibbonButtonBarEvent& evt)
{
	wxFileDialog filedlg(this, wxFileSelectorPromptStr,
		m_pCurrentInstance->m_logDir,
		wxEmptyString, SELECTOR_RAW_OUTPUT,
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if(m_pCurrentInstance->AppendToFile())
		filedlg.SetExtraControlCreator(&termFileDlgExtra::CreateFuncAppend);
	else
		filedlg.SetExtraControlCreator(&termFileDlgExtra::CreateFuncNoAppend);

	if(filedlg.ShowModal() == wxID_CANCEL)
		return;

	wxString path = filedlg.GetPath();

	// Get parent dir
	size_t pos = path.find_last_of('/');
	m_pCurrentInstance->m_logDir = wxString(path, pos);

	termFileDlgExtra* pExtra =
		static_cast<termFileDlgExtra*>(filedlg.GetExtraControl());

	m_pCurrentInstance->SaveOutput(path, pExtra->AppendToFile());
}

void mainframe::OnStartStopLog(wxRibbonButtonBarEvent& evt)
{
	if(m_pCurrentInstance->IsLogging())
	{
		m_pCurrentInstance->StopLogging();
		m_pStatusBar->SetStatusText(MSG_STATUS_STOP_LOG);
	}
	else
	{
		wxFileDialog filedlg(this, wxFileSelectorPromptStr,
			m_pCurrentInstance->m_logDir,
			wxEmptyString, SELECTOR_RAW_OUTPUT,
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

		if(m_pCurrentInstance->AppendToFile())
			filedlg.SetExtraControlCreator(&termFileDlgExtra::CreateFuncAppend);
		else
			filedlg.SetExtraControlCreator(&termFileDlgExtra::CreateFuncNoAppend);

		if(filedlg.ShowModal() == wxID_CANCEL)
			return;

		wxString path = filedlg.GetPath();

		// Get parent dir
		size_t pos = path.find_last_of('/');
		m_pCurrentInstance->m_logDir = wxString(path, pos);

		termFileDlgExtra* pExtra =
				static_cast<termFileDlgExtra*>(filedlg.GetExtraControl());

		m_pCurrentInstance->StartLogging(path, pExtra->AppendToFile());
	}
}

void mainframe::OnOnEnterSelect(wxCommandEvent& evt)
{
	m_pCurrentInstance->SetOnEnterStr(m_pTermOnEnterSel->GetSelection());
	m_presetChanged = true;
}

void mainframe::OnInputFormatSelect(wxCommandEvent& evt)
{
	m_pCurrentInstance->SetInputMode(m_pTermInputSel->GetSelection());
	m_pTermInputBox->SetInputMode(m_pTermInputSel->GetSelection());
	m_presetChanged = true;
}

void mainframe::OnEnableHistory(wxCommandEvent& evt)
{
	m_pCurrentInstance->EnableHistory(m_pTermEnHistory->IsChecked());
	m_presetChanged = true;
}

void mainframe::OnHistorySize(wxCommandEvent& evt)
{
	long int num;
	wxString str = m_pHistoryLength->GetValue();
	if(!IsPositiveNumber(str, &num))
		return;

	m_pCurrentInstance->SetHistorySize(num);
	m_presetChanged = true;
}

/**********************************************************************\
 * GUI events - advanced
\**********************************************************************/

void mainframe::OnReadInputPins(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	m_pCurrentInstance->ReadInputPins();
}

void mainframe::OnToggleDTR(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	m_pCurrentInstance->ToggleOutputPin(TIOCM_DTR);
}

void mainframe::OnToggleRTS(wxRibbonButtonBarEvent& WXUNUSED(evt))
{
	m_pCurrentInstance->ToggleOutputPin(TIOCM_RTS);
}

void mainframe::OnClearStatRx(wxRibbonButtonBarEvent& evt)
{
	m_pCurrentInstance->ClearRxCount();
}

void mainframe::OnClearStatTx(wxRibbonButtonBarEvent& evt)
{
	m_pCurrentInstance->ClearTxCount();
}

/**********************************************************************\
 * GUI events - view
\**********************************************************************/

void mainframe::OnShowBase(wxCommandEvent& evt)
{
	bool ascii = m_pTermShowAsc->IsChecked();
	bool hex   = m_pTermShowHex->IsChecked();
	bool dec   = m_pTermShowDec->IsChecked();
	bool oct   = m_pTermShowOct->IsChecked();
	bool bin   = m_pTermShowBin->IsChecked();

	// Get toggled checkbox
	wxCheckBox* pCtrl;
	int base;
	switch(evt.GetId())
	{
		case ID_SHOW_ASC:
			pCtrl = m_pTermShowAsc;
			base = multiBaseCommon::BASE_ASC;
			break;

		case ID_SHOW_HEX:
			pCtrl = m_pTermShowHex;
			base = multiBaseCommon::BASE_HEX;
			break;

		case ID_SHOW_DEC:
			pCtrl = m_pTermShowDec;
			base = multiBaseCommon::BASE_DEC;
			break;

		case ID_SHOW_OCT:
			pCtrl = m_pTermShowOct;
			base = multiBaseCommon::BASE_OCT;
			break;

		case ID_SHOW_BIN:
			pCtrl = m_pTermShowBin;
			base = multiBaseCommon::BASE_BIN;
			break;
	}

	// Don't disable last base
	if(!ascii && !hex && !dec && !oct && !bin)
	{
		pCtrl->SetValue(true);
		return;
	}

	bool checked = pCtrl->IsChecked();
		m_pCurrentInstance->EnableOutputBase(base, checked);
	m_pTermOutputBox->EnableBase(base, checked);

	m_pTermOutputBox->Redraw();
	m_presetChanged = true;
}

void mainframe::OnShowCharClass(wxCommandEvent& evt)
{
	bool rx  = m_pTermShowRX->IsChecked();
	bool tx  = m_pTermShowTX->IsChecked();
	bool err = m_pTermShowERR->IsChecked();

	// Get toggled checkbox
	wxCheckBox* pCtrl;
	int charClass;
	switch(evt.GetId())
	{
		case ID_SHOW_RX:
			pCtrl = m_pTermShowRX;
			charClass = multiBaseCommon::COLOR_RX;
			break;

		case ID_SHOW_TX:
			pCtrl = m_pTermShowTX;
			charClass = multiBaseCommon::COLOR_TX;
			break;

		case ID_SHOW_ERR:
			pCtrl = m_pTermShowERR;
			charClass = multiBaseCommon::COLOR_ERR;
			break;
	}

	// Don't disable last base
	if(!rx && !tx && !err)
	{
		pCtrl->SetValue(true);
		return;
	}

	bool checked = pCtrl->IsChecked();
	m_pCurrentInstance->EnableCharClass(charClass, checked);
	m_pTermOutputBox->EnableCharClass(charClass, checked);

	m_pTermOutputBox->Redraw();
	m_presetChanged = true;
}

void mainframe::OnEnableAutoscroll(wxCommandEvent& evt)
{
	m_pCurrentInstance->EnableAutoscroll(m_pTermEnHistory->IsChecked());
	m_presetChanged = true;
}

void mainframe::OnNewlineSelect(wxCommandEvent& evt)
{
	int selection = m_pTermNewlineSel->GetSelection();
	uint8_t newlineChar = 0;
	bool enabled = false;

	if(selection == 0)
	{
		newlineChar = '\n';
		enabled = false;
	}
	else if(selection == 1)
	{
		newlineChar = '\n';
		enabled = true;
	}
	else if(selection == 2)
	{
		newlineChar = '\r';
		enabled = true;
	}
	else if(selection == 3)
	{
		newlineChar = 0;
		enabled = true;
	}

	m_pCurrentInstance->SetNewlineChar(newlineChar, enabled);
	m_pTermOutputBox->SetNewlineChar(newlineChar, enabled);
	m_pTermOutputBox->Redraw();
	m_presetChanged = true;
}

void mainframe::OnMaxLineLen(wxCommandEvent& evt)
{
	long int len;
	wxString str = m_pTermNewlineEveryNSelect->GetValue();
	bool enabled = m_pTermEnNewlineEveryN->GetValue();

	if(!IsPositiveNumber(str, &len))
		return;

	m_pCurrentInstance->SetMaxLineLength(len, enabled);
	if(enabled)
		m_pTermOutputBox->SetMaxLineLength(len);
	else
		m_pTermOutputBox->SetMaxLineLength(INT_MAX);
	m_pTermOutputBox->Redraw();
	m_presetChanged = true;
}

/**********************************************************************\
 * GUI events - input
\**********************************************************************/

void mainframe::OnEnter(wxCommandEvent& evt)
{
	char* pData;
	size_t len;

	if(!m_pCurrentInstance->IsConnected())
		return;

	if(m_pCurrentInstance->IsSelected(termInstance::TERM_EN_HISTORY))
	{
		std::string str = m_pTermInputBox->GetValue();
		if(!str.empty())
			m_pCurrentInstance->AddToHistory(std::move(str));
	}

	m_pTermInputBox->GetByteValue
		(&pData, &len, termInstance::MAX_ON_ENTER_STR_SIZE);
	m_pTermInputBox->ClearInput();
	m_pCurrentInstance->Write(pData, len);
}

void mainframe::OnUp(wxCommandEvent& evt)
{
	m_pTermInputBox->SetValue(m_pCurrentInstance->GetNextFromHistory());
}

void mainframe::OnDown(wxCommandEvent& evt)
{
	m_pTermInputBox->SetValue(m_pCurrentInstance->GetPrevFromHistory());
}

/**********************************************************************\
 * GUI events - other
\**********************************************************************/

void mainframe::OnRedrawLimitTimer(wxTimerEvent& evt)
{
	if(m_redrawSuppressed)
	{
		m_redrawSuppressed = false;
		m_pTermOutputBox->Redraw();
	}
}

void mainframe::OnAccelerator(wxCommandEvent& evt)
{
	int id = evt.GetId();

	switch(id)
	{
		case ID_ACCEL_UNDO:
			UndoDelete();
			break;

		case ID_ACCEL_ASCII:
			m_pTermInputSel->SetSelection(0);
			m_pTermInputBox->SetInputMode(multiBaseCommon::BASE_ASC);
			break;

		case ID_ACCEL_BIN:
			m_pTermInputSel->SetSelection(1);
			m_pTermInputBox->SetInputMode(multiBaseCommon::BASE_BIN);
			break;

		case ID_ACCEL_OCT:
			m_pTermInputSel->SetSelection(2);
			m_pTermInputBox->SetInputMode(multiBaseCommon::BASE_OCT);
			break;

		case ID_ACCEL_DEC:
			m_pTermInputSel->SetSelection(3);
			m_pTermInputBox->SetInputMode(multiBaseCommon::BASE_DEC);
			break;

		case ID_ACCEL_HEX:
			m_pTermInputSel->SetSelection(4);
			m_pTermInputBox->SetInputMode(multiBaseCommon::BASE_HEX);
			break;
	}
}

void mainframe::OnBackendEvent(backendEvent& evt)
{
	evt.GetInstance()->OnBackendEvent(evt);
}

wxBEGIN_EVENT_TABLE(mainframe, wxFrame)
	EVT_CLOSE(mainframe::OnClose)
	EVT_RIBBONBAR_HELP_CLICK(ID_MAIN_MENU, mainframe::OnAbout)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_ADD_PRESET, mainframe::OnAddPreset)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_RENAME_PRESET, mainframe::OnRenamePreset)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_DEL_PRESET, mainframe::OnDeletePreset)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_CONNECT_DISCONNECT, mainframe::OnConnectDisconnect)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_CLRTERM, mainframe::OnClrTerm)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_TX_FILE, mainframe::OnTxFile)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_SAVE_OUTPUT, mainframe::OnSaveTerm)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_START_STOP_LOG, mainframe::OnStartStopLog)
	EVT_COMBOBOX(mainframe::ID_PRESET_SELECT, mainframe::OnPresetSelect)
	EVT_COMBOBOX(mainframe::ID_CONNECTION_TYPE, mainframe::OnConnectionSettingsChanged)
	EVT_COMBOBOX(mainframe::ID_PARAM, mainframe::OnConnectionSettingsChanged)
	EVT_TEXT(mainframe::ID_PARAM, mainframe::OnConnectionSettingsChanged)
	EVT_TEXT(mainframe::ID_IFACE_NAME, mainframe::OnConnectionSettingsChanged)
	EVT_COMBOBOX(mainframe::ID_DATABITS, mainframe::OnConnectionSettingsChanged)
	EVT_COMBOBOX(mainframe::ID_PARITY, mainframe::OnConnectionSettingsChanged)
	EVT_COMBOBOX(mainframe::ID_STOPBITS, mainframe::OnConnectionSettingsChanged)
	EVT_COMBOBOX(mainframe::ID_FLOWCTRL, mainframe::OnConnectionSettingsChanged)
	EVT_COMBOBOX(mainframe::ID_ON_ENTER_SEL, mainframe::OnOnEnterSelect)
	EVT_COMBOBOX(mainframe::ID_INPUT_FORMAT, mainframe::OnInputFormatSelect)
	EVT_CHECKBOX(mainframe::ID_SHOW_ASC, mainframe::OnShowBase)
	EVT_CHECKBOX(mainframe::ID_SHOW_HEX, mainframe::OnShowBase)
	EVT_CHECKBOX(mainframe::ID_SHOW_DEC, mainframe::OnShowBase)
	EVT_CHECKBOX(mainframe::ID_SHOW_OCT, mainframe::OnShowBase)
	EVT_CHECKBOX(mainframe::ID_SHOW_BIN, mainframe::OnShowBase)
	EVT_CHECKBOX(mainframe::ID_ENABLE_HISTORY, mainframe::OnEnableHistory)
	EVT_TEXT_ENTER(mainframe::ID_HISTORY_LENGTH, mainframe::OnHistorySize)
	EVT_CHECKBOX(mainframe::ID_ENABLE_AUTOSCROLL, mainframe::OnEnableAutoscroll)
	EVT_COMBOBOX(mainframe::ID_NEWLINE_SEL, mainframe::OnNewlineSelect)
	EVT_CHECKBOX(mainframe::ID_NEWLINE_EVERY_N_ENABLE, mainframe::OnMaxLineLen)
	EVT_TEXT_ENTER(mainframe::ID_NEWLINE_EVERY_N_SEL, mainframe::OnMaxLineLen)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_LINECTRL_DSR, mainframe::OnReadInputPins)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_LINECTRL_CTS, mainframe::OnReadInputPins)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_LINECTRL_DCD, mainframe::OnReadInputPins)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_LINECTRL_RNG, mainframe::OnReadInputPins)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_LINECTRL_DTR, mainframe::OnToggleDTR)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_LINECTRL_RTS, mainframe::OnToggleRTS)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_CLEAR_STAT_RX, mainframe::OnClearStatRx)
	EVT_RIBBONBUTTONBAR_CLICKED(mainframe::ID_CLEAR_STAT_TX, mainframe::OnClearStatTx)
	EVT_CHECKBOX(mainframe::ID_SHOW_RX, mainframe::OnShowCharClass)
	EVT_CHECKBOX(mainframe::ID_SHOW_TX, mainframe::OnShowCharClass)
	EVT_CHECKBOX(mainframe::ID_SHOW_ERR, mainframe::OnShowCharClass)
	EVT_COMMAND(MBASE_INPUT_ENTER, MBASE_INPUT_EVENT, mainframe::OnEnter)
	EVT_COMMAND(MBASE_INPUT_UP, MBASE_INPUT_EVENT, mainframe::OnUp)
	EVT_COMMAND(MBASE_INPUT_DOWN, MBASE_INPUT_EVENT, mainframe::OnDown)
	EVT_TIMER(ID_REDRAW_LIMIT_TIMER, mainframe::OnRedrawLimitTimer)
	EVT_MENU(mainframe::ID_ACCEL_UNDO, mainframe::OnAccelerator)
	EVT_MENU(mainframe::ID_ACCEL_ASCII, mainframe::OnAccelerator)
	EVT_MENU(mainframe::ID_ACCEL_BIN, mainframe::OnAccelerator)
	EVT_MENU(mainframe::ID_ACCEL_OCT, mainframe::OnAccelerator)
	EVT_MENU(mainframe::ID_ACCEL_DEC, mainframe::OnAccelerator)
	EVT_MENU(mainframe::ID_ACCEL_HEX, mainframe::OnAccelerator)
	EVT_BACKEND_THREAD(wxID_ANY, BACKEND_CMD_RESULT, mainframe::OnBackendEvent)
wxEND_EVENT_TABLE()

/**********************************************************************\
 * Private functions
\**********************************************************************/

void mainframe::CreateInstance(wxString name)
{
	m_instances.emplace_back();
	termInstance& instance = *(--m_instances.end());
	instance.m_name = name;

	if(m_pCurrentInstance)
	{
		instance.m_onEnterSelection	= m_pCurrentInstance->m_onEnterSelection;
		instance.m_inputMode		= m_pCurrentInstance->m_inputMode;
		instance.m_selection		= m_pCurrentInstance->m_selection;
	}

	UpdatePresetList();
}

void mainframe::SelectInstance(wxString name)
{
	int id = 0;
	termInstance* pInstance = 0;
	for(auto& instance : m_instances)
	{
		if(instance.m_name == name)
		{
			pInstance = &instance;
			break;
		}
		++id;
	}

	// Not found
	if(!pInstance)
		return;

	m_pPresetSelect->SetSelection(id);
	if(m_pCurrentInstance)
		m_pCurrentInstance->Deselect();
	m_pCurrentInstance = pInstance;
	m_pCurrentInstance->Select();
}

void mainframe::UpdatePresetList()
{
	m_presetChanged = true;
	int selection = m_pPresetSelect->GetSelection();

	m_pPresetSelect->Freeze();
	m_pPresetSelect->Clear();

	int id = 0;
	for(const auto& instance : m_instances)
	{
		if(!instance.m_deleted)
		{
			m_pPresetSelect->Append(instance.m_name);
			++id;
		}
		else if(id == selection)
			selection = 0;
	}

	m_pPresetSelect->SetSelection(selection);
	m_pPresetSelect->Thaw();
}

void mainframe::UndoDelete()
{
	bool changed = false;
	if(m_undoCounter)
	{
		--m_undoCounter;

		for(auto& instance : m_instances)
		{
			if(instance.m_deleted > m_undoCounter)
			{
				instance.m_deleted = 0;
				changed = true;
			}
		}

		if(changed)
			UpdatePresetList();
	}
}

void mainframe::LoadPresets()
{
	FILE* file = 0;
	std::string utf8str;
	wxString wxstr;
	char* data = 0;

	Freeze();
	try
	{
		// Open config file
		file = fopen(wxString(m_configFilename.c_str()).ToUTF8().data(), "rb");

		if(!file)
			throw(std::exception());

		size_t filesize;

		fseek(file, 0, SEEK_END);
		filesize = ftell(file);
		data = new char[filesize+1];
		data[filesize] = 0;
		rewind(file);

		if(fread(data, 1, filesize, file) != filesize)
			throw(std::exception());
		YAML::Node yamlDoc = YAML::Load(data);

		// Get Version
		{
			int x;
			x = yamlDoc[NODE_VERSION].as<int>();
			if(x != FILE_FORMAT_VERSION)
			{
				wxMessageBox(MSG_OLD_CONFIG_FILE,
					MSG_OLD_CONFIG_FILE_TITLE, wxICON_INFORMATION);
				// Abort
				throw std::exception();
			}
		}

		// Get Window size
		int x;
		int y;
		bool maximized;
		x = yamlDoc[NODE_WIN_SIZE][1].as<int>();
		y = yamlDoc[NODE_WIN_SIZE][2].as<int>();
		SetSize(x, y);

		maximized = yamlDoc[NODE_WIN_SIZE][0].as<bool>();
		Maximize(maximized);

		// Read preset list
		{
			m_instances.clear();

			const YAML::Node& nPresets = yamlDoc[NODE_PRESETS];
			for(size_t i = 0; i < nPresets.size(); ++i)
			{
				m_instances.emplace_back();
				termInstance& instance = *(--m_instances.end());
				instance.LoadYaml(nPresets[i]);
			}
			UpdatePresetList();
		}

		// Read selection
		utf8str = yamlDoc[NODE_SELECTED_PRESET].as<std::string>();
		wxstr = wxString(utf8str.c_str(), wxConvUTF8);
		SelectInstance(wxstr);

		for(int x = 0; x < multiBaseCommon::COLOR_COUNT; ++x)
		{
			std::string tmp = yamlDoc[NODE_COLORS][x].as<std::string>();
			m_colors.charColors[x].Set(tmp);
		}
		for(int x = 0; x < multiBaseCommon::BASE_COUNT; ++x)
		{
			std::string tmp =
				yamlDoc[NODE_COLORS][x+multiBaseCommon::COLOR_COUNT]
				.as<std::string>();
			m_colors.baseColors[x].Set(tmp);
		}

		m_presetChanged = false;

		SetStatusText(MSG_LOAD_CONFIG_SUCCESS);
	}
	catch(...)
	{
		SetStatusText(MSG_LOAD_CONFIG_FAILED);
	}
	Thaw();

	if(data)
		delete[] data;
	if(file)
		fclose(file);
}

void mainframe::SavePresets()
{
	bool maximized = IsMaximized();
	YAML::Node yamlDoc;

	// Write version
	yamlDoc[NODE_VERSION] = int(FILE_FORMAT_VERSION);

	// Write window size and maximized state
	yamlDoc[NODE_WIN_SIZE].push_back(maximized);

	if(maximized)
	{
		Freeze();
		Maximize(false);
	}

	// Store non-maximized values
	yamlDoc[NODE_WIN_SIZE].push_back(GetSize().x);
	yamlDoc[NODE_WIN_SIZE].push_back(GetSize().y);

	if(maximized)
	{
		Maximize(true);
		Thaw();
	}

	// Write preset list
	for(auto& instance : m_instances)
	{
		if(!instance.m_deleted)
		{
			YAML::Node node;
			instance.SaveYaml(node);
			yamlDoc[NODE_PRESETS].push_back(node);
		}
	}

	// Write selection
	yamlDoc[NODE_SELECTED_PRESET] = m_pCurrentInstance->m_name.ToUTF8().data();

	for(int x = 0; x < multiBaseCommon::COLOR_COUNT; ++x)
	{
		yamlDoc[NODE_COLORS].push_back
			(m_colors.charColors[x]
			.GetAsString(wxC2S_HTML_SYNTAX).ToStdString());
	}
	for(int x = 0; x < multiBaseCommon::BASE_COUNT; ++x)
	{
		yamlDoc[NODE_COLORS].push_back
			(m_colors.baseColors[x]
			.GetAsString(wxC2S_HTML_SYNTAX).ToStdString());
	}

	WriteFormattedYAML(yamlDoc);

	m_presetChanged = false;
}

void mainframe::WriteFormattedYAML(YAML::Node& yamlDoc) const
{
	FILE* pFile;
	pFile = fopen(wxString(m_configFilename.c_str()).ToUTF8().data(), "wb");

	if(!pFile)
	{
		wxMessageBox(MSG_OPEN_CONFIG_FILE_FAILED, MSG_ERROR, wxICON_ERROR);
		return;
	}

	YAML::Emitter ymlEmitter;
	ymlEmitter << YAML::BeginMap;

	// Write version
	ymlEmitter << YAML::Key << NODE_VERSION
		<< YAML::Value << yamlDoc[NODE_VERSION];

	// Write window size and maximized state
	ymlEmitter << YAML::Key << NODE_WIN_SIZE;
	ymlEmitter << YAML::Value << YAML::BeginSeq;
	for(const auto& pos : yamlDoc[NODE_WIN_SIZE])
		ymlEmitter << pos;
	ymlEmitter << YAML::EndSeq;

	// Write preset list
	ymlEmitter << YAML::Key << NODE_PRESETS << YAML::Value << YAML::BeginSeq;
	for(const auto& pos : yamlDoc[NODE_PRESETS])
	{
		termInstance::WriteFormattedYAML(ymlEmitter, pos);
	}
	ymlEmitter << YAML::EndSeq;

	// Write selection
	ymlEmitter << YAML::Key << NODE_SELECTED_PRESET
		<< YAML::Value << yamlDoc[NODE_SELECTED_PRESET];

	// Write colors
	ymlEmitter << YAML::Key << NODE_COLORS <<
		YAML::Value << yamlDoc[NODE_COLORS];

	ymlEmitter << YAML::EndMap;

	if(fwrite(ymlEmitter.c_str(), 1, ymlEmitter.size(), pFile)
	   != ymlEmitter.size())
	{
		wxMessageBox(MSG_SAVE_CONFIG_FILE_FAILED, MSG_ERROR, wxICON_ERROR);
	}

	fclose(pFile);
}

bool mainframe::IsPositiveNumber(wxString& str, long int* num)
{
	if(!str.IsNumber())
	{
		wxMessageBox(ERR_STR_NAN, MSG_WARNING, wxICON_WARNING);
		return false;
	}

	str.ToLong(num, 10);
	if(*num <= 0)
	{
		wxMessageBox(ERR_STR_NOT_POS_NUM, MSG_WARNING, wxICON_WARNING);
		return false;
	}

	return true;
}
