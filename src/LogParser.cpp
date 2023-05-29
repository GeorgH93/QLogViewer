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
#include "AppConfig.h"
#include "LogProfile.h"
#include <QRegularExpression>

namespace
{
	const QString MATCH_GROUP_DATE = "date";
	const QString MATCH_GROUP_TIME = "time";
	const QString MATCH_GROUP_THREAD = "thread";
	const QString MATCH_GROUP_SUBSYS = "subsys";
	const QString MATCH_GROUP_MESSAGE = "message";
	const QString MATCH_GROUP_WHERE = "where";
	const QString MATCH_GROUP_LEVEL = "level";
}

LogParser::~LogParser()
{
	if (ownsFile)
	{
		delete inputFile;
	}
}

void LogParser::FindLogProfile(QTextStream* inputStream)
{
	int line = 0;
	while (!inputStream->atEnd())
	{
		logProfile = AppConfig::GetInstance()->FindProfile(inputStream->readLine(), ++line);
		if (logProfile)
		{
			break;
		}
	}
	if (!logProfile) logProfile = LogProfile::GetDefault();
	inputStream->seek(0);
	for(const auto& level : logProfile->GetLogLevels())
	{
		logLevelMap.insert(level->GetLevelName(), level);
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
		inputStream = new QTextStream(&logMessage, QIODevice::ReadOnly);
	}
	inputStream->setCodec("UTF-8");

	FindLogProfile(inputStream);

	LoadRegexesFromProfile();

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

void LogParser::LoadRegexesFromProfile()
{
	logEntryRegex = QRegularExpression(logProfile->GetLogEntryRegex());
	newLogEntryStart = QRegularExpression(logProfile->GetNewLogEntryStartRegex());
	versionRegex = QRegularExpression(logProfile->GetSystemInfoVersionRegex());
	deviceRegex = QRegularExpression(logProfile->GetSystemInfoDeviceRegex());
	osRegex = QRegularExpression(logProfile->GetSystemInfoOsRegex());
}

bool LogParser::IsNewLogMessage(const QString& string)
{
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

QString GetMatchFromRawData(const QRegularExpressionMatch& match, const QString& group)
{
	QStringView view = match.capturedView(group);
	return QString::fromRawData((QChar*)view.utf16(), view.length());
}

LogEntry LogParser::ParseMessage(const QString& message, uint64_t startLineNumber)
{
	LogEntry e;
	e.entryNumber = ++entryCount;
	e.lineNumber = startLineNumber;
	const auto match = logEntryRegex.match(message);
	TryExtractEnvironment(message);

	e.originalMessage = message;

	if (match.hasMatch())
	{
		e.date = GetMatchFromRawData(match, MATCH_GROUP_DATE);
		e.time = GetMatchFromRawData(match, MATCH_GROUP_TIME);
		e.thread = GetMatchFromRawData(match, MATCH_GROUP_THREAD);
		e.subSystem = GetMatchFromRawData(match, MATCH_GROUP_SUBSYS);
		e.message = GetMatchFromRawData(match, MATCH_GROUP_MESSAGE);
		e.where = GetMatchFromRawData(match, MATCH_GROUP_WHERE);

		// Read log level
		const QString type = GetMatchFromRawData(match, MATCH_GROUP_LEVEL);
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
	e.Process();

	return e;
}

inline void ExtractEnvironmentComponent(const QString& message, QString& targetVar, const QString& captureGroupName, const QRegularExpression& expression)
{
	if (targetVar.isEmpty())
	{
		const auto match = expression.match(message);
		if (match.hasMatch())
		{
			targetVar = match.captured(captureGroupName);
		}
	}
}

void LogParser::TryExtractEnvironment(const QString& message)
{
	if (entryCount > logProfile->GetSystemInfoLinesToCheck()) return;

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
	ExtractEnvironmentComponent(message, device, "device", deviceRegex);
	ExtractEnvironmentComponent(message, os, "os", osRegex);
}

[[nodiscard]] QString LogParser::GetSystemInfo() const
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
