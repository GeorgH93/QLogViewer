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
#include "LogHolder.h"
#include "LogEntry.h"

// Full-format lines that match the default regex
static const QString FULL_INFO1  = "23-01-15 10:00:00.000 INFO first in a() function at line 1\n";
static const QString FULL_INFO2  = "23-01-15 10:00:02.000 INFO third in c() function at line 3\n";
static const QString FULL_DEBUG1 = "23-01-15 10:00:01.000 DEBUG second in b() function at line 2\n";
static const QString FULL_ERROR1 = "23-01-15 10:00:03.000 ERROR fourth in d() function at line 4\n";

// ---------------------------------------------------------------------------
// Load from QString
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: loading empty string produces no filtered entries after filter", "[LogHolder]")
{
	LogHolder holder;
	holder.Load(QString(""));
	holder.Filter([](const LogEntry&) { return true; });
	CHECK(holder.GetFilteredLineCount() == 0);
}

TEST_CASE("LogHolder: loading three log lines gives three entries after pass-all filter", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1 + FULL_ERROR1;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry&) { return true; });
	CHECK(holder.GetFilteredLineCount() == 3);
}

// ---------------------------------------------------------------------------
// Filter
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: filter that rejects everything yields 0 filtered entries", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry&) { return false; });
	CHECK(holder.GetFilteredLineCount() == 0);
}

TEST_CASE("LogHolder: filter by log level name keeps matching entries", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1 + FULL_INFO2;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry& e) {
		return e.level && e.level->GetLevelName() == "INFO";
	});
	CHECK(holder.GetFilteredLineCount() == 2);
}

TEST_CASE("LogHolder: filter can be applied multiple times, replacing previous result", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1;
	LogHolder holder;
	holder.Load(log);

	holder.Filter([](const LogEntry&) { return true; });
	CHECK(holder.GetFilteredLineCount() == 2);

	holder.Filter([](const LogEntry&) { return false; });
	CHECK(holder.GetFilteredLineCount() == 0);
}

// ---------------------------------------------------------------------------
// GetFilteredLineCount / GetMaxLineNumber
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: GetMaxLineNumber returns 0 for empty holder", "[LogHolder]")
{
	LogHolder holder;
	CHECK(holder.GetMaxLineNumber() == 0);
}

TEST_CASE("LogHolder: GetMaxLineNumber returns last entry number after load", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1;
	LogHolder holder;
	holder.Load(log);
	CHECK(holder.GetMaxLineNumber() == 2);
}

// ---------------------------------------------------------------------------
// GetFilteredEntryNumber / GetFilteredLineNumber (bounds)
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: GetFilteredLineNumber returns empty for out-of-range index", "[LogHolder]")
{
	LogHolder holder;
	holder.Load(FULL_INFO1);
	holder.Filter([](const LogEntry&) { return true; });
	CHECK(holder.GetFilteredLineNumber(999).isEmpty());
}

TEST_CASE("LogHolder: GetFilteredEntryNumber returns empty for out-of-range index", "[LogHolder]")
{
	LogHolder holder;
	holder.Load(FULL_INFO1);
	holder.Filter([](const LogEntry&) { return true; });
	CHECK(holder.GetFilteredEntryNumber(999).isEmpty());
}

TEST_CASE("LogHolder: GetFilteredEntryNumber index 0 returns non-empty", "[LogHolder]")
{
	LogHolder holder;
	holder.Load(FULL_INFO1);
	holder.Filter([](const LogEntry&) { return true; });
	REQUIRE(holder.GetFilteredLineCount() >= 1);
	CHECK(!holder.GetFilteredEntryNumber(0).isEmpty());
}

// ---------------------------------------------------------------------------
// Find / FindFiltered
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: Find searches all entries regardless of filter", "[LogHolder]")
{
	// INFO+INFO+DEBUG; filter only DEBUG; Find for INFO should still get 2
	const QString log = FULL_INFO1 + FULL_INFO2 + FULL_DEBUG1;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry& e) {
		return e.level && e.level->GetLevelName() == "DEBUG";
	});
	CHECK(holder.GetFilteredLineCount() == 1);

	auto results = holder.Find([](const LogEntry& e) {
		return e.level && e.level->GetLevelName() == "INFO";
	});
	CHECK(results.size() == 2);
}

TEST_CASE("LogHolder: FindFiltered searches only filtered entries", "[LogHolder]")
{
	// INFO + DEBUG + INFO; filter only INFO; FindFiltered for INFO should get 2
	const QString log = FULL_INFO1 + FULL_DEBUG1 + FULL_INFO2;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry& e) {
		return e.level && e.level->GetLevelName() == "INFO";
	});
	auto results = holder.FindFiltered([](const LogEntry& e) {
		return e.level && e.level->GetLevelName() == "INFO";
	});
	CHECK(results.size() == 2);
}

// ---------------------------------------------------------------------------
// GetFilteredEntries is empty before Filter() is called
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: GetFilteredEntries is empty before filter is called", "[LogHolder]")
{
	LogHolder holder;
	holder.Load(FULL_INFO1);
	CHECK(holder.GetFilteredEntries().empty());
}

// ---------------------------------------------------------------------------
// System info
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: GetSystemInfo returns info extracted from log", "[LogHolder]")
{
	const QString log = "Version: 4.0.0\n" + FULL_INFO1;
	LogHolder holder;
	holder.Load(log);
	CHECK(holder.GetSystemInfo().contains("4.0.0"));
}

// ---------------------------------------------------------------------------
// PreprocessLogEntries: sinceStart/sincePrevious are computed for valid timestamps
// ---------------------------------------------------------------------------
TEST_CASE("LogHolder: sinceStart is zero for the first entry", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry&) { return true; });
	REQUIRE(holder.GetFilteredLineCount() == 2);
	const auto& entries = holder.GetFilteredEntries();
	CHECK(entries[0]->sinceStart.count() == 0);
}

TEST_CASE("LogHolder: sincePrevious is positive for second entry when timestamps differ", "[LogHolder]")
{
	const QString log = FULL_INFO1 + FULL_DEBUG1;
	LogHolder holder;
	holder.Load(log);
	holder.Filter([](const LogEntry&) { return true; });
	REQUIRE(holder.GetFilteredLineCount() == 2);
	const auto& entries = holder.GetFilteredEntries();
	if (entries[1]->timeStamp.isValid() && entries[0]->timeStamp.isValid())
	{
		CHECK(entries[1]->sincePrevious.count() > 0);
	}
}
