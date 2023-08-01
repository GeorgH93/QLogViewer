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
#include <QStringList>
#include <QRegularExpression>
#include <functional>

class LogFilter final
{
public:
	enum class FilterMode
	{
		Include,  // Show only entries that match
		Exclude   // Hide entries that match
	};

	enum class TextMatchMode
	{
		Contains,      // Simple substring match
		StartsWith,    // Message starts with text
		EndsWith,      // Message ends with text
		ExactMatch,    // Exact match
		Regex          // Regular expression match
	};

private:
	QString filterName;

	// Log level filtering
	QStringList includedLevels;   // Empty means all levels allowed
	QStringList excludedLevels;

	// Text filtering
	QString searchText;
	TextMatchMode textMatchMode = TextMatchMode::Contains;
	bool caseSensitive = false;
	QRegularExpression searchRegex;

	// General filter mode
	FilterMode filterMode = FilterMode::Include;

	// Filter enabled state
	bool enabled = true;

public:
	LogFilter() = default;
	explicit LogFilter(const QString& name) : filterName(name) {}
	~LogFilter() = default;

	// Main filtering function
	[[nodiscard]] bool ShowEntry(const LogEntry& logEntry) const;

	// Create a filter function for use with LogHolder::Filter
	[[nodiscard]] std::function<bool(const LogEntry&)> CreateFilterFunction() const;

	// Filter name
	void SetFilterName(const QString& name) { filterName = name; }
	[[nodiscard]] const QString& GetFilterName() const { return filterName; }

	// Log level filtering
	void SetIncludedLevels(const QStringList& levels) { includedLevels = levels; }
	void SetExcludedLevels(const QStringList& levels) { excludedLevels = levels; }
	void AddIncludedLevel(const QString& level) { if (!includedLevels.contains(level)) includedLevels.append(level); }
	void AddExcludedLevel(const QString& level) { if (!excludedLevels.contains(level)) excludedLevels.append(level); }
	void ClearIncludedLevels() { includedLevels.clear(); }
	void ClearExcludedLevels() { excludedLevels.clear(); }
	[[nodiscard]] const QStringList& GetIncludedLevels() const { return includedLevels; }
	[[nodiscard]] const QStringList& GetExcludedLevels() const { return excludedLevels; }

	// Text filtering
	void SetSearchText(const QString& text);
	void SetTextMatchMode(TextMatchMode mode) { textMatchMode = mode; UpdateRegex(); }
	void SetCaseSensitive(bool sensitive) { caseSensitive = sensitive; UpdateRegex(); }
	[[nodiscard]] const QString& GetSearchText() const { return searchText; }
	[[nodiscard]] TextMatchMode GetTextMatchMode() const { return textMatchMode; }
	[[nodiscard]] bool IsCaseSensitive() const { return caseSensitive; }

	// Filter mode
	void SetFilterMode(FilterMode mode) { filterMode = mode; }
	[[nodiscard]] FilterMode GetFilterMode() const { return filterMode; }

	// Enabled state
	void SetEnabled(bool enable) { enabled = enable; }
	[[nodiscard]] bool IsEnabled() const { return enabled; }

private:
	void UpdateRegex();
	[[nodiscard]] bool MatchesLogLevel(const LogEntry& logEntry) const;
	[[nodiscard]] bool MatchesText(const LogEntry& logEntry) const;
};
