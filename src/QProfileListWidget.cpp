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

#include <qDebug>
#include <qevent.h>
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
			std::string path = urlList.at(i).toLocalFile().toStdString();
			checkAndImportProfile(path);
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

bool QProfileListWidget::checkAndImportProfile(std::string& path)
{
	if (checkImportFile(path))
	{
		return false;
	}

	const std::shared_ptr<LogProfile> profile = std::make_shared<LogProfile>(path);
	AppConfig::GetInstance()->GetProfiles().push_back(profile);

	auto* listItem = new QListWidgetItem(profile->GetIcon(), profile->GetProfileName());
	insertItem(0, listItem);
	sortItems(Qt::AscendingOrder);
	setCurrentItem(listItem);

	return true;
}

bool QProfileListWidget::checkImportFile(std::string& path)
{
	if (path.find_last_not_of(".yml"))
	{
		return false;
	}
	return true;
}


QProfileListWidget::~QProfileListWidget() = default;
