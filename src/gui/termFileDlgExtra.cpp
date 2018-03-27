/**********************************************************************\
 * MultiTerminal
 * termFileDlgExtra.cpp
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
#include "stdafx.h"
#include "termFileDlgExtra.h"

termFileDlgExtra::termFileDlgExtra(wxWindow* parent, wxWindowID id)
	: wxWindow(parent, id, wxDefaultPosition, wxDefaultSize)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	m_pCheckbox = new wxCheckBox(this, wxID_ANY, "Append to file",
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_pCheckbox->SetValue(false);

	// XXX: right align does not work
	sizer->AddStretchSpacer(1);
	sizer->Add(m_pCheckbox, 0, wxLEFT | wxRIGHT | wxTOP, 5);
	SetSizerAndFit(sizer);
}

wxWindow* termFileDlgExtra::CreateFuncAppend(wxWindow* pParent)
{
	termFileDlgExtra* pCtrl = new termFileDlgExtra(pParent, wxID_ANY);
	pCtrl->m_pCheckbox->SetValue(true);
	return pCtrl;
}

wxWindow* termFileDlgExtra::CreateFuncNoAppend(wxWindow* pParent)
{
	termFileDlgExtra* pCtrl = new termFileDlgExtra(pParent, wxID_ANY);
	pCtrl->m_pCheckbox->SetValue(false);
	return pCtrl;
}
