/**********************************************************************\
 * MultiTerminal
 * cmdHistory.h
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
#pragma once

class cmdHistory
{
	public:
		cmdHistory(size_t maxSize = 10);
		cmdHistory(const cmdHistory&) = delete;
		cmdHistory(cmdHistory&&);
		~cmdHistory() {};

		// element access
		inline const std::string& GetCurrent() const
			{return m_beforeFirst || !m_history.size() ? m_emptyStr : *m_itCurrent;}
		inline size_t GetMaxSize() const {return m_maxSize;}
		void Clear();

		// iteration
		void Next();
		void Previous();
		inline void First() {m_itCurrent = m_history.begin();}

		// insert and capacity
		void SetMaxSize(size_t maxSize);
		void Insert(std::string& str);
		void Insert(std::string&& str);
		inline size_t size() const {return m_history.size();}

	private:
		void RemoveOldEntries();

		bool m_beforeFirst;
		size_t m_maxSize;
		static std::string m_emptyStr;
		std::list<std::string> m_history;
		std::list<std::string>::iterator m_itCurrent;
};
