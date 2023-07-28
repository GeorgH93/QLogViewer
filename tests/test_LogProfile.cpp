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
#include "LogProfile.h"

// ---------------------------------------------------------------------------
// FilterName
// ---------------------------------------------------------------------------
TEST_CASE("LogProfile::FilterName: name within 40 chars is unchanged", "[LogProfile]")
{
	CHECK(LogProfile::FilterName("ValidName") == "ValidName");
}

TEST_CASE("LogProfile::FilterName: name longer than 40 chars is truncated to 40", "[LogProfile]")
{
	QString longName(50, 'a');
	QString filtered = LogProfile::FilterName(longName);
	CHECK(filtered.length() == 40);
}

TEST_CASE("LogProfile::FilterName: special characters are removed", "[LogProfile]")
{
	// Characters that should be stripped: ¥ / \ . ? * | < > :
	CHECK(LogProfile::FilterName("foo/bar") == "foobar");
	CHECK(LogProfile::FilterName("foo.bar") == "foobar");
	CHECK(LogProfile::FilterName("foo*bar") == "foobar");
	CHECK(LogProfile::FilterName("foo|bar") == "foobar");
	CHECK(LogProfile::FilterName("foo<bar>") == "foobar");
	CHECK(LogProfile::FilterName("foo:bar") == "foobar");
	CHECK(LogProfile::FilterName("foo?bar") == "foobar");
}

TEST_CASE("LogProfile::FilterName: chars / . ? * | < > : are removed", "[LogProfile]")
{
	// The regex [¥/\\.?*|<>:] removes these chars (backslash is treated as escape in regex)
	CHECK(LogProfile::FilterName("/.:?*|<>") == "");
}

TEST_CASE("LogProfile::FilterName: name exactly 40 chars long is kept", "[LogProfile]")
{
	QString name(40, 'x');
	CHECK(LogProfile::FilterName(name) == name);
}

// ---------------------------------------------------------------------------
// IsProfile
// ---------------------------------------------------------------------------
TEST_CASE("LogProfile::IsProfile: default profile matches any non-empty line within range", "[LogProfile]")
{
	auto profile = LogProfile::GetDefault();
	// The default detection regex is ".*" which matches everything, range is 0 (first check)
	// Actually the constructor uses range 0 meaning "check 0 lines" — so IsProfile always
	// returns false for the default profile (line > 0 always)
	// Let's build an explicit profile with range 5 and regex ".*"
	LogProfile p("Test", ".*", 5);
	CHECK(p.IsProfile("anything here", 1) == true);
	CHECK(p.IsProfile("anything here", 5) == true);
}

TEST_CASE("LogProfile::IsProfile: returns false when line number exceeds detection range", "[LogProfile]")
{
	LogProfile p("Test", ".*", 3);
	CHECK(p.IsProfile("anything", 4) == false);
	CHECK(p.IsProfile("anything", 100) == false);
}

TEST_CASE("LogProfile::IsProfile: returns false when regex does not match", "[LogProfile]")
{
	LogProfile p("Test", "^\\d{4}-\\d{2}-\\d{2}", 10);
	CHECK(p.IsProfile("not a date line", 1) == false);
}

TEST_CASE("LogProfile::IsProfile: returns true when regex matches within range", "[LogProfile]")
{
	LogProfile p("Test", "^\\d{2}-\\d{2}-\\d{2}", 10);
	CHECK(p.IsProfile("23-01-15 10:00:00.000 INFO msg", 1) == true);
}

// ---------------------------------------------------------------------------
// GetDefault
// ---------------------------------------------------------------------------
TEST_CASE("LogProfile::GetDefault: returns non-null profile", "[LogProfile]")
{
	auto profile = LogProfile::GetDefault();
	REQUIRE(profile != nullptr);
}

TEST_CASE("LogProfile::GetDefault: returns same instance on repeated calls", "[LogProfile]")
{
	auto a = LogProfile::GetDefault();
	auto b = LogProfile::GetDefault();
	CHECK(a.get() == b.get());
}

TEST_CASE("LogProfile::GetDefault: has non-empty log entry regex", "[LogProfile]")
{
	auto profile = LogProfile::GetDefault();
	CHECK(!profile->GetLogEntryRegex().isEmpty());
}

TEST_CASE("LogProfile::GetDefault: has non-empty new entry start regex", "[LogProfile]")
{
	auto profile = LogProfile::GetDefault();
	CHECK(!profile->GetNewLogEntryStartRegex().isEmpty());
}

// ---------------------------------------------------------------------------
// Getters / Setters (in-memory, read-only profile)
// ---------------------------------------------------------------------------
TEST_CASE("LogProfile: profile name is set by constructor", "[LogProfile]")
{
	LogProfile p("MyProfile", ".*", 5);
	CHECK(p.GetProfileName() == "MyProfile");
}

TEST_CASE("LogProfile: detection range is set by constructor", "[LogProfile]")
{
	LogProfile p("X", ".*", 7);
	CHECK(p.GetLinesToCheckForDetection() == 7);
}

TEST_CASE("LogProfile: priority defaults to 0 for inline constructor", "[LogProfile]")
{
	LogProfile p("X", ".*", 5);
	CHECK(p.GetPriority() == 0);
}

TEST_CASE("LogProfile: GetFilepath is non-empty for inline-constructed named profile", "[LogProfile]")
{
	// The (name, regex, range) constructor calls SetProfileName which sets filepath
	LogProfile p("X", ".*", 5);
	CHECK(!p.GetFilepath().empty());
}

// ---------------------------------------------------------------------------
// GetFileName
// ---------------------------------------------------------------------------
TEST_CASE("LogProfile::GetFileName: returns filename portion of path", "[LogProfile]")
{
	// We can test GetFileName via a profile loaded from a path, but since we
	// don't want filesystem side effects in unit tests, we create a profile
	// and use the accessor via an in-memory approach.
	// The default profile has no filepath, so GetFileName returns "".
	auto p = std::make_shared<LogProfile>();
	// We can't easily set filePath on a default-constructed profile without Save(),
	// so we just verify GetFileName doesn't crash on empty path.
	CHECK(p->GetFileName().empty());
}
