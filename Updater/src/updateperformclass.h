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

#ifndef UPDATEPERFORMCLASS_H
#define UPDATEPERFORMCLASS_H

#include <QObject>
#include <QtNetwork>
#include <QtXml>
#include <QApplication>

class UpdatePerformClass : public QObject
{
    Q_OBJECT
public:
    explicit UpdatePerformClass(QObject *parent = 0);

    //Reads xml from a reply
    bool ReadXML(QNetworkReply *Reply);

    //gets the updater file
    QNetworkReply* GetXML(QString Url);

    //Downloads files
    void DownloadFiles(QStringList &LinkList, QStringList &DirectoryList, bool Rename = true);

    void RenameFile(QString Filename);

    void startRequest(QUrl url, QString Directory = "/", bool Rename = true);
    bool GetAppVersion(QString AppXmlName);

public slots:
    void PerformUpdate();

private slots:
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void ReplyFinished(QNetworkReply *Reply, QUrl Url, QString Directory, bool Rename);

signals:
    void finished();
    void SetProgressValue(int Value);
    void SetProgressMax(int Value);
    void SetProgressLabel(QString Text);
    void OpenWarningMessageBox(QString Title,QString Text);

private:
    QNetworkAccessManager NetworkManager;

    /* Variables storing versions locally */
    float LocalAppMajorVersion;
    float LocalAppMinorVersion;

    /* Variables storing versions online */
    float AppMajorVersion;
    float AppMinorVersion;
    float UpdaterVersion;

    QStringList AppLinkList;
    QStringList AppDirectoryList;

    QStringList UpdaterLinkList;
    QStringList UpdaterDirectoryList;

    bool ErrorsDownloading;
    bool DataRead;

    int UpdatesPerformed;
    int UpdateCount;
    
};

#endif // UPDATEPERFORMCLASS_H
