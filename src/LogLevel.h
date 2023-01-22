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

#include "EditInfoAreaWidgetMetaDescription.h"
#include <QString>
#include <QColor>

class LogLevel
{
	QString levelName;
	QColor fontColor = Qt::black, backgroundColor = Qt::transparent;

	EditInfoAreaWidgetMetaDescription metaDescription = { levelName, fontColor, backgroundColor, Qt::AlignCenter };

public:
	LogLevel(const QString& levelName, const QColor& fontColor = Qt::black, const QColor& bkgColor = Qt::transparent, int align = Qt::AlignCenter)
		: levelName(levelName), fontColor(fontColor), backgroundColor(bkgColor)
	{
		metaDescription.alignment = align;
	}

	const EditInfoAreaWidgetMetaDescription& GetMetaDescription() { return metaDescription; }

	void SetFontColor(const QColor& color) { fontColor = color; }

	void SetBackgroundColor(const QColor& color) { backgroundColor = color; }

	void SetAlignment(int align) { metaDescription.alignment = align; }
};