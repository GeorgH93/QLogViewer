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

#include "LogParser.h"
#include <QRegularExpression>

LogParser::~LogParser()
{
	if (ownsFile)
	{
		delete inputFile;
	}
}

std::vector<LogEntry> LogParser::Parse()
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

	//TODO fill logType with known log types from profile

	QString msg;
	uint64_t currentLine = 1;
	while (!(msg = GetNextMessage(*inputStream)).isEmpty())
	{
		if (!msg.isEmpty()) entries.push_back(ParseMessage(msg, currentLine));
		currentLine = lineNumber;
	}
	if (inputFile)
	{
		inputFile->close();
	}
	delete inputStream;
	return entries;
}

bool IsNewLogMessage(const QString& string)
{ //TODO move to log profile
	static const QRegularExpression newLogEntryStart(R"(^\d\d-\d\d-\d\d)");
	if (string.isEmpty()) return false;
	return newLogEntryStart.match(string).hasMatch();
}

QString LogParser::GetNextMessage(QTextStream& inputStream)
{
	QString message;
	while((message.isEmpty() || !IsNewLogMessage(readAhead)) && (!inputStream.atEnd() || !readAhead.isNull()))
	{
		if (message.isEmpty())
		{
			message = readAhead;
		}
		else if (!readAhead.isEmpty())
		{
			message += QChar(0x000023CE); //("⏎");
			message += readAhead;
		}
		readAhead = inputStream.readLine();
		lineNumber++;
	}
	return message;
}

LogEntry LogParser::ParseMessage(const QString& message, uint64_t startLineNumber)
{
	LogEntry e;
	e.entryNumber = ++entryCount;
	e.lineNumber = startLineNumber;
	static QRegularExpression logEntryRegex{ R"(^(?<date>\d\d-\d\d-\d\d(\d\d)?)\s+(?<time>\d\d:\d\d:\d\d(\.\d+)?):?\s+(?<level>\w+)\s*:?\s+((?<subsys>(\[\s*\w+\s*\]|\w+\s?:|UI\s*:\s*\w+\s*))\s*:)?\s*(?<message>.*)\s+(?<where>in\s+~?\w+(\([^\s]*\))*\s+function\s+at\s+line\s+\d+))" };
	const auto match = logEntryRegex.match(message);
	TryExtractEnvironment(message);
	if (match.hasMatch())
	{
		e.date = match.captured("date");
		e.time = match.captured("time");
		e.thread = match.captured("thread");
		e.subSystem = match.captured("subsys");
		e.message = match.captured("message");
		e.where = match.captured("where");

		// Read log level
		const QString type = match.captured("level");
		if (logLevelMap.contains(type))
		{
			e.level = logLevelMap[type];
		}
		else
		{
			e.level = std::make_shared<LogLevel>(type);
			logLevelMap.insert(type, e.level);
		}

		//TODO
		e.timeStamp = QDateTime::fromString("20" + e.date + ' ' + e.time, Qt::ISODateWithMs);
	}
	else
	{
		e.message = message;
	}
	e.originalMessage = message;
	e.Process();

	return e;
}

void LogParser::TryExtractEnvironment(const QString& message)
{
	static QRegularExpression versionRegex{ R"(Version\s*(:|=)?\s*([vV]?(?<version>\d+(\.\d+)*)(?<tags>(-[^-\s]+)*))\s*(\((?<buildnr>\d+)\))?)" };
	static QRegularExpression deviceRegex{ R"(Device\s*[:=]\s*(?<device>[\w ,-]+(\([\w ,]+\))?))" };
	static QRegularExpression osRegex{ R"((OS|Operating\s*System)\s*[:=]\s*(?<os>(Windows|Android|iOS) [\d\.]+(\s*,\s*SDK\s*\d+)?))" };

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
