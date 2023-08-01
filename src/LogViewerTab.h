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

#include "ui_LogViewerTab.h"
#include "LogHolder.h"
#include <QIcon>
#include <QFutureWatcher>
#include <memory>

class LogViewer;

struct LoadResult
{
	bool success = false;
	QString errorMessage;
	QString logContent;
	LogHolder logHolder;
	QString systemInfo;
	QIcon tabIcon;
};

class LogViewerTab final : public QSplitter
{
	Q_OBJECT

public:
	LogViewerTab(const QString& filePath, QWidget *parent = nullptr);

	~LogViewerTab() override;

	[[nodiscard]] inline const QString& GetTabTitle() const { return tabTitle; }

	[[nodiscard]] inline const QString& GetTabToolTip() const { return tabToolTip; }

	[[nodiscard]] inline const QString& GetFileName() const { return fileName; }

	[[nodiscard]] inline const QIcon& GetTabIcon() const { return tabIcon; }

	[[nodiscard]] inline const QString& GetSystemInfo() const { return systemInfo; }

	[[nodiscard]] bool IsLoading() const { return isLoading; }

signals:
	void LoadCompleted(bool success, const QString& errorMessage);

private slots:
	void OnSelectedLineChange() const;

	void OnLoadFinished();

private:
	void SetupUI();

	void DisplayLoadedContent(const LoadResult& result);

	void HighlightCurrentLineInFullView() const;

	static LoadResult LoadFileAsync(const QString& filePath);

	Ui::LogViewerTabClass ui;

	QString tabTitle, tabToolTip, fileName, systemInfo;

	QIcon tabIcon;

	LogHolder logHolder;

	QFutureWatcher<LoadResult>* loadWatcher = nullptr;
	bool isLoading = false;
};
