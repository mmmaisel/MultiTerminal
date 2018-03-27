/**********************************************************************\
 * MultiTerminal
 * messages.cpp
 *
 * UI string constants
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
#include "messages.h"

/**********************************************************************\
 * Global string constants.
\**********************************************************************/
const char* WINDOW_NAME				= "MultiTerminal";
const char* CONFIRM_CLOSE			= "The file has not been saved... Close anyway?";
const char* CONFIRM_CLOSE_TITLE		= "Please confirm";
const char* EMPTY_INPUT_FIELD		= "Error: One or more input fields are empty!";
const char* GENERIC_CONN_ERROR		= "Error while connecting to port or host.";
const char* GENERIC_CONN_ERROR_MSG	=
	"Can not connect to port/host.\n"
	"This can happen if the port does not exists,\n"
	"is blocked by another application or\n"
	"if the hostname can not resolved.";
const char* CONNECTION_EXISTS_ERROR_MSG =
	"Can not connect to port/host.\n"
	"The connection is already busy.";
const char* INVALID_ID				= "Invalid ID";

const char* MSG_ERROR		= "Error";
const char* MSG_INST_ERROR	= "Error in instance %s";
const char* MSG_WARNING		= "Warning";
const char* MSG_INFO		= "Information";
const char* MSG_ABOUT		= "About MultiTerminal";
const char* MSG_CLOSE		= "Close";
const char* MSG_NOT_CONNECTED="Not connected";

/**********************************************************************\
 * Ribbon string constants.
\**********************************************************************/
const char* RIBBON_SETUP			= "Setup";
const char* RIBBON_PRESET			= "Preset";
const char* RIBBON_ADD_PRESET		= "Add Preset";
const char* RIBBON_RENAME_PRESET	= "Rename Preset";
const char* RIBBON_DEL_PRESET		= "Delete Preset";
const char* RIBBON_SETTINGS			= "Settings";
const char* RIBBON_CONN_TYPE		= "Type";
const char* RIBBON_DEV_NAME			= "Device Name";
const char* RIBBON_BAUDRATE			= "Baudrate";
const char* RIBBON_FFORMAT			= "Format";
const char* TOOLTIP_DATABITS		= "Databits";
const char* TOOLTIP_PARITY			= "Parity";
const char* TOOLTIP_STOPBITS		= "Stopbits";
const char* TOOLTIP_FLOWCTRL		= "Flow Control";

const char* RIBBON_TERMINAL			= "Terminal";
const char* RIBBON_CONNECTION		= "Connection";
const char* RIBBON_CONNECT			= "Connect";
const char* RIBBON_DISCONNECT		= "Disconnect";
const char* RIBBON_CLR_TERM			= "Clear Terminal";
const char* RIBBON_TX_FILE			= "Transmit file";
const char* RIBBON_LOGGING			= "Logging";
const char* RIBBON_LOG_SAVE			= "Save Output";
const char* RIBBON_LOG_START		= "Start Logging";
const char* RIBBON_LOG_STOP			= "Stop Logging";
const char* RIBBON_SEND_ON_ENTER	= "Send on Enter";
const char* RIBBON_INPUT_FORMAT		= "Input format";
const char* RIBBON_ENABLE_HISTORY	= "Enable History";
const char* RIBBON_HISTORY_LEN		= "History length";
const char* TOOLTIP_ENABLE_HISTORY	=
	"Disable terminal input history e.g. when"
	"transmitting passwords over terminal!";
const char* TOOLTIP_SAVE_OUTPUT		= "Saves Terminal Output";

const char* RIBBON_VIEW				= "View";
const char* RIBBON_NUMSYS			= "Number Systems";
const char* RIBBON_SHOW_ASC			= "Show ASCII";
const char* RIBBON_SHOW_HEX			= "Show Hex";
const char* RIBBON_SHOW_DEC			= "Show Decimal";
const char* RIBBON_SHOW_OCT			= "Show Octal";
const char* RIBBON_SHOW_BIN			= "Show Binary";
const char* RIBBON_TERM_WND			= "Terminal Window";
const char* RIBBON_SHOW_RX			= "Show Received";
const char* RIBBON_SHOW_TX			= "Show Transmitted";
const char* RIBBON_SHOW_ERR			= "Show Errors";
const char* RIBBON_AUTOSCROLL		= "Autoscroll";
const char* RIBBON_NEWLINES			= "Newline config";
const char* RIBBON_NEWLINE_AT		= "Newline at";
const char* RIBBON_NEWLINE_EVERY_N	= "Newline every";

