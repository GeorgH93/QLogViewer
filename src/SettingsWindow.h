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

#include "AppConfig.h"
#include <QMainWindow>
#include "ui_SettingsWindow.h"

class SettingsWindow final : public QMainWindow
{
	Q_OBJECT

public:
	SettingsWindow(QWidget *parent = nullptr);
	~SettingsWindow() override;

private slots:
	void on_cbCOW_stateChanged(int state);
	void on_addProfileButton_clicked();
	void on_removeProfileButton_clicked();
	void on_profilesListWidget_currentRowChanged(int currentRow);
	void on_profilesListWidget_itemChanged(QListWidgetItem* item);

	void on_profileNameBox_textChanged();
	void on_profileNameBox_focusOutEvent(QFocusEvent* e);

private:
	void LoadTabGeneral();
	void LoadTabProfiles();

	Ui::SettingsWindowClass ui;
	AppConfig* config;
};
