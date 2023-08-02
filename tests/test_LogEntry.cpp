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
#include "LogEntry.h"
#include "LogLevel.h"
#include <memory>

// ---------------------------------------------------------------------------
// Default construction
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: default constructed entry has zero values", "[LogEntry]")
{
	LogEntry entry;

	CHECK(entry.entryNumber == 0);
	CHECK(entry.lineNumber == 0);
	CHECK(entry.endLineNumber == 0);
	CHECK(entry.lineCount == 1);
	CHECK(entry.timeStamp.isNull());
	CHECK(entry.level == nullptr);
}

TEST_CASE("LogEntry: default constructed entry is not multi-line", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.IsMultiLine() == false);
}

TEST_CASE("LogEntry: default components are empty strings", "[LogEntry]")
{
	LogEntry entry;

	CHECK(entry.components[LogComponent::ORIGINAL_MESSAGE].isEmpty());
	CHECK(entry.components[LogComponent::MESSAGE].isEmpty());
	CHECK(entry.components[LogComponent::DATE].isEmpty());
	CHECK(entry.components[LogComponent::TIME].isEmpty());
	CHECK(entry.components[LogComponent::THREAD].isEmpty());
	CHECK(entry.components[LogComponent::SUB_SYS].isEmpty());
	CHECK(entry.components[LogComponent::WHERE].isEmpty());
}

// ---------------------------------------------------------------------------
// IsMultiLine
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: IsMultiLine returns false for lineCount 1", "[LogEntry]")
{
	LogEntry entry;
	entry.lineCount = 1;
	CHECK(entry.IsMultiLine() == false);
}

TEST_CASE("LogEntry: IsMultiLine returns true for lineCount > 1", "[LogEntry]")
{
	LogEntry entry;
	entry.lineCount = 2;
	CHECK(entry.IsMultiLine() == true);

	entry.lineCount = 10;
	CHECK(entry.IsMultiLine() == true);

	entry.lineCount = 100;
	CHECK(entry.IsMultiLine() == true);
}

// ---------------------------------------------------------------------------
// Line numbers
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: single line entry has same start and end line", "[LogEntry]")
{
	LogEntry entry;
	entry.lineNumber = 5;
	entry.endLineNumber = 5;
	entry.lineCount = 1;

	CHECK(entry.lineNumber == entry.endLineNumber);
	CHECK(entry.IsMultiLine() == false);
}

TEST_CASE("LogEntry: multi-line entry spans multiple lines", "[LogEntry]")
{
	LogEntry entry;
	entry.lineNumber = 10;
	entry.endLineNumber = 14;
	entry.lineCount = 5;

	CHECK(entry.lineNumber == 10);
	CHECK(entry.endLineNumber == 14);
	CHECK(entry.lineCount == 5);
	CHECK(entry.IsMultiLine() == true);
}

TEST_CASE("LogEntry: lineCount matches endLineNumber - lineNumber + 1", "[LogEntry]")
{
	LogEntry entry;
	entry.lineNumber = 100;
	entry.endLineNumber = 105;
	entry.lineCount = 6;

	CHECK(entry.endLineNumber - entry.lineNumber + 1 == entry.lineCount);
}

// ---------------------------------------------------------------------------
// Entry numbers
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: entryNumber can be set", "[LogEntry]")
{
	LogEntry entry;
	entry.entryNumber = 42;

	CHECK(entry.entryNumber == 42);
}

TEST_CASE("LogEntry: entryNumber is independent of lineNumber", "[LogEntry]")
{
	LogEntry entry;
	entry.entryNumber = 5;
	entry.lineNumber = 100;

	CHECK(entry.entryNumber == 5);
	CHECK(entry.lineNumber == 100);
}

// ---------------------------------------------------------------------------
// Components
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: MESSAGE component can be set", "[LogEntry]")
{
	LogEntry entry;
	entry.components[LogComponent::MESSAGE] = "This is a log message";

	CHECK(entry.components[LogComponent::MESSAGE] == "This is a log message");
}

TEST_CASE("LogEntry: ORIGINAL_MESSAGE can differ from MESSAGE", "[LogEntry]")
{
	LogEntry entry;
	entry.components[LogComponent::ORIGINAL_MESSAGE] = "23-01-15 10:00:00.000 INFO message in func() function at line 5";
	entry.components[LogComponent::MESSAGE] = "message";

	CHECK(entry.components[LogComponent::ORIGINAL_MESSAGE] != entry.components[LogComponent::MESSAGE]);
}

