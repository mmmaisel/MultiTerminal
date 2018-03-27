/**********************************************************************\
 * MultiTerminal
 * configFile.h
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
#pragma once

enum : uint8_t { FILE_FORMAT_VERSION = 1 };

extern const char* CONFIG_FILENAME;
extern const char* NODE_VERSION;
extern const char* NODE_WIN_SIZE;

extern const char* NODE_PRESETS;
extern const char* NODE_PRESET_NAME;
extern const char* NODE_PRESET_IFTYPE;
extern const char* NODE_PRESET_IFTYPE_UNKNOWN;
extern const char* NODE_PRESET_IFTYPE_SERIAL;
extern const char* NODE_PRESET_IFTYPE_TCP;
extern const char* NODE_PRESET_IFNAME;
extern const char* NODE_PRESET_PARAM;
extern const char* NODE_PRESET_FFORMAT;
extern const char* NODE_PRESET_ON_ENTER;
extern const char* NODE_PRESET_INPUT_MODE;
extern const char* NODE_PRESET_SELECTION;
extern const char* NODE_PRESET_HISTORY;
extern const char* NODE_PRESET_HISTORY_LEN;
extern const char* NODE_PRESET_LOGDIR;
extern const char* NODE_PRESET_NEWLINE;
extern const char* NODE_PRESET_LINESIZE;

extern const char* NODE_SELECTED_PRESET;
extern const char* NODE_COLORS;
