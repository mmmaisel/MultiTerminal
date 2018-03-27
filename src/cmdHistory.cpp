/**********************************************************************\
 * MultiTerminal
 * cmdHistory.cpp
 *
 * Command Line Input History
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
#include "cmdHistory.h"

std::string cmdHistory::m_emptyStr;

cmdHistory::cmdHistory(size_t maxSize)
	: m_beforeFirst(false)
	, m_maxSize(maxSize)
{
}

cmdHistory::cmdHistory(cmdHistory&& rhs)
	: m_beforeFirst(rhs.m_beforeFirst)
	, m_maxSize(rhs.m_maxSize)
	, m_history(rhs.m_history)
{
	rhs.m_beforeFirst = false;
	rhs.m_maxSize = 0;

	ptrdiff_t pos = std::distance(rhs.m_history.begin(), rhs.m_itCurrent);
	m_itCurrent = m_history.begin();
	std::advance(m_itCurrent, pos);
}

void cmdHistory::Clear()
{
	m_history.clear();
	m_itCurrent = m_history.begin();
}

void cmdHistory::Next()
{
	// increment iterator if the end is not reached
	// (--end(): end() points one element past the end)
	if(m_history.size() == 0)
		return;
	else if(m_beforeFirst)
		m_beforeFirst = false;
	else if(m_itCurrent != --m_history.end())
		++m_itCurrent;
}

void cmdHistory::Previous()
{
	// decrement iterator if the beginning is not reached
	if(m_history.size() == 0)
		return;
	else if(m_itCurrent != m_history.begin())
	{
		--m_itCurrent;
		m_beforeFirst = false;
	}
	else
		m_beforeFirst = true;
}

void cmdHistory::SetMaxSize(size_t maxSize)
{
	// Set new maximum element count and
	// remove surplus entries.
	m_maxSize = maxSize;
	RemoveOldEntries();
}

void cmdHistory::Insert(std::string& str)
{
	// Insert new element and set iterator back to beginning
	// (you want to get the last input when pressing 'UP')
	// and remove last entry if list is full.
	m_history.push_front(str);
	m_itCurrent = m_history.begin();
	m_beforeFirst = false;
	RemoveOldEntries();
}

void cmdHistory::Insert(std::string&& str)
{
	// description: see above
	m_history.push_front(str);
	m_itCurrent = m_history.begin();
	m_beforeFirst = false;
	RemoveOldEntries();
}

void cmdHistory::RemoveOldEntries()
{
	// If list is full: shrink it
	// (do not resize if the list is not full
	// because empty strings would be inserted)
	if(m_history.size() > m_maxSize)
		m_history.resize(m_maxSize);
}
