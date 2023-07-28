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

#include <QString>
#include <QTime>
#include "LogLevel.h"
#include <memory>
#include <chrono>
#include <array>

class LogComponent
{
public:
	enum Component {
		ORIGINAL_MESSAGE, DATE, TIME, THREAD, SUB_SYS, MESSAGE, WHERE
	};
};

struct LogEntry
{
	uint64_t entryNumber = 0;
	uint64_t lineNumber = 0;
	QDateTime timeStamp;
	std::shared_ptr<LogLevel> level;
	std::array<QString, 8> components;

	std::chrono::microseconds sinceStart{0}, sincePrevious{0};
	QString sinceStartString, sincePreviousString;
};