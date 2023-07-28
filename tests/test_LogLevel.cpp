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
#include "LogLevel.h"

TEST_CASE("LogLevel: default constructor gives empty level name", "[LogLevel]")
{
	LogLevel level;
	CHECK(level.GetLevelName().isEmpty());
}

TEST_CASE("LogLevel: constructor sets level name", "[LogLevel]")
{
	LogLevel level("ERROR");
	CHECK(level.GetLevelName() == "ERROR");
}

TEST_CASE("LogLevel: constructor sets font color", "[LogLevel]")
{
	LogLevel level("INFO", Qt::red);
	CHECK(level.GetFontColor() == Qt::red);
}

TEST_CASE("LogLevel: constructor sets background color", "[LogLevel]")
{
	LogLevel level("DEBUG", Qt::black, Qt::yellow);
	CHECK(level.GetBackgroundColor() == Qt::yellow);
}

TEST_CASE("LogLevel: SetLevelName updates the name", "[LogLevel]")
{
	LogLevel level("OLD");
	level.SetLevelName("NEW");
	CHECK(level.GetLevelName() == "NEW");
}

TEST_CASE("LogLevel: SetFontColor updates color for valid color", "[LogLevel]")
{
	LogLevel level;
	level.SetFontColor(Qt::blue);
	CHECK(level.GetFontColor() == Qt::blue);
}

TEST_CASE("LogLevel: SetFontColor ignores invalid color", "[LogLevel]")
{
	LogLevel level("X", Qt::green);
	level.SetFontColor(QColor()); // invalid
	CHECK(level.GetFontColor() == Qt::green);
}

TEST_CASE("LogLevel: SetBackgroundColor updates color for valid color", "[LogLevel]")
{
	LogLevel level;
	level.SetBackgroundColor(Qt::cyan);
	CHECK(level.GetBackgroundColor() == Qt::cyan);
}

TEST_CASE("LogLevel: SetBackgroundColor ignores invalid color", "[LogLevel]")
{
	LogLevel level("X", Qt::black, Qt::magenta);
	level.SetBackgroundColor(QColor()); // invalid
	CHECK(level.GetBackgroundColor() == Qt::magenta);
}

TEST_CASE("LogLevel: copy constructor produces independent copy", "[LogLevel]")
{
	LogLevel original("WARN", Qt::red, Qt::yellow, Qt::AlignLeft);
	LogLevel copy(original);
	CHECK(copy.GetLevelName() == "WARN");
	CHECK(copy.GetFontColor() == Qt::red);
	CHECK(copy.GetBackgroundColor() == Qt::yellow);
	CHECK(copy.GetAlignment() == Qt::AlignLeft);
}

TEST_CASE("LogLevel: move constructor transfers data", "[LogLevel]")
{
	LogLevel original("WARN", Qt::red, Qt::yellow, Qt::AlignRight);
	LogLevel moved(std::move(original));
	CHECK(moved.GetLevelName() == "WARN");
	CHECK(moved.GetFontColor() == Qt::red);
	CHECK(moved.GetAlignment() == Qt::AlignRight);
}

TEST_CASE("LogLevel: copy assignment operator", "[LogLevel]")
{
	LogLevel a("A", Qt::blue);
	LogLevel b;
	b = a;
	CHECK(b.GetLevelName() == "A");
	CHECK(b.GetFontColor() == Qt::blue);
}

TEST_CASE("LogLevel: GetMetaDescription text references levelName", "[LogLevel]")
{
	LogLevel level("TRACE");
	const auto& desc = level.GetMetaDescription();
	CHECK(desc.text == "TRACE");
}

TEST_CASE("LogLevel: SetAlignment updates alignment in meta description", "[LogLevel]")
{
	LogLevel level;
	level.SetAlignment(Qt::AlignRight);
	CHECK(level.GetAlignment() == Qt::AlignRight);
	CHECK(level.GetMetaDescription().alignment == Qt::AlignRight);
}
