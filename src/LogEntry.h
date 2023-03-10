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

struct LogEntry
{
	uint64_t entryNumber;
	uint64_t lineNumber;
	QDateTime timeStamp;
	QString date, time;
	std::shared_ptr<LogLevel> level;
	QString thread;
	QString subSystem;
	QString where;

	QString message;

	QString originalMessage;


	QString entryNumberString;
	QString lineNumberString;

	std::chrono::microseconds sinceStart, sincePrevious;
	QString sinceStartString, sincePreviousString;

	void Process()
	{
		entryNumberString = QString::number(entryNumber);
		lineNumberString = QString::number(lineNumber);
	}
};