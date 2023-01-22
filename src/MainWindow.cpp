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
#include "ui_mainwindow.h"
#include "LogViewerTab.h"
#include <QFileDialog>
#include <QFile>
#include <QCoreApplication>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidget->clear();
    setAcceptDrops(true);
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
{ //TODO run async
    QFile file(filePath);
    LogViewerTab* viewerTab = new LogViewerTab(&file, this);
    AddTab(viewerTab);
}

void MainWindow::AddTab(LogViewerTab* viewerTab)
{
    ui->stackedWidget->setCurrentIndex(1); // Switch to tab view
    logTabs.append(viewerTab);
    ui->tabWidget->addTab(viewerTab, viewerTab->GetTabTitle());
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    const int index = ui->tabWidget->currentIndex();
    ui->tabWidget->setTabToolTip(index, viewerTab->GetTabToolTip());
    ui->tabWidget->setTabIcon(index, viewerTab->GetTabIcon());
}

void MainWindow::on_tabWidget_currentChanged(int index)
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

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    logTabs.removeAt(index);
    ui->tabWidget->removeTab(index);
    if(!ui->tabWidget->count())
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    const QStringList files = QFileDialog::getOpenFileNames(this, tr("Open File"), "./");
    Open(files);
}

void MainWindow::on_actionQuit_triggered()
{
    QCoreApplication::quit();
}


