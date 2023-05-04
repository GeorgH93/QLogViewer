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

SettingsWindow::SettingsWindow(QWidget *parent)
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
		ui.profilesListWidget->sortItems(Qt::AscendingOrder);
		ui.profilesListWidget->setCurrentItem(ui.profilesListWidget->item(0));
		on_profilesListWidget_currentRowChanged(ui.profilesListWidget->currentRow());
	}
}

void SettingsWindow::on_profilesListWidget_currentRowChanged(int currentRow)
{
	qDebug() << "Profile selection has changed";
	on_profilesListWidget_itemChanged(ui.profilesListWidget->item(currentRow));
}

void SettingsWindow::on_profilesListWidget_itemChanged(QListWidgetItem* item)
{
	ui.profileNameBox->setPlainText(item->text());
	ui.profilePriorityBox->setPlainText("-");
}

void SettingsWindow::on_addProfileButton_clicked()
{
	qDebug() << "Creating new profile...";
	LogProfile* profile = new LogProfile("Placeholder Name", {}, 0);
	ui.profilesListWidget->insertItem(0, new QListWidgetItem({}, profile->GetProfileName()));
}

void SettingsWindow::on_removeProfileButton_clicked()
{
	qDebug() << "Removing profile at row " << ui.profilesListWidget->currentRow() << " ...";
	ui.profilesListWidget->takeItem(ui.profilesListWidget->currentRow());
}

void SettingsWindow::on_profileNameBox_textChanged()
{
	const QString& profileName = ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->text();
	for (const auto& profile : config->GetProfiles())
	{
		if (profile->GetProfileName() == profileName)
		{
			const auto& position = ui.profileNameBox->textCursor();
			profile->SetProfileName(ui.profileNameBox->toPlainText());
			ui.profilesListWidget->item(ui.profilesListWidget->currentRow())->setText(profile->GetProfileName());
			ui.profileNameBox->setTextCursor(position);
			break;
		}
	}
	// TODO: This works for now and saves the changes with every keystroke. Because the file then is directly persisted, it should be changed.
	//		 There should be an exit event, so changes are only persisted when exiting the field.
	// TODO: Extract profile finding into a method (possibly of LogProfile)
}

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::on_cbCOW_stateChanged(int state)
{
	config->SetCopyOnWrite(state == Qt::CheckState::Checked);
}
