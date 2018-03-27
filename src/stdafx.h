/**********************************************************************\
 * MultiTerminal
 * stdafx.h
 *
 * Precompiled Header
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

// C runtime header files
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

// Linux header files
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/serial.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

// WxWidgets header files
#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/dialog.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/tooltip.h>
#include <wx/tipwin.h>

// C++ header files
#include <string>
#include <vector>
#include <list>
#include <map>
#include <future>
#include <functional>
#include <thread>

// YAML
#include <yaml-cpp/yaml.h>

// Boost
#include <boost/circular_buffer.hpp>
