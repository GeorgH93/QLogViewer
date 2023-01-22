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

#include <LogEntry.h>
#include <QString>
#include <QTextStream>

//TODO improve handling of multi line log messages

class LogParser final
{
	QFile* inputFile = nullptr;
	QString* logMessage = nullptr;
	QString readAhead;

	uint64_t entryCount = 0;
	uint64_t lineNumber = 1;
	bool ownsFile = false;

	QMap<QString, std::shared_ptr<LogType>> logTypeMap;

public:

	QString version, device, os;

	LogParser(const std::string& filePath) : inputFile(new QFile(QString(filePath.c_str()))), ownsFile(true)
	{}

	LogParser(QFile* file) : inputFile(file)
	{}

	LogParser(QString* log) : logMessage(log)
	{}

	~LogParser();

	QString GetNextMessage(QTextStream& inputStream);

	std::vector<LogEntry> Parse();

	LogEntry ParseMessage(const QString& message, uint64_t startLineNumber);

	void TryExtractEnvironment(const QString& message);

	QString GetSystemInfo() const
	{
		QString systemInfo;
		if (!version.isEmpty())
		{
			systemInfo += "Version: " + version;
		}
		if (!device.isEmpty())
		{
			systemInfo += "\tDevice: " + device;
		}
		if (!os.isEmpty())
		{
			systemInfo += "\tOS: " + os;
		}
		return systemInfo;
	}
};