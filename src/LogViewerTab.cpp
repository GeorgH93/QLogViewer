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
#include "LogProfile.h"
#include "AppConfig.h"
#include "Profiler.hpp"
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QtConcurrent>
#include <QMessageBox>

LogViewerTab::LogViewerTab(const QString& filePath, QWidget *parent)
	: QSplitter(parent), tabToolTip(filePath), fileName(filePath)
{
	SetupUI();

	const QFileInfo fileInfo(filePath);
	tabTitle = fileInfo.fileName();

	// Start async loading
	isLoading = true;
	loadWatcher = new QFutureWatcher<LoadResult>(this);
	connect(loadWatcher, &QFutureWatcher<LoadResult>::finished, this, &LogViewerTab::OnLoadFinished);

	// Show loading state in the viewer
	ui.logViewer->setPlainText(tr("Loading file: %1...").arg(tabTitle));

	// Start the async load
	QFuture<LoadResult> future = QtConcurrent::run(&LogViewerTab::LoadFileAsync, filePath);
	loadWatcher->setFuture(future);
}

LogViewerTab::~LogViewerTab()
{
	if (loadWatcher && loadWatcher->isRunning())
	{
		loadWatcher->cancel();
		loadWatcher->waitForFinished();
	}
	ui.logViewer = nullptr;
}

void LogViewerTab::SetupUI()
{
	ui.setupUi(this);

	// Prevent collapsing the main log view
	const auto mainView = ui.mainViewSplitter;
	setCollapsible(this->indexOf(mainView), false);
	mainView->setCollapsible(mainView->indexOf(ui.logViewer), false);

	connect(ui.logViewer, &LogViewer::cursorPositionChanged, this, &LogViewerTab::OnSelectedLineChange);
}

LoadResult LogViewerTab::LoadFileAsync(const QString& filePath)
{
	LoadResult result;

	QFile file(filePath);
	if (!file.exists())
	{
		result.success = false;
		result.errorMessage = QObject::tr("File does not exist: %1").arg(filePath);
		return result;
	}

	if (!file.open(QIODevice::ReadOnly))
	{
		result.success = false;
		result.errorMessage = QObject::tr("Cannot open file: %1\nError: %2")
			.arg(filePath).arg(file.errorString());
		return result;
	}

	// Read file content
	result.logContent = QString(file.readAll());
	file.close();

	// Parse the log
	result.logHolder.Load(result.logContent);
	result.systemInfo = result.logHolder.GetSystemInfo();
	result.tabIcon = result.logHolder.GetLogProfile()->GetIcon();
	result.success = true;

	return result;
}

void LogViewerTab::OnLoadFinished()
{
	if (!loadWatcher)
	{
		return;
	}

	LoadResult result = loadWatcher->result();
	isLoading = false;

	DisplayLoadedContent(result);

	emit LoadCompleted(result.success, result.errorMessage);
}

void LogViewerTab::DisplayLoadedContent(const LoadResult& result)
{
	if (!result.success)
	{
		ui.logViewer->setPlainText(tr("Error loading file:\n%1").arg(result.errorMessage));
		return;
	}

	// Store the loaded data
	logHolder = std::move(const_cast<LoadResult&>(result).logHolder);
	systemInfo = result.systemInfo;
	tabIcon = result.tabIcon;

	// Display full log in the full view
	ui.fullLogView->setPlainText(result.logContent);

	// Apply default filter (show all entries)
	logHolder.Filter([](const LogEntry&) -> bool { return true; });

	// Set up the filtered view
	ui.logViewer->SetLogHolder(&logHolder);
}

void LogViewerTab::OnSelectedLineChange() const
{
	if (isLoading)
	{
		return;
	}

	const auto& entries = logHolder.GetFilteredEntries();
	if (entries.empty()) return;
	const auto textCursor = ui.logViewer->textCursor();
	const auto blockNr = static_cast<size_t>(textCursor.blockNumber());
	const auto lineNumber = entries[std::min(blockNr, entries.size() - 1)]->lineNumber;
	QTextCursor cursor = ui.fullLogView->textCursor();
	cursor.movePosition(QTextCursor::Start);
	if (lineNumber > 0)
	{
		cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1);
	}
	ui.fullLogView->setTextCursor(cursor);
	ui.fullLogView->centerCursor();

	// Highlight current line in full view
	HighlightCurrentLineInFullView();
}

void LogViewerTab::HighlightCurrentLineInFullView() const
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	QTextEdit::ExtraSelection selection;

	selection.format.setBackground(AppConfig::GetInstance()->GetHighlightedLineBackgroundColor());
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = ui.fullLogView->textCursor();
	selection.cursor.clearSelection();
	extraSelections.append(selection);

	ui.fullLogView->setExtraSelections(extraSelections);
}
