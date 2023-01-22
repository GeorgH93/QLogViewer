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

#include "InfoAreaEnabledPlainTextEdit.h"
#include "EditInfoAreaWidget.h"

InfoAreaEnabledPlainTextEdit::InfoAreaEnabledPlainTextEdit(QWidget* parent)
{
    connect(this, &InfoAreaEnabledPlainTextEdit::updateRequest, this, &InfoAreaEnabledPlainTextEdit::UpdateMetaInfoSidebar);
}

void InfoAreaEnabledPlainTextEdit::AddInfoAreaWidget(EditInfoAreaWidget* infoWidget)
{
    infoAreaWidgets.append(infoWidget);
    infoWidget->SetOnWidthChangedEvent([this] { SetViewportMargins(0); });
    SetViewportMargins();
}

void InfoAreaEnabledPlainTextEdit::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    int left = cr.left();
    for (EditInfoAreaWidget* widget : infoAreaWidgets)
    {
        widget->setGeometry(QRect(left, cr.top(), widget->GetAreaWidth(), cr.height()));
        left += widget->GetAreaWidth();
    }
}

void InfoAreaEnabledPlainTextEdit::SetViewportMargins(int)
{
    int margin = 0;
    for (const EditInfoAreaWidget* widget : infoAreaWidgets)
    {
        margin += widget->GetAreaWidth();
    }
    setViewportMargins(margin, 0, 0, 0);
}

void InfoAreaEnabledPlainTextEdit::UpdateMetaInfoSidebar(const QRect& rect, int dy)
{
    int margin = 0;
    for (EditInfoAreaWidget* widget : infoAreaWidgets)
    {
        if (dy)
        {
            widget->scroll(0, dy);
        }
        else
        {
            widget->update(0, rect.y(), widget->width(), rect.height());
        }

        margin += widget->GetAreaWidth();
    }

    if (rect.contains(viewport()->rect()))
    {
        setViewportMargins(margin, 0, 0, 0);
    }
}
