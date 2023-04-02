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

#include "AppConfig.h"

#include "YamlConverters.h"
#include "LogProfile.h"
#include <QStandardPaths>
#include <QDir>
#include <yaml-cpp/yaml.h>

AppConfig::AppConfig()
{
	Load();
}

void AppConfig::Load()
{
	const QString& appDataDir = GetAppDataLocation();
	QDir().mkpath(appDataDir); // Ensure app config dir exists
	QDir().mkpath(GetProfilesLocation()); // Ensure app config dir exists

	LoadProfiles();
}

void AppConfig::LoadProfiles()
{
	QDir profileDir(GetProfilesLocation());
	QStringList profilePathList = profileDir.entryList(QStringList() << "*.yml", QDir::Files);
	for(const QString& profilePath : profilePathList)
	{
		profiles.push_back(std::make_shared<LogProfile>(profilePath.toStdString()));
	}
	std::sort(profiles.begin(), profiles.end(), [](const std::shared_ptr<LogProfile>& left, const std::shared_ptr<LogProfile>& right) { return left->GetPriority() > right->GetPriority(); });
}

const QString& AppConfig::GetAppDataLocation()
{
	static const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/QLogViewer/";
	return appDataPath;
}

const QString& AppConfig::GetProfilesLocation()
{
	static const QString profilesDir = GetAppDataLocation() + "Profiles/";
	return profilesDir;
}

AppConfig* AppConfig::GetInstance()
{
	static AppConfig config;
	return &config;
}

void AppConfig::Save()
{

}

std::shared_ptr<LogProfile> AppConfig::FindProfile(const QString& logLine) const
{
	for(const auto& profile : profiles)
	{
		if (profile->IsProfile(logLine))
		{
			return profile;
		}
	}
	return nullptr;
}

void AppConfig::AddProfile(const std::shared_ptr<LogProfile>& profile)
{
	profiles.push_back(profile);
}

