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

#include "LogViewer.h"
#include "LogParser.h"
#include "Profiler.hpp"
#include "LineNumberAreaWidget.h"
#include "AppConfig.h"

#include <QPainter>
#include <QTextBlock>
#include <QStringBuilder>

#include <iostream>

LogViewer::LogViewer(QWidget *parent) : InfoAreaEnabledPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberAreaWidget(this);

    //connect(this, &LogViewer::blockCountChanged, this, &LogViewer::SetViewportMargins);
    connect(this, &LogViewer::cursorPositionChanged, this, &LogViewer::HighlightCurrentLine);
    
    HighlightCurrentLine();

    setLineWrapMode(LineWrapMode::NoWrap);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    setFont(font);
}

void LogViewer::SetLogHolder(LogHolder* holder)
{
	logHolder = holder;
	UpdateLogView();
    lineNumberArea->SetWidthForMaxNumber(logHolder->GetMaxLineNumber());
    AddInfoAreaWidget(lineNumberArea);
}

void LogViewer::UpdateLogView()
{
    BlockProfiler profiler("Update log view");
    QString string;
    {
        BlockProfiler buildProfiler("Build log message");
        for (const LogEntry* entry : logHolder->GetFilteredEntries())
        {
			if (!string.isEmpty())
			{
				string.append('\n');
			}
            string.append(entry->message);
        }
    }
    {
        BlockProfiler setProfiler("Set log view");
        setPlainText(string);
    }
}

void LogViewer::HighlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    //if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        selection.format.setBackground(AppConfig::GetInstance()->GetHighlightedLineBackgroundColor());
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
