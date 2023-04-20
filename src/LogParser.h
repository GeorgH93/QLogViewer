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
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QRegularExpression>

//TODO improve handling of multi line log messages

class LogProfile;

class LogParser final
{
	QFile* inputFile = nullptr;
	QString logMessage = nullptr;
	QString readAhead;

	uint64_t entryCount = 0;
	uint64_t lineNumber = 0;
	bool ownsFile = false;

	QMap<QString, std::shared_ptr<LogLevel>> logLevelMap;

	std::shared_ptr<LogProfile> logProfile;

	// Regexes
	QRegularExpression logEntryRegex;
	QRegularExpression newLogEntryStart;
	QRegularExpression versionRegex;
	QRegularExpression deviceRegex;
	QRegularExpression osRegex;

public:

	QString version, device, os;

	LogParser(const std::string& filePath) : inputFile(new QFile(QString(filePath.c_str()))), ownsFile(true)
	{}

	explicit LogParser(QFile* file) : inputFile(file)
	{}

	explicit LogParser(const QString& log) : logMessage(log)
	{}

	~LogParser();

	std::vector<LogEntry> Parse();

	[[nodiscard]] QString GetSystemInfo() const;

	[[nodiscard]] std::shared_ptr<LogProfile> GetUsedProfile() const { return logProfile; }

private:
	void TryExtractEnvironment(const QString& message);

	QString GetNextMessage(QTextStream& inputStream);

	LogEntry ParseMessage(const QString& message, uint64_t startLineNumber);

	bool IsNewLogMessage(const QString& string);

	void FindLogProfile(QTextStream* inputStream);

	void LoadRegexesFromProfile();

	std::shared_ptr<LogLevel> GetLogLevel(const QString& levelName);
};