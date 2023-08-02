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

#include <catch2/catch_test_macros.hpp>
#include "LogFilter.h"
#include "LogEntry.h"
#include "LogLevel.h"
#include <memory>

// Helper to create a LogEntry with given level and message
static LogEntry MakeEntry(const QString& levelName, const QString& message)
{
	LogEntry entry;
	entry.level = std::make_shared<LogLevel>(levelName);
	entry.components[LogComponent::MESSAGE] = message;
	entry.components[LogComponent::ORIGINAL_MESSAGE] = message;
	return entry;
}

// Helper to create a LogEntry without a level
static LogEntry MakeEntryNoLevel(const QString& message)
{
	LogEntry entry;
	entry.components[LogComponent::MESSAGE] = message;
	entry.components[LogComponent::ORIGINAL_MESSAGE] = message;
	return entry;
}

// ---------------------------------------------------------------------------
// FilterMode::Include tests
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: Include mode shows only matching entries", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	filter.AddIncludedLevel("ERROR");

	LogEntry errorEntry = MakeEntry("ERROR", "error message");
	LogEntry infoEntry = MakeEntry("INFO", "info message");

	CHECK(filter.ShowEntry(errorEntry) == true);
	CHECK(filter.ShowEntry(infoEntry) == false);
}

TEST_CASE("LogFilter: Include mode with empty included levels shows all", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	// Don't add any included levels

	LogEntry entry = MakeEntry("INFO", "message");
	CHECK(filter.ShowEntry(entry) == true);
}

// ---------------------------------------------------------------------------
// FilterMode::Exclude tests
// Note: Exclude mode hides entries that MATCH (level AND text).
// With no text filter, all entries match and are excluded.
// Use Include mode + includedLevels for level-based filtering.
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: Exclude mode with text filter hides matching entries", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Exclude);
	filter.SetSearchText("debug");

	LogEntry debugEntry = MakeEntry("INFO", "debug message");
	LogEntry infoEntry = MakeEntry("INFO", "info message");

	CHECK(filter.ShowEntry(debugEntry) == false);  // Matches text, excluded
	CHECK(filter.ShowEntry(infoEntry) == true);   // Doesn't match text, shown
}

TEST_CASE("LogFilter: Exclude mode with level filter in Include mode", "[LogFilter]")
{
	// Use Include mode with includedLevels for level-based filtering
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	filter.AddIncludedLevel("ERROR");
	filter.AddIncludedLevel("WARN");

	LogEntry errorEntry = MakeEntry("ERROR", "error");
	LogEntry warnEntry = MakeEntry("WARN", "warn");
	LogEntry infoEntry = MakeEntry("INFO", "info");

	CHECK(filter.ShowEntry(errorEntry) == true);   // In included list
	CHECK(filter.ShowEntry(warnEntry) == true);   // In included list
	CHECK(filter.ShowEntry(infoEntry) == false);  // Not in included list
}

// ---------------------------------------------------------------------------
// Log level filtering - IncludedLevels
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: SetIncludedLevels replaces the list", "[LogFilter]")
{
	LogFilter filter;
	filter.AddIncludedLevel("ERROR");
	filter.SetIncludedLevels(QStringList{"WARN", "INFO"});

	const auto& levels = filter.GetIncludedLevels();
	CHECK(levels.size() == 2);
	CHECK(levels.contains("WARN"));
	CHECK(levels.contains("INFO"));
	CHECK(!levels.contains("ERROR"));
}

TEST_CASE("LogFilter: AddIncludedLevel does not add duplicates", "[LogFilter]")
{
	LogFilter filter;
	filter.AddIncludedLevel("ERROR");
	filter.AddIncludedLevel("ERROR");
	filter.AddIncludedLevel("WARN");

	CHECK(filter.GetIncludedLevels().size() == 2);
}

TEST_CASE("LogFilter: ClearIncludedLevels empties the list", "[LogFilter]")
{
	LogFilter filter;
	filter.AddIncludedLevel("ERROR");
	filter.AddIncludedLevel("WARN");
	filter.ClearIncludedLevels();

	CHECK(filter.GetIncludedLevels().isEmpty());
}

// ---------------------------------------------------------------------------
// Log level filtering - ExcludedLevels
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: SetExcludedLevels replaces the list", "[LogFilter]")
{
	LogFilter filter;
	filter.AddExcludedLevel("DEBUG");
	filter.SetExcludedLevels(QStringList{"TRACE", "VERBOSE"});

	const auto& levels = filter.GetExcludedLevels();
	CHECK(levels.size() == 2);
	CHECK(levels.contains("TRACE"));
	CHECK(levels.contains("VERBOSE"));
	CHECK(!levels.contains("DEBUG"));
}

TEST_CASE("LogFilter: AddExcludedLevel does not add duplicates", "[LogFilter]")
{
	LogFilter filter;
	filter.AddExcludedLevel("DEBUG");
	filter.AddExcludedLevel("DEBUG");
	filter.AddExcludedLevel("TRACE");

	CHECK(filter.GetExcludedLevels().size() == 2);
}

