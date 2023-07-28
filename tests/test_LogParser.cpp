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
#include "LogParser.h"
#include "LogEntry.h"
#include "LogProfile.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::vector<LogEntry> ParseString(const QString& log)
{
	LogParser parser(log);
	return parser.Parse();
}

// A log line that fully satisfies the default regex (needs the "where" clause)
// Format: yy-mm-dd hh:mm:ss.mmm LEVEL message in func() function at line N
static const QString LINE_ERROR =
	"23-01-15 12:34:56.000 ERROR my error message in doWork() function at line 42\n";
static const QString LINE_INFO =
	"23-01-15 12:34:57.000 INFO  my info message in main() function at line 10\n";
static const QString LINE_DEBUG =
	"23-01-15 12:34:58.000 DEBUG debug stuff in setup() function at line 5\n";

// ---------------------------------------------------------------------------
// Basic parsing
// ---------------------------------------------------------------------------
TEST_CASE("LogParser: empty input yields no entries", "[LogParser]")
{
	auto entries = ParseString("");
	CHECK(entries.empty());
}

TEST_CASE("LogParser: single well-formed log line is parsed", "[LogParser]")
{
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	CHECK(!entries[0].components[LogComponent::MESSAGE].isEmpty());
}

TEST_CASE("LogParser: entry numbers are sequential starting at 1", "[LogParser]")
{
	const QString log = LINE_ERROR + LINE_INFO + LINE_DEBUG;
	auto entries = ParseString(log);
	REQUIRE(entries.size() == 3);
	CHECK(entries[0].entryNumber == 1);
	CHECK(entries[1].entryNumber == 2);
	CHECK(entries[2].entryNumber == 3);
}

TEST_CASE("LogParser: line numbers are tracked correctly", "[LogParser]")
{
	const QString log = LINE_ERROR + LINE_INFO;
	auto entries = ParseString(log);
	REQUIRE(entries.size() == 2);
	CHECK(entries[0].lineNumber == 1);
}

TEST_CASE("LogParser: original message is preserved", "[LogParser]")
{
	const QString trimmed = LINE_ERROR.trimmed();
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	CHECK(entries[0].components[LogComponent::ORIGINAL_MESSAGE] == trimmed);
}

TEST_CASE("LogParser: lines without match still produce an entry", "[LogParser]")
{
	const QString log = "this is just plain text without any date prefix";
	auto entries = ParseString(log);
	REQUIRE(entries.size() == 1);
	CHECK(entries[0].components[LogComponent::MESSAGE] == log.trimmed());
}

// ---------------------------------------------------------------------------
// Log level parsing
// ---------------------------------------------------------------------------
TEST_CASE("LogParser: log level is extracted from a fully-matching line", "[LogParser]")
{
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].level != nullptr);
	CHECK(entries[0].level->GetLevelName() == "ERROR");
}

TEST_CASE("LogParser: INFO level is extracted correctly", "[LogParser]")
{
	auto entries = ParseString(LINE_INFO);
	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].level != nullptr);
	CHECK(entries[0].level->GetLevelName() == "INFO");
}

TEST_CASE("LogParser: non-matching line gets empty-string log level", "[LogParser]")
{
	const QString log = "this does not match the log regex at all";
	auto entries = ParseString(log);
	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].level != nullptr);
	CHECK(entries[0].level->GetLevelName().isEmpty());
}

TEST_CASE("LogParser: same log level object is reused across entries", "[LogParser]")
{
	const QString log = LINE_ERROR + LINE_ERROR;
	auto entries = ParseString(log);
	REQUIRE(entries.size() == 2);
	CHECK(entries[0].level.get() == entries[1].level.get());
}

// ---------------------------------------------------------------------------
// GetUsedLogLevels
// ---------------------------------------------------------------------------
TEST_CASE("LogParser: GetUsedLogLevels returns all distinct levels", "[LogParser]")
{
	const QString log = LINE_ERROR + LINE_INFO + LINE_DEBUG + LINE_INFO; // duplicate INFO
	LogParser parser(log);
	parser.Parse();
	auto levels = parser.GetUsedLogLevels();
	CHECK(levels.size() == 3);
}

// ---------------------------------------------------------------------------
// System info extraction
// ---------------------------------------------------------------------------
TEST_CASE("LogParser: version is extracted from early log lines", "[LogParser]")
{
	const QString log = "Version: 1.2.3\n" + LINE_INFO;
	LogParser parser(log);
	parser.Parse();
	CHECK(parser.version == "1.2.3");
}

TEST_CASE("LogParser: version with build number is extracted", "[LogParser]")
{
	const QString log = "Version: 2.0.0 (42)\n" + LINE_INFO;
	LogParser parser(log);
	parser.Parse();
	CHECK(parser.version.contains("2.0.0"));
	CHECK(parser.version.contains("42"));
}

TEST_CASE("LogParser: os is extracted from early log lines", "[LogParser]")
{
	const QString log = "OS: Windows 10.0\n" + LINE_INFO;
	LogParser parser(log);
	parser.Parse();
	CHECK(parser.os.contains("Windows"));
}

TEST_CASE("LogParser: GetSystemInfo returns formatted string when info present", "[LogParser]")
{
	const QString log = "Version: 3.1.4\nOS: Android 12.0\n" + LINE_INFO;
	LogParser parser(log);
	parser.Parse();
	const QString info = parser.GetSystemInfo();
	CHECK(info.contains("3.1.4"));
	CHECK(info.contains("Android"));
}

TEST_CASE("LogParser: GetSystemInfo returns empty string when no info present", "[LogParser]")
{
	LogParser parser(LINE_INFO);
	parser.Parse();
	CHECK(parser.GetSystemInfo().isEmpty());
}

// ---------------------------------------------------------------------------
// Multi-entry handling
// ---------------------------------------------------------------------------
TEST_CASE("LogParser: multiple entries from multi-line input", "[LogParser]")
{
	const QString log = LINE_ERROR + LINE_INFO + LINE_DEBUG;
	auto entries = ParseString(log);
	CHECK(entries.size() == 3);
}

TEST_CASE("LogParser: timestamp is parsed for 2-digit year entries", "[LogParser]")
{
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	CHECK(entries[0].timeStamp.isValid());
}

TEST_CASE("LogParser: GetUsedProfile returns non-null after parsing", "[LogParser]")
{
	LogParser parser(LINE_INFO);
	parser.Parse();
	CHECK(parser.GetUsedProfile() != nullptr);
}

TEST_CASE("LogParser: WHERE component is extracted for full-format lines", "[LogParser]")
{
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	CHECK(!entries[0].components[LogComponent::WHERE].isEmpty());
}

TEST_CASE("LogParser: DATE component is extracted", "[LogParser]")
{
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	CHECK(!entries[0].components[LogComponent::DATE].isEmpty());
}

TEST_CASE("LogParser: TIME component is extracted", "[LogParser]")
{
	auto entries = ParseString(LINE_ERROR);
	REQUIRE(entries.size() == 1);
	CHECK(!entries[0].components[LogComponent::TIME].isEmpty());
}
