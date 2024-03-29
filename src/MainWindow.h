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

#include <QMainWindow>
#include <QList>

class LogViewerTab;
class SettingsWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void Open(const QStringList& files);

    void Open(const QString& file);

    void AddTab(LogViewerTab* viewerTab);

private slots:
    void OnTabCloseRequested(int index);

    void OnTabCurrentChanged(int index);

    void OnActionOpenTriggered();

	void OnActionSettingsTriggered();

    void dropEvent(QDropEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;

    void OpenSearchTab();

private:
    Ui::MainWindow *ui;
    QList<LogViewerTab*> logTabs;
	SettingsWindow *settingsWindow;
};