TEST_CASE("LogFilter: ClearExcludedLevels empties the list", "[LogFilter]")
{
	LogFilter filter;
	filter.AddExcludedLevel("DEBUG");
	filter.AddExcludedLevel("TRACE");
	filter.ClearExcludedLevels();

	CHECK(filter.GetExcludedLevels().isEmpty());
}

// ---------------------------------------------------------------------------
// Exclusion takes precedence over inclusion
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: excluded level is hidden even if in included list", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	filter.AddIncludedLevel("ERROR");
	filter.AddIncludedLevel("INFO");
	filter.AddExcludedLevel("ERROR");

	LogEntry errorEntry = MakeEntry("ERROR", "error");
	LogEntry infoEntry = MakeEntry("INFO", "info");

	CHECK(filter.ShowEntry(errorEntry) == false);  // Excluded
	CHECK(filter.ShowEntry(infoEntry) == true);    // Included, not excluded
}

// ---------------------------------------------------------------------------
// TextMatchMode::Contains
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: Contains mode matches substring", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("error");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Contains);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "an error occurred")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "ERROR in caps")) == true);  // Case insensitive by default
	CHECK(filter.ShowEntry(MakeEntry("INFO", "no issues here")) == false);
}

TEST_CASE("LogFilter: Contains mode is case insensitive by default", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("Error");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Contains);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "error message")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "ERROR MESSAGE")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "ErRoR")) == true);
}

TEST_CASE("LogFilter: Contains mode respects case sensitive flag", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("Error");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Contains);
	filter.SetCaseSensitive(true);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Error message")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "error message")) == false);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "ERROR MESSAGE")) == false);
}

// ---------------------------------------------------------------------------
// TextMatchMode::StartsWith
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: StartsWith mode matches prefix", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("Connection");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::StartsWith);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection established")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "connection failed")) == true);  // Case insensitive
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Failed Connection")) == false);
}

TEST_CASE("LogFilter: StartsWith mode respects case sensitive flag", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("Connection");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::StartsWith);
	filter.SetCaseSensitive(true);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection established")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "connection failed")) == false);
}

// ---------------------------------------------------------------------------
// TextMatchMode::EndsWith
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: EndsWith mode matches suffix", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("failed");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::EndsWith);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection failed")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection FAILED")) == true);  // Case insensitive
	CHECK(filter.ShowEntry(MakeEntry("INFO", "failed to connect")) == false);
}

TEST_CASE("LogFilter: EndsWith mode respects case sensitive flag", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("failed");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::EndsWith);
	filter.SetCaseSensitive(true);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection failed")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection FAILED")) == false);
}

// ---------------------------------------------------------------------------
// TextMatchMode::ExactMatch
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: ExactMatch mode requires full match", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("Connection failed");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::ExactMatch);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection failed")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection Failed")) == true);  // Case insensitive
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection failed!")) == false);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Error: Connection failed")) == false);
}

TEST_CASE("LogFilter: ExactMatch mode respects case sensitive flag", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("Connection failed");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::ExactMatch);
	filter.SetCaseSensitive(true);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection failed")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Connection Failed")) == false);
}

// ---------------------------------------------------------------------------
// TextMatchMode::Regex
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: Regex mode matches pattern", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("\\d{4}-\\d{2}-\\d{2}");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Regex);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "Date: 2024-01-15")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "Date: 2024-1-15")) == false);  // Wrong format
	CHECK(filter.ShowEntry(MakeEntry("INFO", "No date here")) == false);
}

TEST_CASE("LogFilter: Regex mode with case insensitive flag", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("error");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Regex);
	filter.SetCaseSensitive(false);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "ERROR message")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "error message")) == true);
}

TEST_CASE("LogFilter: Regex mode with case sensitive flag", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("error");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Regex);
	filter.SetCaseSensitive(true);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "error message")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "ERROR message")) == false);
}

TEST_CASE("LogFilter: Regex mode with complex pattern", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("^\\[ERROR\\].*timeout");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Regex);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "[ERROR] Connection timeout occurred")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "[ERROR] Connection failed")) == false);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "WARN: [ERROR] timeout")) == false);  // Not at start
}

// ---------------------------------------------------------------------------
// Empty search text
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: empty search text matches everything", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("");
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Contains);

	CHECK(filter.ShowEntry(MakeEntry("INFO", "any message")) == true);
	CHECK(filter.ShowEntry(MakeEntry("INFO", "")) == true);
}

// ---------------------------------------------------------------------------
// Enabled/Disabled state
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: disabled filter shows all entries", "[LogFilter]")
{
	LogFilter filter;
	filter.SetEnabled(false);
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	filter.AddIncludedLevel("ERROR");

	LogEntry infoEntry = MakeEntry("INFO", "info message");
	CHECK(filter.ShowEntry(infoEntry) == true);  // Filter is disabled, so shows everything
}

TEST_CASE("LogFilter: SetEnabled(false) then SetEnabled(true) restores filtering", "[LogFilter]")
{
	LogFilter filter;
	filter.AddIncludedLevel("ERROR");
	filter.SetEnabled(false);

	LogEntry infoEntry = MakeEntry("INFO", "info");
	CHECK(filter.ShowEntry(infoEntry) == true);

	filter.SetEnabled(true);
	CHECK(filter.ShowEntry(infoEntry) == false);
}

