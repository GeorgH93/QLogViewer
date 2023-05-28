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

#include "QProfileListWidget.h"

#include <filesystem>
#include <qDebug>
#include <qevent.h>
#include <QMessageBox>
#include <QMimeData>

#include "AppConfig.h"
#include "LogProfile.h"

QProfileListWidget::QProfileListWidget(QWidget* parent)
	: QListWidget(parent)
{
}

void QProfileListWidget::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (mimeData->hasUrls())
	{
		const QList<QUrl> urlList = mimeData->urls();

		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			checkAndImportProfile(urlList.at(i));
		}
	}
}

void QProfileListWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept();
}

void QProfileListWidget::dragMoveEvent(QDragMoveEvent* event)
{
}

bool QProfileListWidget::checkAndImportProfile(const QUrl& path)
{
	if (!isImportable(path))
	{
		return false;
	}

	const std::shared_ptr<LogProfile> profile = std::make_shared<LogProfile>(path.toLocalFile().toStdString());
	AppConfig::GetInstance()->GetProfiles().push_back(profile);

	auto* listItem = new QListWidgetItem(profile->GetIcon(), profile->GetProfileName());
	insertItem(0, listItem);
	sortItems(Qt::AscendingOrder);
	setCurrentItem(listItem);

	return true;
}

bool QProfileListWidget::exportProfile(std::string& path)
{
	std::shared_ptr<LogProfile> profile = AppConfig::GetInstance()->GetProfileForName(item(currentRow())->text());
	path = path + "/" + profile->GetFileName();

	if (std::filesystem::exists(path))
	{
		const int result = QMessageBox::question(this, "File already exists",
			"A file already exists to where you want to export the profile.\nDo you want to overwrite it?",
			QMessageBox::Yes, QMessageBox::No);
		if (result == QMessageBox::Yes)
		{
			return copy_file(profile->GetFilepath(), path, std::filesystem::copy_options::overwrite_existing);
		}
		return false;
	}

	return std::filesystem::copy_file(profile->GetFilepath(), path);
}

bool QProfileListWidget::isImportable(const QUrl& path)
{
	if (!path.toLocalFile().endsWith(".yml"))
	{
		return false;
	}
	const QString profileName = path.fileName().remove(".yml");
	auto profile = AppConfig::GetInstance()->GetProfileForName(profileName);
	if (profile)
	{
		const int result = QMessageBox::question(this, "Profile already exists", 
			"\"" + profile->GetProfileName() + "\" already exists. Do you want to replace the exiting profile?",
			QMessageBox::Yes, QMessageBox::No);
		if (result == QMessageBox::Yes)
		{
			takeItem(row(findItems(profileName, Qt::MatchExactly)[0]));
			AppConfig::GetInstance()->DeleteProfile(profile);
			return true;
		}
		return false;
	}

	return true;
}


QProfileListWidget::~QProfileListWidget() = default;
