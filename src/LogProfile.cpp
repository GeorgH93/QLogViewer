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

#include "LogProfile.h"
#include "LogLevel.h"
#include "LogLevel.Convert.h"
#include "LogFilter.h"
#include "LogFilter.Convert.h"
#include "YamlConverters.h"
#include "AppConfig.h"
#include <filesystem>
#include <fstream>
#include <QFile>
#include <yaml-cpp/yaml.h>

// TODO cache yaml config tree??

LogProfile::LogProfile(const std::string& path)
	: filePath(path)
{
	Load();
}

bool LogProfile::IsProfile(const QString& logMessageLine) const
{
	const auto match = detectionRegex.match(logMessageLine);
	return match.hasMatch();
}

void LogProfile::AddFilterPreset(const std::shared_ptr<LogFilter>& filter)
{
	filterPresets.push_back(filter);
	Save();
}

QString LogProfile::FilterName(QString name)
{
	if (name.length() > 40)
	{
		name = name.left(40);
	}
	// Remove special chars
	name.remove(QRegExp("[¥/\\.?*|<>:]"));
	return name;
}

void LogProfile::SetProfileName(const QString& name)
{
	profileName = FilterName(name);
	SetFilePath(AppConfig::GetProfilesLocation() + "/" + profileName + ".yml");
}

void LogProfile::SetDetectionRegex(const QString& newDetectionRegex)
{
	detectionRegex = QRegularExpression(newDetectionRegex);
	Save();
}

void LogProfile::SetFilePath(const std::string& newPath)
{
	if (!filePath.empty() && std::filesystem::exists(filePath))
	{ // delete old file
		std::filesystem::remove(filePath);
	}
	filePath = newPath;
	Save();
}

void LogProfile::SetPriority(int prio)
{
	priority = prio;
	Save();
}

void LogProfile::SetIcon(const QString& iconFilePath)
{
	if (!QFile::exists(iconFilePath)) return;

    QImage image(iconFilePath);
    if (image.format() != QImage::Format_ARGB32)
    {
        image = image.convertToFormat(QImage::Format_ARGB32);
    }
    int size = std::min(image.width(), image.height());
    if (image.height() != image.width())
    { // Crop image
        const QRect rect((image.width() - size) / 2, (image.height() - size) / 2, size, size);
        image = image.copy(rect);
    }
    if (size > 32)
    { // resize large image
        image = image.scaled(32, 32);
    }
    profileIcon = QIcon(QPixmap::fromImage(image));
}

void LogProfile::Load()
{
	YAML::Node config = YAML::LoadFile(filePath);
	profileName = config["Name"].as<QString>();
	priority = config["Priority"].as<int>(1);
	detectionRegex = QRegularExpression(config["DetectionRegex"].as<QString>());
	detectionLinesToCheck = config["DetectionRange"].as<int>(10);
	filterPresets = config["FilterPresets"].as<decltype(filterPresets)>(decltype(filterPresets)());
	logLevels = config["LogLevels"].as<decltype(logLevels)>(decltype(logLevels)());
}

void LogProfile::Save() const
{
	if (readOnly || filePath.empty()) return;
	HandleBackupFiles();
	std::ofstream stream;
	stream.open(filePath, std::ios::out);
	YAML::Emitter configWriter(stream);

	YAML::Node config;
	config["Name"] = profileName;
	config["Priority"] = priority;
	config["DetectionRegex"] = detectionRegex.pattern();
	config["DetectionRange"] = detectionLinesToCheck;
	config["FilterPresets"] = filterPresets;
	config["LogLevels"] = logLevels;
    //config["Icon"] = profileIcon;

	configWriter << config;

	stream.close();
}

void LogProfile::HandleBackupFiles() const
{
	std::string backupPath = filePath + ".back";
	if (std::filesystem::exists(backupPath))
	{ // delete old backup file
		std::filesystem::remove(backupPath);
	}
	if (AppConfig::GetInstance()->UseCopyOnWriteEnabled())
	{
		if (std::filesystem::exists(filePath))
		{ // move file to backup file
			std::filesystem::rename(filePath, backupPath);
		}
	}
}
