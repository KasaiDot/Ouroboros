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

#ifndef MAINUPDATEWINDOW_H
#define MAINUPDATEWINDOW_H

#include <QtNetwork>
#include <QtXml>

#include <QMainWindow>

namespace Ui {
class MainUpdateWindow;
}

class MainUpdateWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainUpdateWindow(QWidget *parent = 0);
    ~MainUpdateWindow();

private slots:
    void OpenWarningMessageBox(QString Title,QString Text);
    void LaunchApp();

    void on_SkipUpdaterButton_clicked();

private:
    Ui::MainUpdateWindow *ui;
    QNetworkAccessManager NetworkManager;

    QString Version;
    QStringList LinkList;

    bool ErrorsDownloading;
    bool DataRead;

    QThread *Thread;

};

#endif // MAINUPDATEWINDOW_H
