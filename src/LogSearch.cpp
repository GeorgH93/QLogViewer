#include "LogSearch.h"

#include "LogHolder.h"
#include "Profiler.hpp"
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

LogSearch::LogSearch(const LogHolder* logHolderr, QPlainTextEdit* resultTextBox)
{
	logHolder = logHolderr;
	resultText = resultTextBox;
}

void LogSearch::search(const QString& tokens, bool regex)
{
	if (tokens == nullptr || tokens.isEmpty())
	{
		qDebug() << "Empty search, clearing results...";
		resultText->setPlainText(nullptr);
		return;
	}

	if (tokens.length() <= SEARCH_LIMIT)
	{
		qDebug() << "Search length too short, not filling results yet...";
		return;
	}

	BlockProfiler profiler("Update search results");
	QString string;

	{
		BlockProfiler buildProfiler("Filter entries");
		for (const LogEntry* entry : logHolder->GetFilteredEntries())
		{
			if (!entry->originalMessage.contains(tokens))
			{
				continue;
			}

			if (!string.isEmpty())
			{
				string.append('\n');
			}
			string.append(entry->message);
		}
	}

	{
		BlockProfiler setProfiler("Set search results");
		resultText->setPlainText(string);
	}
}
