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
	void on_profileSaveButton_clicked();
	void on_addLogLevelButton_clicked();
	void on_removeLogLevelButton_clicked();

	void on_profilesListWidget_currentRowChanged(int currentRow);
	void on_logLevelTable_cellClicked(int row, int column);


private:
	int LOG_LEVEL_COLUMN = 0;
	int FONT_COLOR_COLUMN = 1;
	int BACKGROUND_COLOR_COLUMN = 2;

	void LoadTabGeneral();
	void LoadTabProfiles();

	void SetAllTextBoxes(const std::shared_ptr<LogProfile>& profile);
	void FillColorCell(const int row, const int column, const QColor& color);
	void ClearAllFields();
	void SaveToProfile(std::shared_ptr<LogProfile>& profile);


	bool IsProfileNameUnique();
	bool IsProfileNameEqualToCurrentListItem();


	Ui::SettingsWindowClass ui;
	AppConfig* config;
};
