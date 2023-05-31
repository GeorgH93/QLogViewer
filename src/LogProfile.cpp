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

LogProfile::LogProfile() : readOnly(true)
{}

LogProfile::LogProfile(const std::string& path)
	: filePath(path)
{
	// Allow easy usage of this constructor by handling external profile file locations
	if (!path.empty() && path.rfind(AppConfig::GetProfilesLocation().toStdString(), 0) != 0)
	{
		const std::string& targetPath = AppConfig::GetProfilesLocation().toStdString() + filePath.substr(filePath.find_last_of("/"), filePath.size() - 1);

		std::filesystem::copy_file(filePath, targetPath);
		
		filePath = targetPath;
	}

	Load();
}

LogProfile::LogProfile(const QString &name, const QString &detectionRegex, int detectionLinesCount)
		: detectionRegex(detectionRegex), detectionLinesToCheck(detectionLinesCount), readOnly(true)
{
	SetProfileName(name);
}

bool LogProfile::IsProfile(const QString& logMessageLine, int line) const
{
	if (line > GetLinesToCheckForDetection()) return false;
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

const std::string LogProfile::GetFileName()
{
	std::string filepath = filePath.substr(filePath.find_last_of("/") + 1, filePath.length() - 1);
	return filepath;
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
	auto defaultProfile = LogProfile::GetDefault();
	YAML::Node config = YAML::LoadFile(filePath);
	profileName = config["Name"].as<QString>();
	priority = config["Priority"].as<int>(1);
	detectionRegex = QRegularExpression(config["DetectionRegex"].as<QString>(defaultProfile->detectionRegex.pattern()));
	detectionLinesToCheck = config["DetectionRange"].as<int>(10);
	filterPresets = config["FilterPresets"].as<decltype(filterPresets)>(decltype(filterPresets)());
	logLevels = config["LogLevels"].as<decltype(logLevels)>(decltype(logLevels)());
	profileIcon = config["Icon"].as<decltype(profileIcon)>(decltype(profileIcon)());

	logEntryRegex = config["Entries.Regex"].as<QString>(defaultProfile->GetLogEntryRegex());
	newlogEntryStartRegex = config["Entries.NewEntryStartRegex"].as<QString>(defaultProfile->GetNewLogEntryStartRegex());
	sysInfoVersionRegex = config["SystemInfo.VersionRegex"].as<QString>(defaultProfile->GetSystemInfoVersionRegex());
	sysInfoDeviceRegex = config["SystemInfo.DeviceRegex"].as<QString>(defaultProfile->GetSystemInfoDeviceRegex());
	sysInfoOsRegex = config["SystemInfo.OsRegex"].as<QString>(defaultProfile->GetSystemInfoOsRegex());
	sysInfoLinesToCheck = config["SystemInfo.LinesToCheck"].as<uint32_t>(sysInfoLinesToCheck);
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
    config["Icon"] = profileIcon;

	config["Entries.Regex"] = logEntryRegex;
	config["Entries.NewEntryStartRegex"] = newlogEntryStartRegex;
	config["SystemInfo.VersionRegex"] = sysInfoVersionRegex;
	config["SystemInfo.DeviceRegex"] = sysInfoDeviceRegex;
	config["SystemInfo.OsRegex"] = sysInfoOsRegex;
	config["SystemInfo.LinesToCheck"] = sysInfoLinesToCheck;

	configWriter << config;

	stream.close();
}

void LogProfile::Delete()
{
	SetReadOnly();
	if (std::filesystem::exists(filePath))
	{
		std::filesystem::remove(filePath);
	}
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

std::shared_ptr<LogProfile> LogProfile::MakeDefault()
{
	auto profile = std::make_shared<LogProfile>("Default", ".*", 0);

	profile->SetLogEntryRegex(R"(^(?<date>\d\d-\d\d-\d\d(\d\d)?)\s+(?<time>\d\d:\d\d:\d\d(\.\d+)?):?\s+(?<level>\w+)\s*:?\s+((?<subsys>(\[\s*\w+\s*\]|\w+\s?:|UI\s*:\s*\w+\s*))\s*:)?\s*(?<message>.*)\s+(?<where>in\s+~?\w+(\([^\s]*\))*\s+function\s+at\s+line\s+\d+))");
	profile->SetNewLogEntryStartRegex(R"(^(\d\d)?\d\d-\d\d-\d\d)");
	profile->SetSystemInfoVersionRegex(R"(Version\s*(:|=)?\s*([vV]?(?<version>\d+(\.\d+)*)(?<tags>(-[^-\s]+)*))\s*(\((?<buildnr>\d+)\))?)");
	profile->SetSystemInfoDeviceRegex(R"(Device\s*[:=]\s*(?<device>[\w ,-]+(\([\w ,]+\))?))");
	profile->SetSystemInfoOsRegex(R"((OS|Operating\s*System)\s*[:=]\s*(?<os>(Windows|Android|iOS) [\d\.]+(\s*,\s*SDK\s*\d+)?))");

	return profile;
}

std::shared_ptr<LogProfile> LogProfile::GetDefault()
{
	static std::shared_ptr<LogProfile> defaultProfile = MakeDefault();
	return defaultProfile;
}

void LogProfile::SetLogEntryRegex(const QString &regex)
{
	logEntryRegex = regex;
	Save();
}

void LogProfile::SetNewLogEntryStartRegex(const QString &regex)
{
	newlogEntryStartRegex = regex;
	Save();
}

void LogProfile::SetSystemInfoVersionRegex(const QString &regex)
{
	sysInfoVersionRegex = regex;
	Save();
}

void LogProfile::SetSystemInfoDeviceRegex(const QString &regex)
{
	sysInfoDeviceRegex = regex;
	Save();
}

void LogProfile::SetSystemInfoOsRegex(const QString &regex)
{
	sysInfoOsRegex = regex;
	Save();
}

void LogProfile::SetSystemInfoLinesToCheck(uint32_t linesToCheck)
{
	sysInfoLinesToCheck = linesToCheck;
	Save();
}

void LogProfile::SetLogLevels(const std::vector<std::shared_ptr<LogLevel>>& levels)
{
	logLevels = levels;
	Save();
}

void LogProfile::SetLinesToCheckForDetection(const uint32_t lines)
{
	detectionLinesToCheck = lines;
	Save();
}

void LogProfile::SetLinesToCheckForSystemInformation(const uint32_t lines)
{
	sysInfoLinesToCheck = lines;
	Save();
}
