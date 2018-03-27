/**********************************************************************\
 * MultiTerminal
 * termFileDlgExtra.h
 *
 * Log file dialog extra controls
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

class termFileDlgExtra
	: public wxWindow
{
	public:
		termFileDlgExtra(wxWindow* parent, wxWindowID id);
		termFileDlgExtra(const termFileDlgExtra&) = delete;
		termFileDlgExtra(termFileDlgExtra&&) = delete;

		inline bool AppendToFile() const
			{ return m_pCheckbox->IsChecked(); }

		static wxWindow* CreateFuncAppend(wxWindow* pParent);
		static wxWindow* CreateFuncNoAppend(wxWindow* pParent);

	private:
		wxCheckBox * m_pCheckbox;
};

