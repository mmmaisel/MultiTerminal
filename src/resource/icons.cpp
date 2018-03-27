/**********************************************************************\
 * MultiTerminal
 * icons.cpp
 *
 * Icon paths string constants
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
#include "icons.h"

#ifndef LOCALINSTALL
#define ICON_INSTALL_PREFIX ICON_DIR
#else
#define ICON_INSTALL_PREFIX ""
#endif

const char* ICON_MULTITERM			= ICON_INSTALL_PREFIX "icons/MultiTerminal_128.png";

const char* ICON_ADD_PRESET			= ICON_INSTALL_PREFIX "icons/add-preset.png";
const char* ICON_EDIT_PRESET		= ICON_INSTALL_PREFIX "icons/edit-preset.png";
const char* ICON_DEL_PRESET			= ICON_INSTALL_PREFIX "icons/del-preset.png";

const char* ICON_TERM_CONNECT		= ICON_INSTALL_PREFIX "icons/connect.png";
const char* ICON_TERM_DISCONNECT	= ICON_INSTALL_PREFIX "icons/disconnect.png";

const char* ICON_TERM_SAVE			= ICON_INSTALL_PREFIX "icons/save-term.png";
const char* ICON_TERM_CLEAR			= ICON_INSTALL_PREFIX "icons/clear-term.png";
const char* ICON_TX_FILE			= ICON_INSTALL_PREFIX "icons/tx-file.png";
const char* ICON_START_LOG			= ICON_INSTALL_PREFIX "icons/start-log.png";
const char* ICON_STOP_LOG			= ICON_INSTALL_PREFIX "icons/stop-log.png";

const char* ICON_PIN_ACTIVE			= ICON_INSTALL_PREFIX "icons/pin-active.png";
const char* ICON_PIN_INACTIVE		= ICON_INSTALL_PREFIX "icons/pin-inactive.png";
const char* ICON_CLEAR_STAT_RX		= ICON_INSTALL_PREFIX "icons/clear-rx.png";
const char* ICON_CLEAR_STAT_TX		= ICON_INSTALL_PREFIX "icons/clear-tx.png";
