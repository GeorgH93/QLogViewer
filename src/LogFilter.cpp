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

#include "LogFilter.h"

void LogFilter::SetSearchText(const QString& text)
{
	searchText = text;
	UpdateRegex();
}

void LogFilter::UpdateRegex()
{
	if (searchText.isEmpty())
	{
		searchRegex = QRegularExpression();
		return;
	}

	QString pattern;
	auto regexOptions = caseSensitive ? QRegularExpression::NoPatternOption
	                                  : QRegularExpression::CaseInsensitiveOption;

	switch (textMatchMode)
	{
		case TextMatchMode::Contains:
			pattern = QRegularExpression::escape(searchText);
			break;
		case TextMatchMode::StartsWith:
			pattern = "^" + QRegularExpression::escape(searchText);
			break;
		case TextMatchMode::EndsWith:
			pattern = QRegularExpression::escape(searchText) + "$";
			break;
		case TextMatchMode::ExactMatch:
			pattern = "^" + QRegularExpression::escape(searchText) + "$";
			break;
		case TextMatchMode::Regex:
			pattern = searchText;
			// For user-provided regex, ensure case insensitive if needed
			// (already set in regexOptions initialization above)
			break;
	}

	searchRegex = QRegularExpression(pattern, regexOptions);
}

bool LogFilter::MatchesLogLevel(const LogEntry& logEntry) const
{
	if (!logEntry.level)
	{
		// If no level info, check if we allow empty levels
		return includedLevels.isEmpty() || includedLevels.contains("");
	}

	const QString& levelName = logEntry.level->GetLevelName();

	// Check exclusions first
	if (excludedLevels.contains(levelName))
	{
		return false;
	}

	// If inclusion list is empty, all non-excluded levels are allowed
	if (includedLevels.isEmpty())
	{
		return true;
	}

	return includedLevels.contains(levelName);
}

bool LogFilter::MatchesText(const LogEntry& logEntry) const
{
	if (searchText.isEmpty() || !searchRegex.isValid())
	{
		return true;  // No text filter = match everything
	}

	// Search in the message and original message
	const QString& message = logEntry.components[LogComponent::MESSAGE];
	const QString& original = logEntry.components[LogComponent::ORIGINAL_MESSAGE];

	return searchRegex.match(message).hasMatch() || searchRegex.match(original).hasMatch();
}

bool LogFilter::ShowEntry(const LogEntry& logEntry) const
{
	if (!enabled)
	{
		return true;  // Disabled filter shows everything
	}

	const bool matchesLevel = MatchesLogLevel(logEntry);
	const bool matchesText = MatchesText(logEntry);
	const bool matches = matchesLevel && matchesText;

	// In Include mode: show only matching entries
	// In Exclude mode: hide matching entries
	return (filterMode == FilterMode::Include) ? matches : !matches;
}

std::function<bool(const LogEntry&)> LogFilter::CreateFilterFunction() const
{
	return [this](const LogEntry& entry) { return ShowEntry(entry); };
}
