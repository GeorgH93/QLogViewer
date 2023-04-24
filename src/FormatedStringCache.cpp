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

#include "FormatedStringCache.h"

std::vector<QString> FormattedStringCache::numberStrings;

const QString& FormattedStringCache::NumberAsString(uint64_t number)
{
	for(uint64_t i = numberStrings.size(); i <= number; i++)
	{
		numberStrings.push_back(QString::number(i));
	}
	return numberStrings[number];
}