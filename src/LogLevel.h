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

class LogLevel final
{
	QString levelName;
	QColor fontColor = Qt::black, backgroundColor = Qt::transparent;

	EditInfoAreaWidgetMetaDescription metaDescription = { levelName, fontColor, backgroundColor, Qt::AlignCenter };

public:
	LogLevel(const QString& levelName = "", const QColor& fontColor = Qt::black, const QColor& bkgColor = Qt::transparent, int align = Qt::AlignCenter)
		: levelName(levelName), fontColor(fontColor), backgroundColor(bkgColor)
	{
		metaDescription.alignment = align;
	}

	LogLevel(const LogLevel& old)
		: levelName(old.levelName), fontColor(old.fontColor), backgroundColor(old.backgroundColor)
	{
		metaDescription.alignment = old.GetAlignment();
	}

	LogLevel(LogLevel&& old) noexcept
		: levelName(std::move(old.levelName)), fontColor(std::move(old.fontColor)), backgroundColor(std::move(old.backgroundColor))
	{
		metaDescription.alignment = old.GetAlignment();
	}

	~LogLevel() = default;

	const EditInfoAreaWidgetMetaDescription& GetMetaDescription() { return metaDescription; }

	void SetLevelName(const QString& name) { levelName = name; }

	void SetFontColor(const QColor& color) { if (color.isValid()) fontColor = color; }

	void SetBackgroundColor(const QColor& color) { if (color.isValid()) backgroundColor = color; }

	void SetAlignment(int align) { metaDescription.alignment = align; }

	[[nodiscard]] const QString& GetLevelName() const { return levelName; }

	[[nodiscard]] const QColor& GetFontColor() const { return fontColor; }

	[[nodiscard]] const QColor& GetBackgroundColor() const { return backgroundColor; }

	[[nodiscard]] int GetAlignment() const { return metaDescription.alignment; }
	
	LogLevel& operator =(const LogLevel& right)
	{
		levelName = right.levelName;
		fontColor = right.fontColor;
		backgroundColor = right.backgroundColor;
		metaDescription.alignment = right.metaDescription.alignment;
		return *this;
	}

	LogLevel& operator =(LogLevel&& right) noexcept
	{
		levelName = std::move(right.levelName);
		fontColor = std::move(right.fontColor);
		backgroundColor = std::move(right.backgroundColor);
		metaDescription.alignment = right.metaDescription.alignment;
		return *this;
	}
};
