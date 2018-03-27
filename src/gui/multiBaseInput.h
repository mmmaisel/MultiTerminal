/**********************************************************************\
 * MultiTerminal
 * multiBaseInput.h
 *
 * Mixed number system input box
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

#include "multiBaseCommon.h"

wxDECLARE_EVENT(MBASE_INPUT_EVENT, wxCommandEvent);
enum
{
	MBASE_INPUT_ENTER,
	MBASE_INPUT_UP,
	MBASE_INPUT_DOWN
};

class multiBaseInput
	: public wxWindow
{
	public:
		multiBaseInput(wxWindow* pParent,
			wxWindowID id, multiBaseCommon* pCommon);
		multiBaseInput(const multiBaseInput&) = delete;
		multiBaseInput(multiBaseInput&&) = delete;
		virtual ~multiBaseInput();

		void Redraw();

		struct mbase_char
		{
			// base 0 = ASCII
			inline mbase_char(char _c, uint8_t _base = 0)
				: c(_c), base(_base) {}

			inline operator char() const { return c; }
			char c;
			uint8_t base;
		};

		void ClearInput();
		inline void SetInputMode(int base) { m_inputBase = base; }

		void SetValue(const std::string& str);
		std::string GetValue() const;
		void GetByteValue
			(char** ppData, size_t* pLen, size_t extraAlloc = 0) const;

	private:
		void OnDraw(wxPaintEvent& evt);
		void OnSize(wxSizeEvent& evt);

		void OnMousePressed(wxMouseEvent& evt);
		void OnChar(wxKeyEvent& evt);

		DECLARE_EVENT_TABLE()

		void Render(wxDC& dc);

		enum
		{
			PROC_BYTE_INCOMPLETE,
			PROC_BYTE_COMPLETE,
			PROC_BYTE_OVERFLOW
		};

		static uint8_t charVal(char c, uint8_t base);
		static int ProcByteValue(const mbase_char& ch, uint8_t& retval,
			uint16_t& byteVal, uint8_t& charCount);

		std::vector<mbase_char> m_buffer;
		int m_baseNCharCount[multiBaseCommon::BASE_COUNT];
		wxFont m_termFont;
		int m_inputBase;
		size_t m_cursorPos;
		size_t m_drawStart;

		multiBaseCommon* m_pCommon;
		wxSize m_charSize;
};
