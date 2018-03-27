/**********************************************************************\
 * MultiTerminal
 * multiBaseOutput.cpp
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
#include "stdafx.h"
#include "multiBaseOutput.h"
#include "itoa.h"

wxDEFINE_EVENT(MBASE_OUTPUT_EVENT, wxCommandEvent);

multiBaseOutput::multiBaseOutput(wxWindow* pParent,
	wxWindowID id, multiBaseCommon* pCommon)
	: wxScrolledWindow(pParent, id)
	, m_pBuffer(0)
	, m_termFont(wxFontInfo(12).FaceName("Courier"))
	, m_basesEnabled((1 << multiBaseCommon::BASE_ASC))
	, m_charClassesEnabled(0xFF)
	, m_newlineChar('\n')
	, m_maxLineLen(INT_MAX)
	, m_autoscroll(true)
	, m_pBMP(0)
	, m_pCommon(pCommon)
{
}

multiBaseOutput::~multiBaseOutput()
{
	if(m_pBMP)
	{
		delete m_pBMP;
		m_pBMP = 0;
	}
}

void multiBaseOutput::Redraw()
{
	wxClientDC dc(this);
	DoPrepareDC(dc);
	Render(dc);
}

void multiBaseOutput::OnDraw(wxDC& dc)
{
	Render(dc);
}

void multiBaseOutput::OnSize(wxSizeEvent& evt)
{
	Refresh();
}

BEGIN_EVENT_TABLE(multiBaseOutput, wxWindow)
	EVT_SIZE(multiBaseOutput::OnSize)
END_EVENT_TABLE()

void multiBaseOutput::Render(wxDC& _dc)
{
	int posInLine = 0;
	wxPoint pos(GetScrollPos(wxHORIZONTAL), GetScrollPos(wxVERTICAL));
	wxPoint currentPos = {1, 1};
	wxSize totalSize;
	wxSize elementSize;
	uint8_t heightMul = 0;
	uint8_t widthMul = 1;

	wxPoint start = GetViewStart() * SCROLL_UNIT;
	wxSize  wndSize=GetClientSize();
	wxMemoryDC dc;

	if(!m_pBuffer)
		return;

	if(!m_pBMP)
		m_pBMP = new wxBitmap(wndSize);
	else if(m_pBMP->GetWidth() != wndSize.x || m_pBMP->GetHeight() != wndSize.y)
	{
		delete m_pBMP;
		m_pBMP = new wxBitmap(wndSize);
	}

	dc.SelectObject(*m_pBMP);

	// Fill background
	dc.SetBrush(*wxBLACK_BRUSH);
	dc.DrawRectangle(wxPoint(0,0), wndSize);

	dc.SetFont(m_termFont);

	// XXX: get OS dependent scrollbar width - not possible
	//      without scrollbar size subtraction: shadowed chars

	for(uint8_t x = 1; x; x <<= 1)
	{
		if(m_basesEnabled & x)
			++heightMul;
	}

	// Get size of constant size char
	m_charSize = dc.GetTextExtent("a");
	totalSize = {GetSize().x - 20, m_charSize.y*heightMul+2};  // -20: scrollbar width

	if(m_basesEnabled & (1 << multiBaseCommon::BASE_BIN))
		widthMul = 8;
	else if(m_basesEnabled & ((1 << multiBaseCommon::BASE_OCT)
			| (1 << multiBaseCommon::BASE_DEC)))
		widthMul = 3;
	else if(m_basesEnabled & (1 << multiBaseCommon::BASE_HEX))
		widthMul = 2;
	// else HEX: widthMul=1 == default value

	elementSize.x = m_charSize.x*widthMul+2;
	elementSize.y = m_charSize.y*heightMul+2;

	for(const auto& ch : *m_pBuffer)
	{
		uint8_t charRow = 0;
		if(!(m_charClassesEnabled & (1 << ch.colId)))
			continue;

		dc.SetTextForeground(m_pCommon->charColors[ch.colId]);

		// Check if drawing element would be visible.
		// Check only in y direction as there is no scrolling in x direction.
		if(currentPos.y+elementSize.y > start.y && currentPos.y < start.y+wndSize.y)
		{
			for(int x = 0; x < multiBaseCommon::BASE_COUNT; ++x)
			{
				if(m_basesEnabled & (1 << x))
				{
					if(x == multiBaseCommon::BASE_ASC)
					{
						if(m_basesEnabled != (1 << multiBaseCommon::BASE_ASC))
						{
							dc.SetBrush(m_pCommon->baseColors[x]);
							dc.DrawRectangle(wxPoint(currentPos.x, currentPos.y-start.y),
								wxSize(elementSize.x, m_charSize.y+2));
						}

						// Printable char: draw it
						if(static_cast<uint8_t>(ch.c) <  0x7F &&
						   static_cast<uint8_t>(ch.c) >= 0x20)
						{
							dc.DrawText(ch.c, currentPos.x+1+m_charSize.x*(widthMul-1)/2,
								currentPos.y+1-start.y);
						}
						// Non printable char: draw placeholder
						else
						{
							wxSize placeholderSize = m_charSize;
							placeholderSize.x -= 6;
							placeholderSize.y -= 10;

							wxPoint pt(
								currentPos.x+1+m_charSize.x*(widthMul-1)/2+3,
								currentPos.y+1-start.y+5);

							dc.SetPen(m_pCommon->charColors[ch.colId]);
							dc.DrawRectangle(pt, placeholderSize);
							dc.SetPen(*wxBLACK_PEN);
						}
					}
					else
					{
						char buffer[8*sizeof(char)+1];
						// Cast "ch" to uint8_t to force unsigned extension to int.
						itoa((uint8_t)ch, buffer, multiBaseCommon::BASE_RADIX(x));
						int len = strlen(buffer);

						// Add leading zeros in case of BIN, OCT or HEX
						if(x == multiBaseCommon::BASE_BIN ||
						   x == multiBaseCommon::BASE_OCT ||
						   x == multiBaseCommon::BASE_HEX)
						{
							if(len != multiBaseCommon::BASE_MAX_DIGITS(x))
							{
								int zeros = multiBaseCommon::BASE_MAX_DIGITS(x) - len;
								memmove(buffer+zeros, buffer, len+1);
								memset(buffer, '0', zeros);
								len = multiBaseCommon::BASE_MAX_DIGITS(x);
							}
						}

						dc.SetBrush(m_pCommon->baseColors[x]);

						wxSize rectSize(elementSize.x, m_charSize.y + 2);
						wxPoint rectPos
						(
							currentPos.x,
							currentPos.y + m_charSize.y * charRow - start.y
						);

						wxCoord textX =
							currentPos.x+1 + m_charSize.x*(widthMul-len)/2;
						wxCoord textY =
							currentPos.y+1 + m_charSize.y*charRow - start.y;

						dc.DrawRectangle(rectPos, rectSize);
						dc.DrawText(buffer, textX, textY);
					}
					++charRow;
				}
			}
		}

		currentPos.x += m_charSize.x*widthMul+2;
		++posInLine;

		// Wrap long lines, scroll only in y direction.
		if(currentPos.x + m_charSize.x*widthMul+2 > totalSize.x
		   || static_cast<uint16_t>(ch.c) == m_newlineChar
		   || posInLine >= m_maxLineLen)
		{
			posInLine = 0;
			currentPos.x = 1;
			currentPos.y += elementSize.y;
			totalSize.y += elementSize.y;
		}
	}

	dc.SelectObject(wxNullBitmap);
	_dc.DrawBitmap(*m_pBMP, start);

	SetVirtualSize(totalSize);
	SetScrollRate(0, SCROLL_UNIT);
}
