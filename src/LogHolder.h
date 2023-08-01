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
#include "FormatedStringCache.h"
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
	std::vector<std::shared_ptr<LogLevel>> usedLogProfiles;

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
        // Count total lines including multi-line entries
        size_t totalLines = 0;
        for (const auto* entry : filteredLogEntries)
        {
            totalLines += entry->lineCount;
        }
        return totalLines;
    }

    [[nodiscard]] QStringView GetFilteredLineNumber(size_t editorLineNumber) const
    {
        // Find the entry that contains the given editor line number
        size_t currentLine = 0;
        for (const auto* entry : filteredLogEntries)
        {
            if (editorLineNumber >= currentLine && editorLineNumber < currentLine + entry->lineCount)
            {
                return FormattedStringCache::NumberAsString(entry->lineNumber);
            }
            currentLine += entry->lineCount;
        }
        return EMPTY_MESSAGE;
    }

    [[nodiscard]] QStringView GetFilteredEntryNumber(size_t editorLineNumber) const
    {
        // Find the entry that contains the given editor line number
        size_t currentLine = 0;
        for (const auto* entry : filteredLogEntries)
        {
            if (editorLineNumber >= currentLine && editorLineNumber < currentLine + entry->lineCount)
            {
                return FormattedStringCache::NumberAsString(entry->entryNumber);
            }
            currentLine += entry->lineCount;
        }
        return EMPTY_MESSAGE;
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

	[[nodiscard]] inline const std::vector<std::shared_ptr<LogLevel>>& GetUsedLogLevels() const
	{
		return usedLogProfiles;
	}

	[[nodiscard]] inline std::shared_ptr<LogProfile> GetLogProfile() const { return logProfile; }

	[[nodiscard]] std::vector<const LogEntry*> Find(const std::function<bool(const LogEntry&)>& searchFilter) const;

	[[nodiscard]] std::vector<const LogEntry*> FindFiltered(const std::function<bool(const LogEntry&)>& searchFilter) const;

private:
    void Load(LogParser& parser);

    void PreprocessLogEntries();
};