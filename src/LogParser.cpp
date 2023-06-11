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
#include <memory>

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
	std::unique_ptr<QTextStream> inputStream;
	if (inputFile)
	{
		if (!inputFile->open(QIODevice::ReadOnly)) { return entries; }
		inputStream = std::make_unique<QTextStream>(inputFile);
	}
	else
	{
		inputStream = std::make_unique<QTextStream>(&logMessage, QIODevice::ReadOnly);
	}
	inputStream->setCodec("UTF-8");

	FindLogProfile(inputStream.get());

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

LogEntry LogParser::ParseMessage(const QString& message, uint64_t startLineNumber)
{
	LogEntry e;
	e.entryNumber = ++entryCount;
	e.lineNumber = startLineNumber;
	const auto match = logEntryRegex.match(message);
	TryExtractEnvironment(message);
	if (match.hasMatch())
	{
		e.components[LogComponent::DATE] = match.captured("date");
		e.components[LogComponent::TIME] = match.captured("time");
		e.components[LogComponent::THREAD] = match.captured("thread");
		e.components[LogComponent::SUB_SYS] = match.captured("subsys");
		e.components[LogComponent::MESSAGE] = match.captured("message");
		e.components[LogComponent::WHERE] = match.captured("where");

		// Read log level
		e.level = GetLogLevel(match.captured("level"));

		//TODO
		e.timeStamp = QDateTime::fromString("20" + e.components[LogComponent::DATE] + ' ' + e.components[LogComponent::TIME], Qt::ISODateWithMs);
	}
	else
	{
		e.components[LogComponent::MESSAGE] = message;
		e.level = GetLogLevel("");
	}
	e.components[LogComponent::ORIGINAL_MESSAGE] = message;

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

std::shared_ptr<LogLevel> LogParser::GetLogLevel(const QString& type)
{
	if (logLevelMap.contains(type))
	{
		return logLevelMap[type];
	}
	auto level = std::make_shared<LogLevel>(type);
	logLevelMap.insert(type, level);
	return level;
}

std::vector<std::shared_ptr<LogLevel>> LogParser::GetUsedLogLevels() const
{
	std::vector<std::shared_ptr<LogLevel>> usedLevels;
	for(const auto& level : logLevelMap)
	{
		usedLevels.push_back(level);
	}
	return usedLevels;
}
