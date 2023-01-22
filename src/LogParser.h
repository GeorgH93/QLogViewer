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
#include <QTime>
#include <QTextStream>
#include <QRegularExpression>

//TODO improve handling of multi line log messages

class LogParser final
{
	QFile* inputFile = nullptr;
	QString* logMessage = nullptr;
	QString readAhead;

	uint64_t entryCount = 0;
	uint64_t lineNumber = 1;
	bool ownsFile = false;

public:

	QString version, device, os;

	LogParser(const std::string& filePath) : inputFile(new QFile(QString(filePath.c_str()))), ownsFile(true)
	{
	}

	LogParser(QFile* file) : inputFile(file)
	{
	}

	LogParser(QString* log) : logMessage(log)
	{}

	~LogParser()
	{
		if (ownsFile)
		{
			delete inputFile;
		}
	}

	QString GetNextMessage(QTextStream& inputStream)
	{
		static QRegularExpression newLogEntryStart("^\\d\\d-\\d\\d-\\d\\d");
		QString message = "";
		while (message.isEmpty() || (!readAhead.isNull() && !newLogEntryStart.match(readAhead).hasMatch()))
		{
			if (message.isEmpty())
			{
				message = readAhead;
			}
			else if(!readAhead.isEmpty())
			{
				message += QChar(0x000023CE); //("⏎");
				message += readAhead;
			}
			readAhead = inputStream.readLine();
			lineNumber++;
		}
		return message;
	}

	std::vector<LogEntry> Parse()
	{
		std::vector<LogEntry> entries;
		entries.reserve(100000);
		QTextStream* inputStream;
		if (inputFile)
		{
			if (!inputFile->open(QIODevice::ReadOnly)) { return entries; }
			inputStream = new QTextStream(inputFile);
		}
		else
		{
			inputStream = new QTextStream(logMessage);
		}
		inputStream->setCodec("UTF-8");
		while(!inputStream->atEnd())
		{
			const uint64_t currentLine = lineNumber;
			QString msg = GetNextMessage(*inputStream);
			entries.push_back(ParseMessage(msg, currentLine));
		}
		if (inputFile)
		{
			inputFile->close();
		}
		delete inputStream;
		return entries;
	}

	LogEntry ParseMessage(const QString& message, uint64_t startLineNumber)
	{
		LogEntry e;
		e.entryNumber = ++entryCount;
		e.lineNumber = startLineNumber;
		static QRegularExpression logEntryRegex{R"(^(?<date>\d\d-\d\d-\d\d(\d\d)?)\s+(?<time>\d\d:\d\d:\d\d(\.\d+)?):?\s+(?<type>\w+)\s*:?\s+((?<subsys>(\[\s*\w+\s*\]|\w+\s?:|UI\s*:\s*\w+\s*))\s*:)?\s*(?<message>.*)\s+(?<where>in\s+~?\w+(\([^\s]*\))*\s+function\s+at\s+line\s+\d+))"};
		const auto match = logEntryRegex.match(message);
		TryExtractEnvironment(message);
		if (match.hasMatch())
		{
			e.date = match.captured("date");
			e.time = match.captured("time");
			e.type = match.captured("type");
			e.thread = match.captured("thread");
			e.subSystem = match.captured("subsys");
			e.message = match.captured("message");
			e.where = match.captured("where");

			//TODO
			e.timeStamp = QDateTime::fromString(e.date + ' ' + e.time, Qt::ISODateWithMs);
		}
		else
		{
			e.message = message;
		}
		e.originalMessage = message;
		e.Process();

		return e;
	}

	void TryExtractEnvironment(const QString& message)
	{
		static QRegularExpression versionRegex{ R"(Version\s*(:|=)?\s*([vV]?(?<version>\d+(\.\d+)*)(?<tags>(-[^-\s]+)*))\s*(\((?<buildnr>\d+)\))?)" };
		static QRegularExpression deviceRegex{ R"(Device\s*[:=]\s*(?<device>[\w ,-]+(\([\w ,]+\))?))" };
		static QRegularExpression osRegex{R"((OS|Operating\s*System)\s*[:=]\s*(?<os>(Windows|Android|iOS) [\d\.]+(\s*,\s*SDK\s*\d+)?))"};

		if (entryCount > 100) return;

		if (version.isEmpty())
		{
			const auto match = versionRegex.match(message);
			if (match.hasMatch())
			{
				version = match.captured("version") + match.captured("tags");
				auto bnr = match.captured("buildnr");
				if (!bnr.isEmpty())
				{
					version += " (" + bnr + ")";
				}
			}
		}
		if (device.isEmpty())
		{
			const auto match = deviceRegex.match(message);
			if (match.hasMatch())
			{
				device = match.captured("device");
			}
		}
		if (os.isEmpty())
		{
			const auto match = osRegex.match(message);
			if (match.hasMatch())
			{
				os = match.captured("os");
			}
		}
	}

	QString GetSystemInfo()
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