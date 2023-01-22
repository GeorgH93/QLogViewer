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

#include "EditInfoAreaWidget.h"

class LineNumberAreaWidget : public EditInfoAreaWidget
{
    QString lineNumberString;
    QColor fontColor;
    QColor fontBackgroundColor;
    MetaDescription description;

public:
    LineNumberAreaWidget(InfoAreaEnabledPlainTextEdit* editor, int marginLeft = 3, int marginRight = 3, int alignment = Qt::AlignRight)
        : EditInfoAreaWidget(editor, std::bind(&LineNumberAreaWidget::GetMetaDescriptionForLine, this, std::placeholders::_1), marginLeft, marginRight)
        , description({ lineNumberString, fontColor, fontBackgroundColor, alignment })
    {
        SetBackgroundColor(Qt::lightGray);
    }

    virtual char GetWidthCalculationChar() override
    {
        return '9';
    }

    void SetWidthForMaxNumber(const uint64_t maxLineNumber)
    {
        int digits = 1;
        uint64_t max = qMax(static_cast<uint64_t>(1), maxLineNumber);
        while (max >= 10)
        {
            max /= 10;
            ++digits;
        }
        SetWidthForCharCount(digits);
    }

    inline void SetFontColor(const QColor& color = Qt::black)
    {
        fontColor = color;
    }

    inline void SetFontBackgroundColor(const QColor& color = Qt::transparent)
    {
        fontBackgroundColor = color;
    }

    inline void SetAlignment(int alignment = Qt::AlignRight)
    {
        description.alignment = alignment;
    }

    const MetaDescription& GetMetaDescriptionForLine(int lineNr)
    {
        lineNumberString = QString::number(lineNr + 1);
        return description;
    }
};