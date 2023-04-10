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
	int detectionLinesToCheck = 10, priority = 0;
	QIcon profileIcon;

	std::vector<std::shared_ptr<LogFilter>> filterPresets;
	std::vector<std::shared_ptr<LogLevel>> logLevels;
	std::string filePath;

	bool readOnly = false;

public:
	LogProfile(const QString& name, const QString& detectionRegex, int detectionLinesCount)
		: profileName(name), detectionRegex(detectionRegex), detectionLinesToCheck(detectionLinesCount)
	{}

	LogProfile(const std::string& path);

	[[nodiscard]] inline int GetLinesToCheckForDetection() const { return detectionLinesToCheck; }

	[[nodiscard]] bool IsProfile(const QString& logMessageLine, int line) const;

	[[nodiscard]] inline const QString& GetProfileName() const { return profileName; }

	[[nodiscard]] inline const std::vector<std::shared_ptr<LogFilter>>& GetFilterPresets() const { return filterPresets; }

	void Save() const;

	void Delete();

	void AddFilterPreset(const std::shared_ptr<LogFilter>& filter);

	void SetProfileName(const QString& name);

	void SetDetectionRegex(const QString& newDetectionRegex);

	inline void SetFilePath(const QString& newPath) { const std::string path = newPath.toUtf8().constData(); SetFilePath(path); }

	void SetFilePath(const std::string& newPath);

	void SetIcon(const QString& iconFilePath);

	[[nodiscard]] const QIcon& GetIcon() const { return profileIcon; }

	[[nodiscard]] int GetPriority() const { return priority; }

	void SetPriority(int prio);

	[[nodiscard]] bool IsReadOnly() const { return readOnly; }

	void SetReadOnly() { readOnly = true; }

	static QString FilterName(QString name);

private:
	void Load();

	void HandleBackupFiles() const;
};