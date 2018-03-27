/**********************************************************************\
 * MultiTerminal
 * multiBaseInput.cpp
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
#include "stdafx.h"
#include "multiBaseInput.h"

wxDEFINE_EVENT(MBASE_INPUT_EVENT, wxCommandEvent);

multiBaseInput::multiBaseInput(wxWindow* pParent,
	wxWindowID id, multiBaseCommon* pCommon)
	: wxWindow(pParent, id, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS)
	, m_baseNCharCount{0}
	, m_termFont(wxFontInfo(12).FaceName("Courier"))
	, m_inputBase(multiBaseCommon::BASE_ASC)
	, m_cursorPos(0)
	, m_drawStart(0)
	, m_pCommon(pCommon)
{
	m_buffer.reserve(1024);
	wxSize minSize(100, 20);
	SetMinSize(minSize);
}

multiBaseInput::~multiBaseInput()
{
}

void multiBaseInput::Redraw()
{
	wxClientDC dc(this);
	Render(dc);
}

void multiBaseInput::ClearInput()
{
	m_buffer.clear();
	m_cursorPos = 0;
	memset(m_baseNCharCount, 0,
		multiBaseCommon::BASE_COUNT*sizeof(m_baseNCharCount[0]));
	Redraw();
}

void multiBaseInput::SetValue(const std::string& str)
{
	uint8_t base = multiBaseCommon::BASE_ASC;
	bool escape = false;

	m_buffer.clear();
	m_buffer.reserve(str.size());

	for(const auto& it : str)
	{
		if(escape)
		{
			if(it == '\\')
				m_buffer.push_back(mbase_char('\\', multiBaseCommon::BASE_ASC));
			else if(it == 'x')
				base = multiBaseCommon::BASE_HEX;
			else if(it == 'd')
				base = multiBaseCommon::BASE_DEC;
			else if(it == 'o')
				base = multiBaseCommon::BASE_OCT;
			else if(it == 'b')
				base = multiBaseCommon::BASE_BIN;
			else if(it == 'A')
				base = multiBaseCommon::BASE_ASC;
			// else: all other escape sequences are ignored
			escape = false;
		}
		else if(it == '\\')
		{
			escape = true;
		}
		// Normal data
		else
		{
			m_buffer.push_back(mbase_char(it, base));
			++m_baseNCharCount[base];
		}
	}
	m_cursorPos = m_buffer.size();
	m_drawStart = 0;
	Redraw();
}

std::string multiBaseInput::GetValue() const
{
	uint8_t base = multiBaseCommon::BASE_ASC;
	std::string retval;
	retval.reserve(m_buffer.size());

	for(const auto& it : m_buffer)
	{
		if(it.base != base)
		{
			base = it.base;
			retval.push_back('\\');
			if(it.base == multiBaseCommon::BASE_ASC)
				retval.push_back('A');
			else if(it.base == multiBaseCommon::BASE_BIN)
				retval.push_back('b');
			else if(it.base == multiBaseCommon::BASE_OCT)
				retval.push_back('o');
			else if(it.base == multiBaseCommon::BASE_DEC)
				retval.push_back('d');
			else if(it.base == multiBaseCommon::BASE_HEX)
				retval.push_back('x');
		}

		if(it.c == '\\')
			retval.push_back('\\');

		retval.push_back(it.c);
	}
	return std::move(retval);
}

void multiBaseInput::GetByteValue
	(char** ppData, size_t* pLen,  size_t extraAlloc) const
{
	int lastBase = -1;
	uint8_t  digitCount = 0;
	uint8_t  pushVal = 0;
	uint16_t byteVal = 0;

	char* pData = new char[m_buffer.size() + extraAlloc];
	size_t len = 0;

	for(const auto& it : m_buffer)
	{
		// Save incomplete byte
		if(lastBase != it.base && digitCount != 0)
		{
			pData[len++] = byteVal;
			digitCount = 0;
			byteVal = 0;
		}

		if(it.base == multiBaseCommon::BASE_ASC)
		{
			pData[len++] = it.c;
		}
		else
		{
			if(ProcByteValue(it, pushVal, byteVal, digitCount))
				pData[len++] = pushVal;
		}

		lastBase = it.base;
	}

	// Save incomplete byte
	if(digitCount != 0)
		pData[len++] = byteVal;

	*ppData = pData;
	*pLen = len;
}

void multiBaseInput::OnDraw(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	Render(dc);
}

void multiBaseInput::OnSize(wxSizeEvent& evt)
{
	Refresh();
}

void multiBaseInput::OnMousePressed(wxMouseEvent& evt)
{
	SetFocus();
}

void multiBaseInput::OnChar(wxKeyEvent& evt)
{
	if(evt.GetKeyCode() == WXK_DELETE)
	{
		if(m_cursorPos < m_buffer.size())
		{
			auto it = m_buffer.begin()+m_cursorPos;
			--m_baseNCharCount[it->base];
			m_buffer.erase(it);
		}
	}
	else if(evt.GetKeyCode() == WXK_BACK)
	{
		if(m_cursorPos != 0 && m_buffer.size())
		{
			auto it = m_buffer.begin()+(--m_cursorPos);
			--m_baseNCharCount[it->base];
			m_buffer.erase(it);
		}
	}
	else if(evt.GetKeyCode() == WXK_RETURN)
	{
		wxCommandEvent* pEvent =
			new wxCommandEvent(MBASE_INPUT_EVENT, MBASE_INPUT_ENTER);
		wxQueueEvent(m_parent, pEvent);
	}
	else if(evt.GetKeyCode() == WXK_UP)
	{
		wxCommandEvent* pEvent =
			new wxCommandEvent(MBASE_INPUT_EVENT, MBASE_INPUT_UP);
		wxQueueEvent(m_parent, pEvent);
	}
	else if(evt.GetKeyCode() == WXK_DOWN)
	{
		wxCommandEvent* pEvent =
			new wxCommandEvent(MBASE_INPUT_EVENT, MBASE_INPUT_DOWN);
		wxQueueEvent(m_parent, pEvent);
	}
	else if(evt.GetKeyCode() == WXK_LEFT)
	{
		if(m_cursorPos != 0)
			--m_cursorPos;
	}
	else if(evt.GetKeyCode() == WXK_RIGHT)
	{
		if(m_cursorPos != m_buffer.size())
			++m_cursorPos;
	}
	else if(evt.GetKeyCode() == WXK_HOME)
	{
		m_cursorPos = 0;
	}
	else if(evt.GetKeyCode() == WXK_END)
	{
		m_cursorPos = m_buffer.size();
	}
	else
	{
		unsigned int keyCode = evt.GetKeyCode();

		if(m_inputBase == multiBaseCommon::BASE_ASC)
		{
			// Block invalid (non ASCII) input
			if(keyCode < 0x80)
			{
				m_buffer.insert(m_buffer.begin()+m_cursorPos,
					mbase_char(keyCode, m_inputBase));
				++m_cursorPos;
				++m_baseNCharCount[m_inputBase];
			}
		}
		else if(m_inputBase == multiBaseCommon::BASE_BIN)
		{
			// Block non binary input
			if(keyCode == '0' || keyCode == '1')
			{
				m_buffer.insert(m_buffer.begin()+m_cursorPos,
					mbase_char(keyCode, m_inputBase));
				++m_cursorPos;
				++m_baseNCharCount[m_inputBase];
			}
		}
		else if(m_inputBase == multiBaseCommon::BASE_OCT)
		{
			// Block non octal input
			if(keyCode >= '0' && keyCode <= '7')
			{
				m_buffer.insert(m_buffer.begin()+m_cursorPos,
					mbase_char(keyCode, m_inputBase));
				++m_cursorPos;
				++m_baseNCharCount[m_inputBase];
			}
		}
		else if(m_inputBase == multiBaseCommon::BASE_DEC)
		{
			// Block non decimal input
			if(isdigit(keyCode))
			{
				m_buffer.insert(m_buffer.begin()+m_cursorPos,
					mbase_char(keyCode, m_inputBase));
				++m_cursorPos;
				++m_baseNCharCount[m_inputBase];
			}
		}
		else if(m_inputBase == multiBaseCommon::BASE_HEX)
		{
			// Block non hex input
			if(isxdigit(keyCode))
			{
				if(islower(keyCode))
				{
					// Convert to upper case
					keyCode -= 0x20;
				}
				m_buffer.insert(m_buffer.begin()+m_cursorPos,
					mbase_char(keyCode, m_inputBase));
				++m_cursorPos;
				++m_baseNCharCount[m_inputBase];
			}
		}
	}
	Redraw();
}

BEGIN_EVENT_TABLE(multiBaseInput, wxWindow)
	EVT_PAINT(multiBaseInput::OnDraw)
	EVT_SIZE(multiBaseInput::OnSize)
	EVT_LEFT_DOWN(multiBaseInput::OnMousePressed)
	EVT_CHAR(multiBaseInput::OnChar)
END_EVENT_TABLE()

void multiBaseInput::Render(wxDC&  dc)
{
	bool splitNow = false;
	bool splitNext= false;
	bool asciiOnly= false;
	uint16_t tmpVal   = 0;
	uint8_t  blockLen = 0;
	uint8_t  lastBase = 0xFF;
	wxPoint  currentPos = {1, 1};

	// fill background
	dc.SetBrush(*wxBLACK_BRUSH);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(wxPoint(0, 0), GetSize());

	dc.SetFont(m_termFont);
	dc.SetTextForeground(*wxWHITE);

	// Get size of constant size char
	m_charSize = dc.GetTextExtent("a");

	size_t visibleChars = GetSize().x / (m_charSize.x+2) - 1;

	// Scroll in textbox with cursor,
	// keep one char before/behind cursor visible
	if(m_cursorPos > visibleChars + m_drawStart - 1)	// right
	{
		if(m_cursorPos != m_buffer.size())
			m_drawStart = m_cursorPos - visibleChars + 1;
		else
			m_drawStart = m_cursorPos - visibleChars;
	}
	else if(m_cursorPos < m_drawStart + 1)				// left
	{
		if(m_cursorPos != 0)
			m_drawStart = m_cursorPos - 1;
		else
			m_drawStart = m_cursorPos;
	}

	if(m_buffer.size())
		lastBase = m_buffer[0].base;

	if(m_baseNCharCount[multiBaseCommon::BASE_ASC] != 0)
	{
		asciiOnly = true;
		dc.SetBrush(*wxTRANSPARENT_BRUSH);

		for(int x = multiBaseCommon::BASE_ASC+1;
			x < multiBaseCommon::BASE_COUNT; ++x)
		{
			if(m_baseNCharCount[x] != 0)
			{
				asciiOnly = false;
				break;
			}
		}
	}

	size_t pos = 0;
	for(const auto& ch : m_buffer)
	{
		if(pos < m_drawStart)
		{
			++pos;
			continue;
		}

		if(ch.base != lastBase)
		{
			splitNow = true;
			lastBase = ch.base;
			blockLen = 0;
			tmpVal = 0;
		}
		if(splitNext)
		{
			splitNow = true;
			splitNext = false;
		}
		if(ch.base != multiBaseCommon::BASE_ASC)
		{
			uint8_t dummy;
			int result = ProcByteValue(ch, dummy, tmpVal, blockLen);
			if(result == PROC_BYTE_OVERFLOW)
				splitNow = true;
			else if(result == PROC_BYTE_COMPLETE)
				splitNext = true;
		}
		else
			++blockLen;

		if(splitNow)
		{
			++currentPos.x;
			splitNow = false;
		}

		if(!asciiOnly)
			dc.SetBrush(m_pCommon->baseColors[ch.base]);

		dc.DrawRectangle(currentPos, wxSize(m_charSize.x+2, m_charSize.y+2));
		if(ch.c < 0x80)
			dc.DrawText(ch.c, currentPos.x+1, currentPos.y+1);

		if(pos == m_cursorPos)
		{
			dc.SetPen(*wxWHITE_PEN);
			dc.DrawLine(wxPoint(currentPos.x-1, currentPos.y),
				wxPoint(currentPos.x-1, currentPos.y+m_charSize.y));
			dc.SetPen(*wxTRANSPARENT_PEN);
		}

		currentPos.x += m_charSize.x+2;

		// Don't draw invisible chars
		if(pos > visibleChars+m_drawStart)
			break;

		++pos;
	}

	// Draw cursor at end, not at end cursor is drawn in loop
	if(pos == m_cursorPos)
	{
		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(wxPoint(currentPos.x-1, currentPos.y),
			wxPoint(currentPos.x-1, currentPos.y+m_charSize.y));
		dc.SetPen(*wxTRANSPARENT_PEN);
	}
}

uint8_t multiBaseInput::charVal(char c, uint8_t base)
{
	if(base <= 10)
	{
		if(c >= '0' && c <= '0' + base - 1)
			return c - '0';
		else
			return -1;
	}
	else
	{
		if(c >= '0' && c <= '9')
			return c - '0';
		else if(c >= 'A' && c <= 'A' + base - 11)
			return c - 0x37;
		else if(c >= 'a' && c <= 'a' + base - 11)
			return c - 0x57;
		else
			return -1;
	}
}

int multiBaseInput::ProcByteValue(const mbase_char& ch, uint8_t& retval,
	uint16_t& byteVal, uint8_t& charCount)
{
	uint16_t newVal = byteVal;
	newVal *= multiBaseCommon::BASE_RADIX(ch.base);
	newVal += charVal(ch.c, multiBaseCommon::BASE_RADIX(ch.base));
	++charCount;

	if(newVal > 0xFF)
	{
		retval = byteVal;
		charCount = 1;
		byteVal = charVal(ch.c, multiBaseCommon::BASE_RADIX(ch.base));
		return PROC_BYTE_OVERFLOW;
	}
	else if(charCount == multiBaseCommon::BASE_MAX_DIGITS(ch.base))
	{
		retval = newVal;
		charCount = 0;
		byteVal = 0;
		return PROC_BYTE_COMPLETE;
	}
	else
	{
		byteVal = newVal;
		return PROC_BYTE_INCOMPLETE;
	}
}
