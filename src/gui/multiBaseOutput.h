/**********************************************************************\
 * MultiTerminal
 * multiBaseOutput.h
 *
 * Mixed number system output box
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
#include "termChar.h"

wxDECLARE_EVENT(MBASE_OUTPUT_EVENT, wxCommandEvent);

class multiBaseOutput
	: public wxScrolledWindow
{
	public:
		multiBaseOutput(wxWindow* pParent,
			wxWindowID id, multiBaseCommon* pCommon);
		multiBaseOutput(const multiBaseOutput&) = delete;
		multiBaseOutput(multiBaseOutput&&) = delete;
		virtual ~multiBaseOutput();

		void Redraw();

		// Use (1 << base) here
		inline void SetBase(uint8_t x) { m_basesEnabled = x; }
		inline void EnableBase(uint8_t x, bool enabled)
		{
			if(enabled) m_basesEnabled |= (1 << x);
			else m_basesEnabled &= ~(1 << x);
		}
		inline void EnableCharClass(uint8_t x, bool enabled)
		{
			if(enabled) m_charClassesEnabled |= (1 << x);
			else m_charClassesEnabled &= ~(1 << x);
		}
		inline void EnableAutoscroll(bool x) { m_autoscroll = x; }
		inline void SetNewlineChar(uint8_t c, bool en)
			{ m_newlineChar = c; if(!en) c |= 0x8000; }
		inline void SetMaxLineLength(int maxLen) { m_maxLineLen = maxLen; }

		inline void SetBuffer(const boost::circular_buffer<term_char>* pBuffer)
			{ m_pBuffer = pBuffer; }

	private:
		virtual void OnDraw(wxDC& dc) override;
		void OnSize(wxSizeEvent& evt);

		void Render(wxDC& dc);

		enum : uint8_t { SCROLL_UNIT = 8 };

		const boost::circular_buffer<term_char>* m_pBuffer;
		wxFont m_termFont;
		uint8_t m_basesEnabled;
		uint8_t m_charClassesEnabled;
		uint16_t m_newlineChar;
		int m_maxLineLen;
		bool m_autoscroll;
		wxSize m_charSize;

		wxBitmap* m_pBMP;
		multiBaseCommon* m_pCommon;

		wxDECLARE_EVENT_TABLE();
};