const char* RIBBON_ADVANCED			= "Advanced";
const char* RIBBON_INPUT_PINS		= "Input pins";
const char* RIBBON_OUTPUT_PINS		= "Output pins";
const char* RIBBON_STATISTICS		= "Statistics";
const char* RIBBON_STAT_CLEAR_RX	= "Clear RX";
const char* RIBBON_STAT_CLEAR_TX	= "Clear TX";
const char* RIBBON_STAT_COUNT_RX	= "RX count";
const char* RIBBON_STAT_COUNT_TX	= "TX count";

const char* ADD_PRESET_PROMPT			= "Preset name:";
const char* MSG_CONFIRM_DEL_PRESET		= "Delete Preset %s?";
const char* MSG_OLD_CONFIG_FILE			=
	"Settings file from a different version\n"
	"of MultiTerminal detected. It will be overwritten\n"
	"with the new version on save!";
const char* MSG_OLD_CONFIG_FILE_TITLE	= "Different file version detected";

const char* ERR_PRESET_NAME_EMPTY	= "Preset name must not be empty!";
const char* ERR_PRESET_NAME_EXISTS	= "Preset name already exists!";
const char* ERR_PRESET_DELETE_LAST	= "Can't delete last preset!";
const char* ERR_STR_NAN				= "String value is not a number!";
const char* ERR_STR_NOT_POS_NUM		= "String value is not positive!";

const char* MSG_CONNECT_ERR			= "Connecting to %s failed, Error = %s";
const char* MSG_CONNECT_BAUD_ERR	= "Setting custom baudrate failed!";
const char* MSG_ALREADY_CONNECTED	= "Terminal is already connected to %s";
const char* MSG_READ_ERR			= "Read failed: Error = %s";
const char* MSG_WRITE_ERR			= "Write failed: Error = %s, written bytes: %d";
const char* MSG_DISCONNECT_ERR		= "Terminal disconnected: Error = %s";
const char* MSG_SET_PIN_ERR			= "Toggle %s Pin failed, Error = %s";
const char* MSG_READ_PIN_ERR		= "Read Pin values failed, Error = %s";
const char* MSG_SAVE_LOG_ERR		= "Save terminal data failed, Error = Can not open file %s";
const char* MSG_START_LOG_ERR		= "Start logging failed, Error = Can not open file %s";
const char* MSG_FATAL_BACKEND_ERR	= "Critical failure in connection backend detected! Shutting down!";
const char* MSG_UNKNOWN_CMDEVT_ERR	= "Unknown command event received";
const char* MSG_FILE_NOT_FOUND		= "File %s was not found!";
const char* MSG_TX_FILE_ERR			= "Transmit file failed: Error = %s";
const char* MSG_TX_FILE_COMPLETE	= "File successfully transmitted.";

const char* MSG_STATUS_CONNECTING	= "Connecting to %s ...";
const char* MSG_STATUS_CONNECTED	= "Connected to %s";
const char* MSG_STATUS_DISCONNECTING= "Disconnecting terminal ...";
const char* MSG_STATUS_DISCONNECTED	= "Terminal disconnected";
const char* MSG_STATUS_SET_PIN		= "Toggled %s Pin";
const char* MSG_STATUS_READ_PIN		= "Read pin values";
const char* MSG_STATUS_SAVE_LOG		= "Saved terminal data to %s";
const char* MSG_STATUS_START_LOG	= "Logging to %s";
const char* MSG_STATUS_STOP_LOG		= "Logging stopped";
const char* MSG_STATUS_TX_FILE		= "Transmitting file %s";

const char* MSG_LOAD_CONFIG_SUCCESS		= "Successfully loaded config file";
const char* MSG_LOAD_CONFIG_FAILED		= "Loading config file failed!";
const char* MSG_OPEN_CONFIG_FILE_FAILED = "Open config file failed!";
const char* MSG_SAVE_CONFIG_FILE_FAILED = "Writing config file failed!";

const char* SELECTOR_ALL_FILES = "All files|*";
const char* SELECTOR_RAW_OUTPUT		= "Raw Terminal Output (*)|*";
