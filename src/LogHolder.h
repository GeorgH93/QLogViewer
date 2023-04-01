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
    static constexpr QStringView EMPTY_MESSAGE = u"";

    std::vector<LogEntry> logEntries;
    std::vector<const LogEntry*> filteredLogEntries;
    QString systemInfo;

public:
    LogHolder() = default;

    LogHolder(const std::string& filePath)
    {
        Load(filePath);
    }

    explicit LogHolder(QFile* file)
    {
        Load(file);
    }

    ~LogHolder() = default;

    void Load(const std::string& filePath)
    {
        QFile f(QString(filePath.c_str()));
        Load(&f);
    }

    void Load(QFile* file)
    {
        LogParser parser(file);
        Load(parser);
    }

    void Load(QString* log)
    {
        LogParser parser(log);
        Load(parser);
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

    [[nodiscard]] size_t GetFilteredLineCount() const
    {
        //TODO handle multi line messages
        return filteredLogEntries.size();
    }

    [[nodiscard]] QStringView GetFilteredLineNumber(int editorLineNumber) const
    {
        //TODO handle multi line messages
        if (editorLineNumber >= filteredLogEntries.size()) return EMPTY_MESSAGE;
        return filteredLogEntries[editorLineNumber]->lineNumberString;
    }

    [[nodiscard]] QStringView GetFilteredEntryNumber(int editorLineNumber) const
    {
        //TODO handle multi line messages
        if (editorLineNumber >= filteredLogEntries.size()) return EMPTY_MESSAGE;
        return filteredLogEntries[editorLineNumber]->entryNumberString;
    }

    [[nodiscard]] inline uint64_t GetMaxLineNumber() const
    {
        return logEntries.back().entryNumber;
    }

    [[nodiscard]] inline const std::vector<const LogEntry*>& GetFilteredEntries() const
    {
        return filteredLogEntries;
    }

    [[nodiscard]] inline const QString& GetSystemInfo() const
    {
	    return systemInfo;
    }

private:
    void Load(LogParser& parser)
    {
        {
            BlockProfiler parseProfiler("Parse log");
            logEntries = parser.Parse();
            systemInfo = parser.GetSystemInfo();
        }
        PreprocessLogEntries();
    }

    void PreprocessLogEntries()
    {
        if (logEntries.empty()) return;
        const LogEntry* first = &logEntries[0];
        const LogEntry* previous = &logEntries[0];
        LogEntry* current = nullptr;
        for (auto& logEntry : logEntries)
        {
            current = &logEntry;
            logEntry.Process();
            previous = current;
        }
    }
};