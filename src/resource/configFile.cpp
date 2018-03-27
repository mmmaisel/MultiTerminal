/**********************************************************************\
 * MultiTerminal
 * configFile.cpp
 *
 * Config/Workspace file string constants
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
#include "configFile.h"

const char* CONFIG_FILENAME		= "wxTerminal.yml";
const char* NODE_VERSION		= "version";
const char* NODE_WIN_SIZE		= "windowSize";

const char* NODE_PRESETS				= "presets";
const char* NODE_PRESET_NAME			= "name";
const char* NODE_PRESET_IFTYPE			= "iface_type";
const char* NODE_PRESET_IFTYPE_UNKNOWN	= "unknown";
const char* NODE_PRESET_IFTYPE_SERIAL	= "serial";
const char* NODE_PRESET_IFTYPE_TCP		= "tcp";
const char* NODE_PRESET_IFNAME			= "iface_name";
const char* NODE_PRESET_PARAM			= "param";
const char* NODE_PRESET_FFORMAT			= "format";
const char* NODE_PRESET_ON_ENTER		= "on_enter";
const char* NODE_PRESET_INPUT_MODE		= "input_mode";
const char* NODE_PRESET_SELECTION		= "selection";
const char* NODE_PRESET_HISTORY			= "history";
const char* NODE_PRESET_HISTORY_LEN		= "history_len";
const char* NODE_PRESET_LOGDIR			= "logdir";
const char* NODE_PRESET_NEWLINE			= "newline_char";
const char* NODE_PRESET_LINESIZE		= "linesize";

const char* NODE_SELECTED_PRESET= "preset";
const char* NODE_COLORS			= "colors";