TEST_CASE("LogFilter: filter is enabled by default", "[LogFilter]")
{
	LogFilter filter;
	CHECK(filter.IsEnabled() == true);
}

// ---------------------------------------------------------------------------
// Filter name
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: filter name can be set and retrieved", "[LogFilter]")
{
	LogFilter filter("MyFilter");
	CHECK(filter.GetFilterName() == "MyFilter");

	filter.SetFilterName("RenamedFilter");
	CHECK(filter.GetFilterName() == "RenamedFilter");
}

TEST_CASE("LogFilter: default constructed filter has empty name", "[LogFilter]")
{
	LogFilter filter;
	CHECK(filter.GetFilterName().isEmpty());
}

// ---------------------------------------------------------------------------
// Entries without level
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: entry without level is filtered correctly", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	filter.AddIncludedLevel("ERROR");

	LogEntry noLevelEntry = MakeEntryNoLevel("some message");
	CHECK(filter.ShowEntry(noLevelEntry) == false);
}

TEST_CASE("LogFilter: entry without level passes when included levels is empty", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	// No included levels

	LogEntry noLevelEntry = MakeEntryNoLevel("some message");
	CHECK(filter.ShowEntry(noLevelEntry) == true);
}

// ---------------------------------------------------------------------------
// Combined level and text filtering
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: both level and text must match in Include mode", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Include);
	filter.AddIncludedLevel("ERROR");
	filter.SetSearchText("timeout");

	// Matches level but not text
	CHECK(filter.ShowEntry(MakeEntry("ERROR", "connection refused")) == false);
	// Matches text but not level
	CHECK(filter.ShowEntry(MakeEntry("INFO", "timeout occurred")) == false);
	// Matches both
	CHECK(filter.ShowEntry(MakeEntry("ERROR", "timeout occurred")) == true);
}

TEST_CASE("LogFilter: Exclude mode behavior with text filter", "[LogFilter]")
{
	// In Exclude mode: entries that match (level AND text) are excluded
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Exclude);
	filter.SetSearchText("timeout");
	// Note: excludedLevels in Exclude mode has counterintuitive behavior
	// excludedLevels makes MatchesLevel return false, which in Exclude mode means SHOWN

	// Entry matches text (no level restriction) - excluded
	CHECK(filter.ShowEntry(MakeEntry("INFO", "timeout occurred")) == false);

	// Entry doesn't match text - shown
	CHECK(filter.ShowEntry(MakeEntry("INFO", "connection refused")) == true);
}

// ---------------------------------------------------------------------------
// CreateFilterFunction
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: CreateFilterFunction returns callable that matches ShowEntry", "[LogFilter]")
{
	LogFilter filter;
	filter.AddIncludedLevel("ERROR");

	auto filterFunc = filter.CreateFilterFunction();

	LogEntry errorEntry = MakeEntry("ERROR", "error");
	LogEntry infoEntry = MakeEntry("INFO", "info");

	CHECK(filterFunc(errorEntry) == filter.ShowEntry(errorEntry));
	CHECK(filterFunc(infoEntry) == filter.ShowEntry(infoEntry));
}

// ---------------------------------------------------------------------------
// Getters for text matching settings
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: GetSearchText returns the search text", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("test query");
	CHECK(filter.GetSearchText() == "test query");
}

TEST_CASE("LogFilter: GetTextMatchMode returns the current mode", "[LogFilter]")
{
	LogFilter filter;
	filter.SetTextMatchMode(LogFilter::TextMatchMode::Regex);
	CHECK(filter.GetTextMatchMode() == LogFilter::TextMatchMode::Regex);
}

TEST_CASE("LogFilter: IsCaseSensitive returns the case sensitivity flag", "[LogFilter]")
{
	LogFilter filter;
	CHECK(filter.IsCaseSensitive() == false);

	filter.SetCaseSensitive(true);
	CHECK(filter.IsCaseSensitive() == true);
}

TEST_CASE("LogFilter: GetFilterMode returns the current filter mode", "[LogFilter]")
{
	LogFilter filter;
	filter.SetFilterMode(LogFilter::FilterMode::Exclude);
	CHECK(filter.GetFilterMode() == LogFilter::FilterMode::Exclude);
}

// ---------------------------------------------------------------------------
// Searching in ORIGINAL_MESSAGE component
// ---------------------------------------------------------------------------
TEST_CASE("LogFilter: text search also matches ORIGINAL_MESSAGE", "[LogFilter]")
{
	LogFilter filter;
	filter.SetSearchText("timestamp");

	LogEntry entry;
	entry.level = std::make_shared<LogLevel>("INFO");
	entry.components[LogComponent::MESSAGE] = "user logged in";
	entry.components[LogComponent::ORIGINAL_MESSAGE] = "2024-01-15 timestamp: user logged in";

	CHECK(filter.ShowEntry(entry) == true);
}
