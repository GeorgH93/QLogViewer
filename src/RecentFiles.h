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
#include <QAction>
#include <QMenu>
#include <list>

class RecentFiles final
{
	struct RecentEntry final
	{
		QString path;
		QAction* action;
	};

	RecentFiles();
	~RecentFiles() = default;

public:
	static RecentFiles& GetInstance();

	void Link(QMenu* menu, QObject* parent, QAction* insertBefore, std::function<void(const QString&)> openCallback);

	void Add(const QString& filePath);

	void ClearList(bool save = true);

private:
	QAction* FindInsertBefore(QAction* skip);

	QAction* MakeAction(const QString& filePath);

	void Load();

	void Save();

	static QString GetHistoryFile();

	std::list<RecentEntry> recentList;

	QObject* parent;
	QMenu* menu;
	QAction* insertBefore;
	std::function<void(const QString&)> openCallback;
};
