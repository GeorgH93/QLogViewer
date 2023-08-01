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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "LogViewerTab.h"
#include "SettingsWindow.h"
#include "RecentFiles.h"
#include <QFileDialog>
#include <QFile>
#include <QCoreApplication>
#include <QMimeData>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, settingsWindow(new SettingsWindow(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    setAcceptDrops(true);

	connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::OnTabCurrentChanged);
	connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::OnTabCloseRequested);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::OnActionOpenTriggered);
	connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::OnActionSettingsTriggered);
	connect(ui->actionClearList, &QAction::triggered, [] {RecentFiles::GetInstance().ClearList();});
	connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);
	connect(ui->actionAboutQT, &QAction::triggered, &QApplication::aboutQt);

	RecentFiles::GetInstance().Link(ui->menuRecent, this, ui->menuRecent->actions()[0], [this](const QString& file) { Open(file); });
}

MainWindow::~MainWindow()
{
    ui->tabWidget->clear();
    for(const auto node: logTabs)
        delete node;
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        const QList<QUrl> urlList = mimeData->urls();
        
        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            pathList.append(urlList.at(i).toLocalFile());
        }
        
        Open(pathList);
    }
}

void MainWindow::Open(const QStringList& files)
{
    for(const QString& file : files)
    {
        Open(file);
    }
}

void MainWindow::Open(const QString& filePath)
{
	// Create the tab - it will handle async loading and error display internally
	LogViewerTab* viewerTab = new LogViewerTab(filePath, ui->tabWidget);
	AddTab(viewerTab);
	RecentFiles::GetInstance().Add(filePath);
}

void MainWindow::AddTab(LogViewerTab* viewerTab)
{
    ui->stackedWidget->setCurrentIndex(1); // Switch to tab view
    logTabs.append(viewerTab);
    const int index = ui->tabWidget->addTab(viewerTab, viewerTab->GetTabTitle());
    ui->tabWidget->setCurrentIndex(index);
    ui->tabWidget->setTabToolTip(index, viewerTab->GetTabToolTip());

    // Connect to update tab icon and info when loading completes
    connect(viewerTab, &LogViewerTab::LoadCompleted, this, [this, viewerTab](bool success, const QString& errorMessage) {
        const int tabIndex = ui->tabWidget->indexOf(viewerTab);
        if (tabIndex >= 0)
        {
            ui->tabWidget->setTabIcon(tabIndex, viewerTab->GetTabIcon());
            // Update status bar if this is the current tab
            if (ui->tabWidget->currentIndex() == tabIndex)
            {
                if (success)
                {
                    statusBar()->showMessage(viewerTab->GetSystemInfo());
                }
                else
                {
                    statusBar()->showMessage(tr("Failed to load: %1").arg(errorMessage));
                }
            }
        }
    });
}

void MainWindow::OnTabCurrentChanged(int index)
{
    if (logTabs.isEmpty() || index < 0 || index >= logTabs.count())
    {
        setWindowTitle("QLogViewer");
        statusBar()->clearMessage();
        return;
    }
    statusBar()->showMessage(logTabs.at(index)->GetSystemInfo());
    setWindowTitle("QLogViewer - " + ui->tabWidget->tabText(index));
}

void MainWindow::OnTabCloseRequested(int index)
{
	logTabs[index]->deleteLater();
    logTabs.removeAt(index);
    ui->tabWidget->removeTab(index);
    if(!ui->tabWidget->count())
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::OnActionOpenTriggered()
{
    const QStringList files = QFileDialog::getOpenFileNames(this, tr("Open File"), "./", "Log files (*.log);;Text files (*.txt);;Everything (*)");
    Open(files);
}

void MainWindow::OnActionSettingsTriggered()
{
	if (settingsWindow->isHidden())
	{
		settingsWindow->show();
	}
}