TEST_CASE("LogEntry: all components can be set independently", "[LogEntry]")
{
	LogEntry entry;
	entry.components[LogComponent::DATE] = "2024-01-15";
	entry.components[LogComponent::TIME] = "10:30:45.123";
	entry.components[LogComponent::THREAD] = "main";
	entry.components[LogComponent::SUB_SYS] = "Network";
	entry.components[LogComponent::WHERE] = "connect() at line 42";
	entry.components[LogComponent::MESSAGE] = "Connection established";

	CHECK(entry.components[LogComponent::DATE] == "2024-01-15");
	CHECK(entry.components[LogComponent::TIME] == "10:30:45.123");
	CHECK(entry.components[LogComponent::THREAD] == "main");
	CHECK(entry.components[LogComponent::SUB_SYS] == "Network");
	CHECK(entry.components[LogComponent::WHERE] == "connect() at line 42");
	CHECK(entry.components[LogComponent::MESSAGE] == "Connection established");
}

TEST_CASE("LogEntry: component array has correct size", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.components.size() == 8);
}

// ---------------------------------------------------------------------------
// Log level
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: level can be set to shared_ptr", "[LogEntry]")
{
	LogEntry entry;
	entry.level = std::make_shared<LogLevel>("ERROR");

	REQUIRE(entry.level != nullptr);
	CHECK(entry.level->GetLevelName() == "ERROR");
}

TEST_CASE("LogEntry: level can be null", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.level == nullptr);
}

TEST_CASE("LogEntry: multiple entries can share same level", "[LogEntry]")
{
	auto level = std::make_shared<LogLevel>("INFO");

	LogEntry entry1;
	entry1.level = level;

	LogEntry entry2;
	entry2.level = level;

	CHECK(entry1.level.get() == entry2.level.get());
}

// ---------------------------------------------------------------------------
// Timestamp
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: timeStamp can be set", "[LogEntry]")
{
	LogEntry entry;
	entry.timeStamp = QDateTime::fromString("2024-01-15 10:30:00.000", "yyyy-MM-dd hh:mm:ss.zzz");

	CHECK(entry.timeStamp.isValid());
	CHECK(entry.timeStamp.toString("yyyy-MM-dd") == "2024-01-15");
}

TEST_CASE("LogEntry: timeStamp is null by default", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.timeStamp.isNull());
}

TEST_CASE("LogEntry: timeStamp can represent various date formats", "[LogEntry]")
{
	LogEntry entry;

	// Test different date/time formats
	entry.timeStamp = QDateTime::fromString("23-01-15 10:00:00.000", "yy-MM-dd hh:mm:ss.zzz");
	CHECK(entry.timeStamp.isValid());

	entry.timeStamp = QDateTime::fromString("2024-12-31T23:59:59", Qt::ISODate);
	CHECK(entry.timeStamp.isValid());
}

// ---------------------------------------------------------------------------
// sinceStart / sincePrevious timing
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: sinceStart defaults to zero", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.sinceStart.count() == 0);
}

TEST_CASE("LogEntry: sincePrevious defaults to zero", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.sincePrevious.count() == 0);
}

TEST_CASE("LogEntry: sinceStart can be set", "[LogEntry]")
{
	LogEntry entry;
	entry.sinceStart = std::chrono::microseconds(1500000);  // 1.5 seconds

	CHECK(entry.sinceStart.count() == 1500000);
}

TEST_CASE("LogEntry: sincePrevious can be set", "[LogEntry]")
{
	LogEntry entry;
	entry.sincePrevious = std::chrono::microseconds(500000);  // 0.5 seconds

	CHECK(entry.sincePrevious.count() == 500000);
}

TEST_CASE("LogEntry: timing strings are empty by default", "[LogEntry]")
{
	LogEntry entry;
	CHECK(entry.sinceStartString.isEmpty());
	CHECK(entry.sincePreviousString.isEmpty());
}

TEST_CASE("LogEntry: timing strings can be set", "[LogEntry]")
{
	LogEntry entry;
	entry.sinceStartString = "1.500s";
	entry.sincePreviousString = "0.500s";

	CHECK(entry.sinceStartString == "1.500s");
	CHECK(entry.sincePreviousString == "0.500s");
}

