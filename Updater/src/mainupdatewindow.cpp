/*
**  Ouroboros Updater, Hummingbird.me Desktop App Updater
**  Copyright (C) 2013, Mikunj Varsani
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainupdatewindow.h"
#include "ui_mainupdatewindow.h"
#include "updateperformclass.h"
#include "Globals.h"

#include <QDebug>
#include <QMessageBox>
#include <QProcess>


MainUpdateWindow::MainUpdateWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainUpdateWindow)
{
    ui->setupUi(this);


    UpdatePerformClass *UpdateClass = new UpdatePerformClass(this);

    /* Connect the progressbar and label slots */
    QObject::connect(UpdateClass,SIGNAL(SetProgressValue(int)),ui->DownloadProgress,SLOT(setValue(int)));
    QObject::connect(UpdateClass,SIGNAL(SetProgressMax(int)),ui->DownloadProgress,SLOT(setMaximum(int)));
    QObject::connect(UpdateClass,SIGNAL(SetProgressLabel(QString)),ui->ProgressLabel,SLOT(setText(QString)));

    QObject::connect(UpdateClass,SIGNAL(OpenWarningMessageBox(QString,QString)),this,SLOT(OpenWarningMessageBox(QString,QString)));

    /* Connects the Slots and Signals */
    QObject::connect(UpdateClass,SIGNAL(finished()),this,SLOT(LaunchApp()));

    UpdateClass->PerformUpdate();

}

void MainUpdateWindow::OpenWarningMessageBox(QString Title, QString Text)
{
    QMessageBox::warning(this,Title,Text);
}

void MainUpdateWindow::LaunchApp()
{
    QString AppPath = QApplication::applicationDirPath() + QDir::separator() + MAIN_APP_NAME;
    QStringList Args;
    Args << "-r";
    QProcess::startDetached(AppPath,Args);
    QApplication::exit();
}

MainUpdateWindow::~MainUpdateWindow()
{
    delete ui;
}

void MainUpdateWindow::on_SkipUpdaterButton_clicked()
{
    LaunchApp();
}
