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

#include "LogViewerTab.h"
#include "LogViewer.h"
#include "LogParser.h"
#include <QFile>
#include <QFileInfo>

LogViewerTab::LogViewerTab(QFile* file, QWidget *parent)
	: QSplitter(parent), tabToolTip(file->fileName()), fileName(file->fileName())
{
	ui.setupUi(this);

	const QFileInfo fileInfo(file->fileName());
	tabTitle = fileInfo.fileName();

	Load(file);

	// Prevent collapsing the main log view
	const auto mainView = ui.mainViewSplitter;
	setCollapsible(this->indexOf(mainView), false);
	mainView->setCollapsible(mainView->indexOf(ui.logViewer), false);

	connect(ui.logViewer, &LogViewer::cursorPositionChanged, this, &LogViewerTab::OnSelectedLineChange);
}

LogViewerTab::~LogViewerTab()
{
	ui.logViewer = nullptr;
}

void LogViewerTab::OnSelectedLineChange() const
{
	const auto textCursor = ui.logViewer->textCursor();
	const auto& entries = logHolder.GetFilteredEntries();
	const auto lineNumber = entries[std::min(static_cast<size_t>(textCursor.blockNumber()), entries.size() - 1)]->lineNumber;
	QTextCursor cursor = ui.fullLogView->textCursor();
	cursor.movePosition(QTextCursor::Start);
	if (lineNumber > 0)
	{
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1);
	}
	ui.fullLogView->setTextCursor(cursor);
	ui.fullLogView->centerCursor();

	if (true) //TODO check if highlighting is enabled
	{
		HighlightCurrentLineInFullView();
	}
}

void LogViewerTab::HighlightCurrentLineInFullView() const
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	
	QTextEdit::ExtraSelection selection;

	const QColor lineColor = QColor(Qt::yellow).lighter(160);

	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = ui.fullLogView->textCursor();
	selection.cursor.clearSelection();
	extraSelections.append(selection);

	ui.fullLogView->setExtraSelections(extraSelections);
}

void LogViewerTab::Load(QFile* file)
{
	if (!file->open(QIODevice::ReadOnly))
	{
		// TODO handle error
		return;
	}
	QString log;
	{
		BlockProfiler profileLoadFile("Load log file to RAM");
		log = QString(file->readAll());
	}
	file->close();
	{
		BlockProfiler profilerSetFullLog("Set full log view");
		ui.fullLogView->setPlainText(log);
	}
	
	logHolder.Load(&log);
	logHolder.Filter([](auto) -> bool { return true; }); //TODO
	systemInfo = logHolder.GetSystemInfo();
	
	ui.logViewer->SetLogHolder(&logHolder);
	
}

