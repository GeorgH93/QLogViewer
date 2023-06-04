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

#include "EditInfoAreaWidget.h"
#include "LogHolder.h"
#include "LogLevel.h"

class LogLevelAreaWidget : public EditInfoAreaWidget
{
	const LogLevel fallbackLogLevel;
	const std::vector<const LogEntry*>* logEntries;
public:
	LogLevelAreaWidget(InfoAreaEnabledPlainTextEdit* editor, int marginLeft = 5, int marginRight = 5)
        : EditInfoAreaWidget(editor, [this](auto && lnr) { return GetMetaDescriptionForLine(std::forward<decltype(lnr)>(lnr)); }, marginLeft, marginRight)
		, logEntries(nullptr)
    {
        SetBackgroundColor(Qt::lightGray);
    }

    char GetWidthCalculationChar() override
    {
        return '#';
    }

	void SetLogHolder(const std::vector<const LogEntry*>* logEntries, const std::vector<std::shared_ptr<LogLevel>>& usedLevels)
	{
		this->logEntries = logEntries;
		int maxChars = 0;
		for(const auto& level : usedLevels)
		{
			maxChars = std::max(maxChars, level->GetLevelName().length());
		}
		SetWidthForCharCount(maxChars);
	}

    EditInfoAreaWidgetMetaDescription GetMetaDescriptionForLine(int lineNr)
    {
		if (!logEntries || logEntries->size() <= lineNr)
		{
			return {
				fallbackLogLevel.GetLevelName(),
				fallbackLogLevel.GetFontColor(),
				fallbackLogLevel.GetBackgroundColor(),
				fallbackLogLevel.GetAlignment()
				};
		}
        LogLevel* level = (*logEntries)[lineNr]->level.get();
		return {
			level->GetLevelName(),
			level->GetFontColor(),
			level->GetBackgroundColor(),
			level->GetAlignment()
			};
    }
};