// ---------------------------------------------------------------------------
// Copy semantics
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: copy constructor copies all fields", "[LogEntry]")
{
	LogEntry original;
	original.entryNumber = 10;
	original.lineNumber = 5;
	original.endLineNumber = 8;
	original.lineCount = 4;
	original.timeStamp = QDateTime::currentDateTime();
	original.level = std::make_shared<LogLevel>("WARN");
	original.components[LogComponent::MESSAGE] = "test message";
	original.sinceStart = std::chrono::microseconds(1000);
	original.sinceStartString = "1ms";

	LogEntry copy(original);

	CHECK(copy.entryNumber == original.entryNumber);
	CHECK(copy.lineNumber == original.lineNumber);
	CHECK(copy.endLineNumber == original.endLineNumber);
	CHECK(copy.lineCount == original.lineCount);
	CHECK(copy.timeStamp == original.timeStamp);
	CHECK(copy.level == original.level);  // Shared pointer, same instance
	CHECK(copy.components[LogComponent::MESSAGE] == original.components[LogComponent::MESSAGE]);
	CHECK(copy.sinceStart == original.sinceStart);
	CHECK(copy.sinceStartString == original.sinceStartString);
}

TEST_CASE("LogEntry: copy assignment copies all fields", "[LogEntry]")
{
	LogEntry original;
	original.entryNumber = 42;
	original.components[LogComponent::MESSAGE] = "original message";

	LogEntry assigned;
	assigned = original;

	CHECK(assigned.entryNumber == original.entryNumber);
	CHECK(assigned.components[LogComponent::MESSAGE] == original.components[LogComponent::MESSAGE]);
}

// ---------------------------------------------------------------------------
// Move semantics
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: move constructor transfers ownership", "[LogEntry]")
{
	LogEntry original;
	original.entryNumber = 100;
	original.level = std::make_shared<LogLevel>("DEBUG");
	original.components[LogComponent::MESSAGE] = "move test";

	LogEntry moved(std::move(original));

	CHECK(moved.entryNumber == 100);
	CHECK(moved.level != nullptr);
	CHECK(moved.level->GetLevelName() == "DEBUG");
	CHECK(moved.components[LogComponent::MESSAGE] == "move test");
}

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------
TEST_CASE("LogEntry: large entryNumber", "[LogEntry]")
{
	LogEntry entry;
	entry.entryNumber = UINT64_MAX;

	CHECK(entry.entryNumber == UINT64_MAX);
}

TEST_CASE("LogEntry: large lineNumber", "[LogEntry]")
{
	LogEntry entry;
	entry.lineNumber = UINT64_MAX;

	CHECK(entry.lineNumber == UINT64_MAX);
}

TEST_CASE("LogEntry: lineCount max value", "[LogEntry]")
{
	LogEntry entry;
	entry.lineCount = 255;  // uint8_t max

	CHECK(entry.lineCount == 255);
	CHECK(entry.IsMultiLine() == true);
}

TEST_CASE("LogEntry: empty message is valid", "[LogEntry]")
{
	LogEntry entry;
	entry.components[LogComponent::MESSAGE] = "";
	entry.components[LogComponent::ORIGINAL_MESSAGE] = "";

	CHECK(entry.components[LogComponent::MESSAGE].isEmpty());
	CHECK(entry.components[LogComponent::ORIGINAL_MESSAGE].isEmpty());
}

TEST_CASE("LogEntry: very long message", "[LogEntry]")
{
	LogEntry entry;
	QString longMessage(10000, 'x');
	entry.components[LogComponent::MESSAGE] = longMessage;

	CHECK(entry.components[LogComponent::MESSAGE].length() == 10000);
}

// ---------------------------------------------------------------------------
// LogComponent enum
// ---------------------------------------------------------------------------
TEST_CASE("LogComponent: Component enum values are sequential", "[LogEntry]")
{
	CHECK(static_cast<int>(LogComponent::ORIGINAL_MESSAGE) == 0);
	CHECK(static_cast<int>(LogComponent::DATE) == 1);
	CHECK(static_cast<int>(LogComponent::TIME) == 2);
	CHECK(static_cast<int>(LogComponent::THREAD) == 3);
	CHECK(static_cast<int>(LogComponent::SUB_SYS) == 4);
	CHECK(static_cast<int>(LogComponent::MESSAGE) == 5);
	CHECK(static_cast<int>(LogComponent::WHERE) == 6);
}
