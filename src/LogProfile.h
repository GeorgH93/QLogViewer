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
#include <QIcon>
#include <QRegularExpression>
#include <memory>

class LogFilter;
class LogLevel;

namespace YAML
{
	class Node;
}

class LogProfile final
{
	QString profileName;
	QRegularExpression detectionRegex;
	uint32_t detectionLinesToCheck = 10, priority = 0, sysInfoLinesToCheck = 100;
	QIcon profileIcon;

	std::vector<std::shared_ptr<LogFilter>> filterPresets;
	std::vector<std::shared_ptr<LogLevel>> logLevels;
	std::string filePath;

	bool readOnly = false;

	// Regexes
	QString logEntryRegex;
	QString newlogEntryStartRegex;
	QString sysInfoVersionRegex;
	QString sysInfoDeviceRegex;
	QString sysInfoOsRegex;

public:
	LogProfile();

	LogProfile(const QString& name, const QString& detectionRegex, int detectionLinesCount);

	LogProfile(const std::string& path);

	[[nodiscard]] bool IsProfile(const QString& logMessageLine, int line) const;

	void Save() const;

	void Delete();

	void AddFilterPreset(const std::shared_ptr<LogFilter>& filter);
	void AddLogLevel(const std::shared_ptr<LogLevel>& level);


	[[nodiscard]] bool IsReadOnly() const { return readOnly; }

	void SetReadOnly(bool rOnly = true) { readOnly = rOnly; }

	[[nodiscard]] inline const QString& GetProfileName() const { return profileName; }
	[[nodiscard]] const QIcon& GetIcon() const { return profileIcon; }
	[[nodiscard]] uint32_t GetPriority() const { return priority; }
	[[nodiscard]] inline const std::string& GetFilepath() const { return filePath; }
	const std::string GetFileName();

	void SetProfileName(const QString& name);
	void SetIcon(const QString& iconFilePath);
	void SetPriority(int prio);
	inline void SetFilePath(const QString& newPath) { const std::string path = newPath.toUtf8().constData(); SetFilePath(path); }
	void SetFilePath(const std::string& newPath);


	[[nodiscard]] inline const uint32_t GetLinesToCheckForDetection() const { return detectionLinesToCheck; }
	[[nodiscard]] inline const uint32_t GetLinesToCheckForSystemInformation() const { return sysInfoLinesToCheck; }
	[[nodiscard]] inline const uint32_t GetSystemInfoLinesToCheck() const { return sysInfoLinesToCheck; }

	void SetLinesToCheckForDetection(const uint32_t lines);
	void SetLinesToCheckForSystemInformation(const uint32_t lines);
	void SetSystemInfoLinesToCheck(const uint32_t lines);

	[[nodiscard]] inline const decltype(logLevels)& GetLogLevels() { return logLevels; }
	[[nodiscard]] inline const std::vector<std::shared_ptr<LogFilter>>& GetFilterPresets() const { return filterPresets; }

	void SetLogLevels(const std::vector<std::shared_ptr<LogLevel>>& levels);

	[[nodiscard]] inline const QString& GetLogEntryRegex() const { return logEntryRegex; }
	[[nodiscard]] inline const QString& GetNewLogEntryStartRegex() const { return newlogEntryStartRegex; }
	[[nodiscard]] inline const QString& GetSystemInfoVersionRegex() const { return sysInfoVersionRegex; }
	[[nodiscard]] inline const QString& GetSystemInfoDeviceRegex() const { return sysInfoDeviceRegex; }
	[[nodiscard]] inline const QString& GetSystemInfoOsRegex() const { return sysInfoOsRegex; }

	void SetDetectionRegex(const QString& newDetectionRegex);
	void SetLogEntryRegex(const QString& regex);
	void SetNewLogEntryStartRegex(const QString& regex);
	void SetSystemInfoVersionRegex(const QString& regex);
	void SetSystemInfoDeviceRegex(const QString& regex);
	void SetSystemInfoOsRegex(const QString& regex);


	static QString FilterName(QString name);

	static std::shared_ptr<LogProfile> MakeDefault();

	static std::shared_ptr<LogProfile> GetDefault();

private:
	void Load();

	void HandleBackupFiles() const;
};