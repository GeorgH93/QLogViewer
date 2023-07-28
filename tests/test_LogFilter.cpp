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

TEST_CASE("LogFilter: default ShowEntry always returns true", "[LogFilter]")
{
	LogFilter filter;
	LogEntry entry{};
	CHECK(filter.ShowEntry(entry) == true);
}

TEST_CASE("LogFilter: can be default constructed", "[LogFilter]")
{
	REQUIRE_NOTHROW([]() { LogFilter f; (void)f; }());
}
