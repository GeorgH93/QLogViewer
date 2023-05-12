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

#include "SettingsWindow.h"
#include "LogProfile.h"
#include "LogLevel.h"

#include <QDebug>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QColorDialog>

#include <vector>


SettingsWindow::SettingsWindow(QWidget* parent)
	: QMainWindow(parent), config(AppConfig::GetInstance())
{
	ui.setupUi(this);
	LoadTabGeneral();
	LoadTabProfiles();
}

void SettingsWindow::LoadTabGeneral()
{
	ui.cbCOW->setCheckState(config->UseCopyOnWriteEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void SettingsWindow::LoadTabProfiles()
{
	for (const auto& profile : config->GetProfiles())
	{
		ui.profilesListWidget->insertItem(0, new QListWidgetItem(profile->GetIcon(), profile->GetProfileName()));
	}

	ui.profilesListWidget->sortItems(Qt::AscendingOrder);
	ui.profilesListWidget->setCurrentItem(ui.profilesListWidget->item(0));
	on_profilesListWidget_currentRowChanged(ui.profilesListWidget->currentRow());
}

void SettingsWindow::on_profilesListWidget_currentRowChanged(int currentRow)
{
	qDebug() << "Profile selection has changed";
	QListWidgetItem* item = ui.profilesListWidget->item(currentRow);
	const std::shared_ptr<LogProfile> profile = AppConfig::GetInstance()->GetProfileForName(item->text());

	if (profile->GetProfileName() == item->text())
	{
		SetAllTextBoxes(profile);
	}
	else
	{
		ClearAllFields();
		ui.profileNameBox->setPlainText(item->text());
	}
}

void SettingsWindow::on_logLevelTable_cellClicked(int row, int column)
{
	if (column == LOG_LEVEL_COLUMN)
	{
		return;
	}

	QColor color = QColorDialog::getColor(Qt::green, this);
	if (color.isValid())
	{
		ui.logLevelTable->setItem(row, column, new  QTableWidgetItem(color.name()));
		ui.logLevelTable->item(row, column)->setBackgroundColor(color);
	}
}

void SettingsWindow::SetAllTextBoxes(const std::shared_ptr<LogProfile>& profile)
{
	// General
	ui.profileNameBox->setPlainText(profile->GetProfileName());
	ui.profilePriorityBox->setPlainText(QString::number(profile->GetPriority()));
	ui.profileFilepathLabel->setText(QString::fromStdString(profile->GetFilepath()));

	// Patterns
	ui.profileLogEntryBox->setPlainText(profile->GetLogEntryRegex());
	ui.profileNewLogEntryEntryBox->setPlainText(profile->GetNewLogEntryStartRegex());
	ui.detectionLinesBox->setPlainText(QString::number(profile->GetLinesToCheckForDetection()));
	ui.profileSystemInfoVersionBox->setPlainText(profile->GetSystemInfoVersionRegex());
	ui.profileSystemInfoDeviceBox->setPlainText(profile->GetSystemInfoDeviceRegex());
	ui.profileSystemInfoOsBox->setPlainText(profile->GetSystemInfoOsRegex());
	ui.systemInfoLinesBox->setPlainText(QString::number(profile->GetSystemInfoLinesToCheck()));

    // Log Levels
	ui.logLevelTable->setRowCount(profile->GetLogLevels().size());
	int row = 0;
	for (const std::shared_ptr<LogLevel>& level : profile->GetLogLevels())
	{
		ui.logLevelTable->setItem(row, LOG_LEVEL_COLUMN, new QTableWidgetItem(level->GetLevelName()));
		FillColorCell(row, FONT_COLOR_COLUMN, level->GetFontColor());
		FillColorCell(row, BACKGROUND_COLOR_COLUMN, level->GetBackgroundColor());
		row++;
	}
}

void SettingsWindow::FillColorCell(const int row, const int column, const QColor& color)
{
	ui.logLevelTable->setItem(row, column, new QTableWidgetItem(color.name()));
	ui.logLevelTable->item(row, column)->setBackground(color);
	ui.logLevelTable->item(row, column)->setFlags(ui.logLevelTable->item(row, column)->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
}

void SettingsWindow::ClearAllFields()
{
	// General
	ui.profileNameBox->clear();
	ui.profilePriorityBox->clear();
	ui.profileFilepathLabel->clear();

	// Patterns
	ui.profileLogEntryBox->clear();
	ui.profileNewLogEntryEntryBox->clear();
	ui.detectionLinesBox->clear();
	ui.profileSystemInfoVersionBox->clear();
	ui.profileSystemInfoDeviceBox->clear();
	ui.profileSystemInfoOsBox->clear();
	ui.systemInfoLinesBox->clear();

	// Log Levels
	ui.logLevelTable->clearContents();
}

void SettingsWindow::SaveToProfile(const std::shared_ptr<LogProfile>& profile)
{
	// TODO: When new profile was created trying to save it leads into a questionable error. The encoded name is nothing like the original
	// General
	profile->SetProfileName(ui.profileNameBox->toPlainText());
	profile->SetPriority(ui.profilePriorityBox->toPlainText().toUInt());

	// Patterns
	profile->SetLogEntryRegex(ui.profileLogEntryBox->toPlainText());
	profile->SetNewLogEntryStartRegex(ui.profileNewLogEntryEntryBox->toPlainText());
	profile->SetLinesToCheckForDetection(ui.detectionLinesBox->toPlainText().toUInt());
	profile->SetSystemInfoVersionRegex(ui.profileSystemInfoVersionBox->toPlainText());
	profile->SetSystemInfoDeviceRegex(ui.profileSystemInfoDeviceBox->toPlainText());
	profile->SetSystemInfoOsRegex(ui.profileSystemInfoOsBox->toPlainText());
	profile->SetLinesToCheckForSystemInformation(ui.systemInfoLinesBox->toPlainText().toUInt());

	// Log Levels
	for (int row = 0; row < ui.logLevelTable->rowCount(); row++)
	{
		const std::shared_ptr<LogLevel> level(new LogLevel(
			ui.logLevelTable->item(row, LOG_LEVEL_COLUMN)->text(),
			QColor(ui.logLevelTable->item(row, FONT_COLOR_COLUMN)->text()),
			QColor(ui.logLevelTable->item(row, BACKGROUND_COLOR_COLUMN)->text())
		));
		profile->AddLogLevel(level);
	}
}

void SettingsWindow::on_addProfileButton_clicked()
{
	qDebug() << "Creating new profile...";
	ui.profilesListWidget->insertItem(0, new QListWidgetItem({}, "Placeholder profile name"));
	ui.profilesListWidget->setCurrentItem(ui.profilesListWidget->item(0));
}

void SettingsWindow::on_removeProfileButton_clicked()
{
	qDebug() << "Removing profile at row " << ui.profilesListWidget->currentRow() << " ...";
	// TODO: implement profile deletion, it's  only removed from the list in the UI
	const int result = QMessageBox::question(this, "Confirm deletion", "Do you really want to delete this profile?", QMessageBox::Ok, QMessageBox::Cancel);
	if (result == QMessageBox::Ok)
	{
		ui.profilesListWidget->takeItem(ui.profilesListWidget->currentRow());
	}
}

void SettingsWindow::on_profileSaveButton_clicked()
{
	if (!IsProfileNameUnique() && !IsProfileNameEqualToCurrentListItem())
	{
		qWarning() << "Profile name '" << ui.profileNameBox->toPlainText() << "' is not unique and can't be saved.";
		QMessageBox::warning(this, "Profile exists", "There already exists a profile with this name. Please choose a different one.", QMessageBox::Ok);
		return;
	}

	const QString& profileName = ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->text();
	std::shared_ptr<LogProfile> profile = config->GetProfileForName(profileName);

	if (profile == nullptr)
	{
		profile = std::shared_ptr<LogProfile>();
	}

	SaveToProfile(profile);

	ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->setText(profile->GetProfileName());
	ui.profilesListWidget->sortItems(Qt::AscendingOrder);
}

bool SettingsWindow::IsProfileNameUnique()
{

	return !AppConfig::GetInstance()->GetProfileForName(ui.profileNameBox->toPlainText());
}

bool SettingsWindow::IsProfileNameEqualToCurrentListItem()
{
	return ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->text() == ui.profileNameBox->toPlainText();
}


void SettingsWindow::on_addLogLevelButton_clicked()
{
	qDebug() << "Adding new log level... (wip)";
	const int rowCount = ui.logLevelTable->rowCount() + 1;
	ui.logLevelTable->setRowCount(rowCount);
	FillColorCell(rowCount - 1, FONT_COLOR_COLUMN, Qt::black);
	FillColorCell(rowCount - 1, BACKGROUND_COLOR_COLUMN, Qt::transparent);
}

void SettingsWindow::on_removeLogLevelButton_clicked()
{
	qDebug() << "Removing log level... (wip)";
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::on_cbCOW_stateChanged(int state)
{
	config->SetCopyOnWrite(state == Qt::CheckState::Checked);
}
