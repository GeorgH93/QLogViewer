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

#include "RecentFiles.h"
#include "AppConfig.h"

#include <utility>
#include <QFile>
#include <QTextStream>

RecentFiles &RecentFiles::GetInstance()
{
	static RecentFiles recent;
	return recent;
}

RecentFiles::RecentFiles()
	: parent(nullptr), menu(nullptr), insertBefore(nullptr)
{
}

void RecentFiles::Link(QMenu* menu, QObject* parent, QAction* insertBefore, std::function<void(const QString&)> openCallback)
{
	ClearList(false);
	Load();

	this->menu = menu;
	this->parent = parent;
	this->insertBefore = insertBefore;
	this->openCallback = std::move(openCallback);

	QList<QAction*> actions;
	for(auto& e : recentList)
	{
		e.action = MakeAction(e.path);
		actions.push_back(e.action);
	}
	menu->insertActions(insertBefore, actions);
}

void RecentFiles::Add(const QString& filePath)
{
	for(auto iterator = recentList.begin(); iterator != recentList.end(); iterator++)
	{
		if (iterator->path == filePath)
		{
			RecentEntry entry = *iterator;
			recentList.erase(iterator);
			recentList.push_front(entry);
			if (entry.action)
			{
				menu->removeAction(entry.action);
				menu->insertAction(FindInsertBefore(entry.action), entry.action);
			}
			return;
		}
	}
	while (recentList.size() >= AppConfig::GetInstance()->GetFilesToKeepInHistory())
	{
		RecentEntry& end = recentList.back();
		if (end.action)
		{
			menu->removeAction(end.action);
			delete(end.action); // TODO do i need this?
		}
		recentList.pop_back();
	}
	recentList.push_front({filePath, nullptr});
	if (parent)
	{
		RecentEntry& entry = recentList.front();
		entry.action = MakeAction(entry.path);
		menu->insertAction(FindInsertBefore(entry.action), entry.action);
	}
	Save();
}

QAction *RecentFiles::FindInsertBefore(QAction* skip)
{
	for (const RecentEntry& e : recentList)
	{
		if (e.action && e.action != skip)
		{
			return e.action;
		}
	}
	return insertBefore;
}

void RecentFiles::ClearList(bool save)
{
	if (menu)
	{
		for (const RecentEntry &e: recentList)
		{
			if (e.action)
			{
				menu->removeAction(e.action);
				delete e.action;
			}
		}
	}
	recentList.clear();
	if (save) Save();
}

QAction *RecentFiles::MakeAction(const QString &filePath)
{
	QAction* action = new QAction(parent);
	action->setText(filePath);
	QObject::connect(action, &QAction::triggered, [this, filePath]{
		openCallback(filePath);
	});
	return action;
}

void RecentFiles::Load()
{
	QFile file(GetHistoryFile());
	if (file.exists())
	{
		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream in(&file);
			while (!in.atEnd())
			{
				QString line = in.readLine();
				if (line.length() < 3) continue;
				recentList.push_back({line, nullptr});
			}
			file.close();
		}
	}
}

void RecentFiles::Save()
{
	QFile file(GetHistoryFile());
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		for (const auto& e : recentList)
		{
			out << e.path << '\n';
		}
		file.close();
	}
}

QString RecentFiles::GetHistoryFile()
{
	return AppConfig::GetAppDataLocation() + "history.txt";
}
