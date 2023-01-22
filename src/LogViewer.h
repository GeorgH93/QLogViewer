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
#include <QStatusBar>
#include <QPainter>
#include <QStringBuilder>
#include "LogParser.h"
#include "LogHolder.h"

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberAreaWidget;
class MetaDataArea;

class LogViewer : public InfoAreaEnabledPlainTextEdit
{
    Q_OBJECT

public:
    LogViewer(QWidget *parent = nullptr);
    
    void SetLogHolder(LogHolder* holder);
        
private slots:
    void HighlightCurrentLine();

    void UpdateLogView();
    
private:
    LineNumberAreaWidget* lineNumberArea;

    LogHolder* logHolder;
};
