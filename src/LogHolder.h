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

#pragma once

#include "LogParser.h"
#include "Profiler.hpp"
#include <QString>
#include <QFile>

class LogHolder final
{
    std::vector<LogEntry> logEntries;
    std::vector<const LogEntry*> filteredLogEntries;
    QString systemInfo;

public:
    LogHolder() = default;

    LogHolder(const std::string& filePath)
    {
        Load(filePath);
    }

    LogHolder(QFile* file)
    {
        Load(file);
    }

    ~LogHolder() = default;

    void Load(const std::string filePath)
    {
        QFile f(QString(filePath.c_str()));
        Load(&f);
    }

    void Load(QFile* file)
    {
        BlockProfiler parseProfiler("Parse log");
        LogParser parser(file);
        logEntries = parser.Parse();
        systemInfo = parser.GetSystemInfo();
    }

    void Load(QString* log)
    {
        BlockProfiler parseProfiler("Parse log");
        LogParser parser(log);
        logEntries = parser.Parse();
        systemInfo = parser.GetSystemInfo();
    }

    void Filter(const std::function<bool(const LogEntry&)>& filterFunction)
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

    size_t GetFilteredLineCount() const
    {
        //TODO handle multi line messages
        return filteredLogEntries.size();
    }

    const QString& GetFilteredLineNumber(int editorLineNumber) const
    {
        //TODO handle multi line messages
        if (editorLineNumber >= filteredLogEntries.size()) return "";
        return filteredLogEntries[editorLineNumber]->lineNumberString;
    }

    const QString& GetFilteredEntryNumber(int editorLineNumber) const
    {
        //TODO handle multi line messages
        if (editorLineNumber >= filteredLogEntries.size()) return "";
        return filteredLogEntries[editorLineNumber]->entryNumberString;
    }

    inline uint64_t GetMaxLineNumber()
    {
        return logEntries.back().entryNumber;
    }

    inline const std::vector<const LogEntry*>& GetFilteredEntries()
    {
        return filteredLogEntries;
    }

    inline const QString& GetSystemInfo()
    {
	    return systemInfo;
    }
};