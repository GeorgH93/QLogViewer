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
#include "FormatedStringCache.h"

TEST_CASE("FormattedStringCache: NumberAsString(0) returns '0'", "[FormattedStringCache]")
{
	CHECK(FormattedStringCache::NumberAsString(0) == "0");
}

TEST_CASE("FormattedStringCache: NumberAsString(1) returns '1'", "[FormattedStringCache]")
{
	CHECK(FormattedStringCache::NumberAsString(1) == "1");
}

TEST_CASE("FormattedStringCache: NumberAsString for arbitrary values", "[FormattedStringCache]")
{
	CHECK(FormattedStringCache::NumberAsString(42) == "42");
	CHECK(FormattedStringCache::NumberAsString(100) == "100");
	CHECK(FormattedStringCache::NumberAsString(999) == "999");
}

TEST_CASE("FormattedStringCache: returns reference to same string on repeated calls", "[FormattedStringCache]")
{
	const QString& a = FormattedStringCache::NumberAsString(7);
	const QString& b = FormattedStringCache::NumberAsString(7);
	// Same cache entry — same address
	CHECK(&a == &b);
}

TEST_CASE("FormattedStringCache: large numbers are handled", "[FormattedStringCache]")
{
	CHECK(FormattedStringCache::NumberAsString(1000000) == "1000000");
}

TEST_CASE("FormattedStringCache: consecutive numbers are correct", "[FormattedStringCache]")
{
	for (uint64_t i = 0; i <= 20; ++i)
	{
		CHECK(FormattedStringCache::NumberAsString(i) == QString::number(i));
	}
}
