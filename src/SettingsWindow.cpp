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
#include <QDebug>
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

void SettingsWindow::SetAllTextBoxes(const std::shared_ptr<LogProfile>& profile)
{
	// General
	ui.profileNameBox->setPlainText(profile->GetProfileName());
	ui.profilePriorityBox->setPlainText(QString::number(profile->GetPriority()));
	// Patterns
	ui.profileLogEntryBox->setPlainText(profile->GetLogEntryRegex());
	ui.profileNewLogEntryEntryBox->setPlainText(profile->GetNewLogEntryStartRegex());
	ui.profileSystemInfoVersionBox->setPlainText(profile->GetSystemInfoVersionRegex());
	ui.profileSystemInfoDeviceBox->setPlainText(profile->GetSystemInfoDeviceRegex());
	ui.profileSystemInfoOsBox->setPlainText(profile->GetSystemInfoOsRegex());
}

void SettingsWindow::ClearAllFields()
{
	// General
	ui.profileNameBox->clear();
	ui.profilePriorityBox->clear();
	// Patterns
	ui.profileLogEntryBox->clear();
	ui.profileNewLogEntryEntryBox->clear();
	ui.profileSystemInfoVersionBox->clear();
	ui.profileSystemInfoDeviceBox->clear();
	ui.profileSystemInfoOsBox->clear();
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
	ui.profilesListWidget->takeItem(ui.profilesListWidget->currentRow());
}

void SettingsWindow::on_profileSaveButton_clicked()
{
	const QString& profileName = ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->text();
	std::shared_ptr<LogProfile> profile = AppConfig::GetInstance()->GetProfileForName(profileName);

	if (profile == nullptr)
	{
		profile = std::shared_ptr<LogProfile>();
	}

	profile->SetProfileName(ui.profileNameBox->toPlainText());
	profile->SetPriority(ui.profilePriorityBox->toPlainText().toUInt());

	profile->SetLogEntryRegex(ui.profileLogEntryBox->toPlainText());
	profile->SetNewLogEntryStartRegex(ui.profileNewLogEntryEntryBox->toPlainText());
	profile->SetSystemInfoVersionRegex(ui.profileSystemInfoVersionBox->toPlainText());
	profile->SetSystemInfoDeviceRegex(ui.profileSystemInfoDeviceBox->toPlainText());
	profile->SetSystemInfoOsRegex(ui.profileSystemInfoOsBox->toPlainText());

	ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->setText(profile->GetProfileName());
	ui.profilesListWidget->sortItems(Qt::AscendingOrder);
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::on_cbCOW_stateChanged(int state)
{
	config->SetCopyOnWrite(state == Qt::CheckState::Checked);
}
