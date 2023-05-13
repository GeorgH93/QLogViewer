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

#include <QFont>
#include <QColor>
#include <QString>

struct TextViewConfig
{
	QFont font;
	QColor fontColor;
	QColor backgroundColor;
};

class LogProfile;

class AppConfig final
{
	AppConfig();

public:
	static AppConfig* GetInstance();

	~AppConfig() = default;

	void Save();

	void AddProfile(const std::shared_ptr<LogProfile>& profile);

	[[nodiscard]] std::shared_ptr<LogProfile> FindProfile(const QString& logLine, int lineNumber) const;

	[[nodiscard]] std::vector<std::shared_ptr<LogProfile>>& GetProfiles() { return profiles; }

	std::shared_ptr<LogProfile> GetProfileForName(const QString& name);

	[[nodiscard]] bool UseCopyOnWriteEnabled() const { return copyOnWrite; }

	void SetCopyOnWrite(bool enableCOW);

	[[nodiscard]] uint32_t GetFilesToKeepInHistory() const { return filesToKeepInHistory; }

	void SetFilesToKeepInHistory(uint32_t count);

	[[nodiscard]] QColor GetHighlightedLineBackgroundColor() const { return highlightedLineBackgroundColor; };

	void SetHighlightedLineBackgroundColor(const QColor& color);

private:
	void Load();

	void LoadConfig();

	void LoadProfiles();

	void HandleBackupFiles() const;

	[[nodiscard]] int GetMaxLinesToCheckValue() const;

private:
	std::string filePath;

	std::vector<std::shared_ptr<LogProfile>> profiles;

	bool copyOnWrite = false;

	TextViewConfig mainLogViewConfig, fullLogViewConfig;

	uint32_t filesToKeepInHistory;

	QColor highlightedLineBackgroundColor;

public:
	static const QString& GetAppDataLocation();

	static const QString& GetProfilesLocation();
};
