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

#include "InfoAreaEnabledPlainTextEdit.h"
#include <QWidget>
#include <QString>
#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>


struct MetaDescription
{
    QString& text;
    QColor& fontColor;
    QColor& fontBackgroundColor;
    int alignment = Qt::AlignLeft;
};

class EditInfoAreaWidget : public QWidget
{
    QColor areaBackgroundColor = Qt::white;
    std::function<void()> onAreaWidthChanged = [](){};

public:
    EditInfoAreaWidget(InfoAreaEnabledPlainTextEdit* editor, const std::function<const MetaDescription&(int)>& metaDescriptionProvider, int marginLeft = 3, int marginRight = 3) :
        QWidget(editor), editor(editor), areaWidth(0), areaMarginLeft(marginLeft), areaMarginRight(marginRight), metaDescriptionProvider(metaDescriptionProvider)
    {}

    QSize sizeHint() const override
    {
        return {areaWidth, 0};
    }

    virtual char GetWidthCalculationChar()
    {
        return 'W';
    }

    inline void SetWidthForCharCount(const int charCount)
    {
        areaWidth = areaMarginLeft + fontMetrics().horizontalAdvance(QLatin1Char(GetWidthCalculationChar())) * charCount + areaMarginRight;
        onAreaWidthChanged();
    }

    inline int GetAreaWidth() const { return areaWidth; }

    inline void SetBackgroundColor(const QColor& color)
    {
        areaBackgroundColor = color;
    }

    void SetOnWidthChangedEvent(const std::function<void()>& eventHandler)
    {
        onAreaWidthChanged = eventHandler;
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.fillRect(event->rect(), areaBackgroundColor);

        QTextBlock block = editor->firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = qRound(editor->blockBoundingGeometry(block).translated(editor->contentOffset()).top());
        int bottom = top + qRound(editor->blockBoundingRect(block).height());

        while (block.isValid() && top <= event->rect().bottom())
        {
            if (block.isVisible() && bottom >= event->rect().top()) {
                MetaDescription metaDescription = metaDescriptionProvider(blockNumber);
                painter.setPen(metaDescription.fontColor);
                painter.setBackground(metaDescription.fontBackgroundColor);
                painter.drawText(0, top, width() - areaMarginRight, fontMetrics().height(), metaDescription.alignment, metaDescription.text);
            }

            block = block.next();
            top = bottom;
            bottom = top + qRound(editor->blockBoundingRect(block).height());
            ++blockNumber;
        }
    }

private:
    InfoAreaEnabledPlainTextEdit* editor;
    std::function<MetaDescription(int)> metaDescriptionProvider;
    int areaWidth, areaMarginLeft, areaMarginRight;
};