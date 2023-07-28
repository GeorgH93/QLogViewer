/*
 *   Copyright (C) 2023 GeorgH93
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "LogHolder.h"
#include "LogParser.h"
#include "Profiler.hpp"

void LogHolder::Load(LogParser &parser)
{
	{
		BlockProfiler parseProfiler("Parse log");
		logEntries = parser.Parse();
		systemInfo = parser.GetSystemInfo();
		logProfile = parser.GetUsedProfile();
		usedLogProfiles = parser.GetUsedLogLevels();
	}
	PreprocessLogEntries();
}

void LogHolder::PreprocessLogEntries()
{
	if (logEntries.empty()) return;
	const LogEntry* first = &logEntries[0];
	const LogEntry* previous = &logEntries[0];
	for (auto& logEntry : logEntries)
	{
		LogEntry* current = &logEntry;
		if (current->timeStamp.isValid() && first->timeStamp.isValid())
		{
			current->sinceStart = std::chrono::microseconds(first->timeStamp.msecsTo(current->timeStamp) * 1000);
		}
		if (current->timeStamp.isValid() && previous->timeStamp.isValid())
		{
			current->sincePrevious = std::chrono::microseconds(previous->timeStamp.msecsTo(current->timeStamp) * 1000);
		}
		previous = current;
	}
}

void LogHolder::Filter(const std::function<bool(const LogEntry &)> &filterFunction)
{
	BlockProfiler parseProfiler("Filter log");
	std::vector<const LogEntry*> newlyFilteredLog;
	newlyFilteredLog.reserve(logEntries.size());

	for (const LogEntry& entry : logEntries)
	{
		if (filterFunction(entry))
		{
			newlyFilteredLog.push_back(&entry);
		}
	}

	filteredLogEntries = std::move(newlyFilteredLog);
}

void LogHolder::Load(QFile *file)
{
	LogParser parser(file);
	Load(parser);
}

void LogHolder::Load(const QString &log)
{
	LogParser parser(log);
	Load(parser);
}

void LogHolder::Load(const std::string &filePath)
{
	LogParser parser(filePath);
	Load(parser);
}

std::vector<const LogEntry*> LogHolder::Find(const std::function<bool(const LogEntry&)>& searchFilter) const
{
	std::vector<const LogEntry*> result;
	for(const auto& entry : logEntries)
	{
		if (searchFilter(entry))
		{
			result.push_back(&entry);
		}
	}
	return result;
}

std::vector<const LogEntry*> LogHolder::FindFiltered(const std::function<bool(const LogEntry&)>& searchFilter) const
{
	std::vector<const LogEntry*> result;
	for(const auto& entry : filteredLogEntries)
	{
		if (searchFilter(*entry))
		{
			result.push_back(entry);
		}
	}
	return result;
}
