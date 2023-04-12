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

#include "LogEntry.h"
#include <QString>
#include <QFile>

class LogParser;
class LogProfile;

class LogHolder final
{
    static constexpr QStringView EMPTY_MESSAGE = u"";

    std::vector<LogEntry> logEntries;
    std::vector<const LogEntry*> filteredLogEntries;
    QString systemInfo;
	std::shared_ptr<LogProfile> logProfile;

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

    void Load(const std::string& filePath);

    void Load(QFile* file);

    void Load(const QString& log);

    void Filter(const std::function<bool(const LogEntry&)>& filterFunction);

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
		if (logEntries.empty()) return 0;
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

	[[nodiscard]] inline std::shared_ptr<LogProfile> GetLogProfile() const { return logProfile; }

private:
    void Load(LogParser& parser);

    void PreprocessLogEntries();
};