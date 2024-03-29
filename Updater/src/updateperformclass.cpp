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

#include "updateperformclass.h"
#include "Globals.h"
#include "ServerInfo.h"
#include "updateavailiabledialog.h"

#include <QDebug>

UpdatePerformClass::UpdatePerformClass(QObject *parent) :
    QObject(parent),
    AppUpdateAvailiable(false),
    UpdaterUpdateAvailable(false),
    ErrorsDownloading(false),
    UpdatesPerformed(0),
    UpdateCount(0)
{
}
/******************************
 * Starts the update process
 *****************************/
void UpdatePerformClass::PerformUpdate()
{
    //Get the App Version
    bool AppInfo = GetAppVersion(APP_LOCAL_VERSION_FILENAME);

    if(AppInfo)
    {
        QNetworkReply *AppReply = GetXML(QString(SERVERXML));
        DataRead = ReadXML(AppReply);

        if(DataRead)
        {
            if(QString::number(UPDATER_VERSION) != QString::number(UpdaterVersion))
            {
                UpdateCount += UpdaterDownloadList.size();
                UpdaterUpdateAvailable = true;
            }

            if(LocalAppMajorVersion != AppMajorVersion || LocalAppMinorVersion != AppMinorVersion)
            {
                UpdateCount += AppDownloadList.size();
                AppUpdateAvailiable = true;
            }


        }
    }

    //no updates found
    if(UpdateCount == 0)
    {
        emit Finished();
        return;
    }

    //launch the update dialog
    UpdateAvailiableDialog Dialog(AppUpdateAvailiable,UpdaterUpdateAvailable,AppChangeLog,UpdaterChangeLog);
    connect(&Dialog,SIGNAL(Update(bool,bool)),this,SLOT(Update(bool,bool)));
    if(Dialog.exec() == QDialog::Rejected)
        emit Finished();

}

/********************************************
 * Goes through each update and download it
 ********************************************/
void UpdatePerformClass::RunUpdate()
{
    if(DownloadList.size() <= 0)
    {
        emit Finished();
        return;
    }

    QPair<QPair<QString,QString>,bool> DownloadPair = DownloadList.takeFirst();
    DownloadFiles(DownloadPair.first,DownloadPair.second);
}

/************************************************
 * Handles signal from update availiable dialog
 ************************************************/
void UpdatePerformClass::Update(bool App, bool Updater)
{
    if(Updater)
    {
        for(int i = 0; i < UpdaterDownloadList.size(); ++i)
        {
            DownloadList.append(qMakePair(UpdaterDownloadList.at(i),false));
        }
    }

    if(App)
    {
        for(int i = 0; i < AppDownloadList.size(); ++i)
        {
            DownloadList.append(qMakePair(AppDownloadList.at(i),true));
        }
    }

    RunUpdate();
}

/**********************
 * Gets the online xml
 **********************/
QNetworkReply* UpdatePerformClass::GetXML(QString Url)
{
   QNetworkReply *Reply =  NetworkManager.get(QNetworkRequest(QUrl(Url)));

   QEventLoop Loop;
   QObject::connect(Reply,SIGNAL(finished()), &Loop,SLOT(quit()));
   Loop.exec(); //Excecute the loop until the signal is recieved

   return Reply;
}

/******************************
 * Gets the local app version
 ******************************/
bool UpdatePerformClass::GetAppVersion(QString AppXmlName)
{
    QString Filename = QApplication::applicationDirPath() + QDir::separator() + AppXmlName;

    QFile File(Filename);

    if(!File.exists()) return false; // Return false because we want updater to launch Ouroboros

    if (!File.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open file";
        return false; //File is being used somewhere else
    }

    //Check if file is not empty
    QFileInfo FileInfo(Filename);
    if(FileInfo.size() == 0)
    {
        File.close();
        return false;

    }

    //Read info
    QDomDocument Document;
    Document.setContent(&File);

    QDomElement DocumentElement = Document.documentElement();
    QDomNodeList MajorNode = DocumentElement.elementsByTagName("Major_Version");
    QDomNodeList MinorNode = DocumentElement.elementsByTagName("Minor_Version");

    QDomElement MajorElement = MajorNode.at(0).toElement();
    QDomElement MinorElement = MinorNode.at(0).toElement();


    LocalAppMajorVersion = MajorElement.text().toFloat();
    LocalAppMinorVersion = MinorElement.text().toFloat();

    File.close();

    return true;
}

/*******************
 * Downloads files
 ******************/
void UpdatePerformClass::DownloadFiles(QPair<QString,QString> Pair, bool Rename)
{
    StartRequest(QUrl(Pair.first),Pair.second,Rename);
}

/**********************
 * Sends a GET request
 **********************/
void UpdatePerformClass::StartRequest(QUrl Url,QString Directory,bool Rename)
{
    QFileInfo FileInfo(Url.path());

      /* Prep the progress bar */
    QString LabelText = "Downloading " + FileInfo.fileName() + " ...";
    emit SetProgressLabel(LabelText);
    emit SetProgressValue(0);

    /* Send out the reply */
    QNetworkReply *Reply = NetworkManager.get(QNetworkRequest(Url));
    connect(Reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
    connect(Reply,&QNetworkReply::finished,[=]() {
        ReplyFinished(Reply,Url,Directory,Rename);
    });
}

/*************************
 * Rename update files
 *************************/
void UpdatePerformClass::RenameFile(QString Filename)
{
    QString OldFilename = Filename;
    OldFilename.chop(4); //Remove .upd

    /* Check if file exists */
    QFile OFile(OldFilename);
    if(OFile.exists())
    {
        /* Rename it to [Filename].bak */
        QString NewFilename = OldFilename + ".bak";
        OFile.rename(OldFilename,NewFilename);
    }

    /* Now rename the actual file */
    QFile RFile(Filename);
    if(RFile.rename(Filename,OldFilename))
    {
        /* Remove .bak file */
        QString BakFile = OldFilename + ".bak";
        QFile Bak(BakFile);
        Bak.remove();
    }
}

/*************************
 * Updates progress bar
 *************************/
void UpdatePerformClass::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    emit SetProgressMax(totalBytes);
    emit SetProgressValue(bytesRead);
}

/*******************************
 * Handles file once downloaded
 *******************************/
void UpdatePerformClass::ReplyFinished(QNetworkReply *Reply, QUrl Url, QString Directory, bool Rename)
{
    UpdatesPerformed++;
    if(!Reply->error() == QNetworkReply::NoError) return;

    /* Start making the file */
    QFileInfo FileInfo(Url.path());

    QString Filepath = QApplication::applicationDirPath() + QDir::separator() + Directory;

    //Make the path if it doesn't exist
    if(!QDir(Filepath).exists())
        QDir().mkpath(Filepath);

    QString Filename = FileInfo.fileName() + ".upd";

    /* Check if file exists */
    QFile Check(Filepath.append(Filename));
    if(Check.exists())
    {
        Check.remove();
    }

    QFile File(Filepath);

    if(!File.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open file";
        //Can't open file
        return;
    }

    File.write(Reply->readAll());

    File.close();
    Reply->deleteLater();

    if(Rename)
        RenameFile(Filepath);


    if(UpdateCount <= UpdatesPerformed)
    {
        emit Finished();
        return;
    }

    //Run the next update
    RunUpdate();
}

/*******************************
 * Reads the xml from the reply
 *******************************/
bool UpdatePerformClass::ReadXML(QNetworkReply *Reply)
{
    if(!Reply->error() == QNetworkReply::NoError) return false;

    QByteArray Data = Reply->readAll();
    if(Data.isEmpty()) return false;

    QDomDocument Document;
    Document.setContent(Data);

    QDomElement DocumentElement = Document.documentElement();

    QDomElement ApplicationElement = DocumentElement.elementsByTagName("Ouroboros").at(0).toElement();
    QDomElement UpdaterElement = DocumentElement.elementsByTagName("Updater").at(0).toElement();

    //********************************* Updater info ***************************************************
    QDomNodeList VersionNode = UpdaterElement.elementsByTagName("Version");
    QDomElement VersionElement = VersionNode.at(0).toElement();
    UpdaterVersion = VersionElement.text().toFloat();

    QDomNodeList UpdaterLinks = UpdaterElement.elementsByTagName("Link");
    QDomNodeList UpdaterDirectory = UpdaterElement.elementsByTagName("Directory");
    for(int i = 0; i < UpdaterLinks.size(); i++)
    {
        QDomElement Link = UpdaterLinks.at(i).toElement();
        QDomElement Directory = UpdaterDirectory.at(i).toElement();

        QString LinkString = Link.text();
        QString DirectoryString = (Directory.isNull()) ? "/": Directory.text();

        UpdaterDownloadList.append(qMakePair(LinkString,DirectoryString));
    }

    UpdaterChangeLog = UpdaterElement.elementsByTagName("changelog").at(0).toElement().text();

    //******************************** Read application data *****************************************
    QDomNodeList MajorNode = ApplicationElement.elementsByTagName("Major_Version");
    QDomElement MajorElement = MajorNode.at(0).toElement();

    QDomNodeList MinorNode = ApplicationElement.elementsByTagName("Minor_Version");
    QDomElement MinorElement = MinorNode.at(0).toElement();

    AppMajorVersion = MajorElement.text().toFloat();
    AppMinorVersion = MinorElement.text().toFloat();

    QDomNodeList AppLinks = ApplicationElement.elementsByTagName("Link");
    QDomNodeList AppDirectory = ApplicationElement.elementsByTagName("Directory");
    for(int i = 0; i < AppLinks.size(); i++)
    {
        QDomElement Link = AppLinks.at(i).toElement();
        QDomElement Directory = AppDirectory.at(i).toElement();

        QString LinkString = Link.text();
        QString DirectoryString = (Directory.isNull()) ? "/": Directory.text();
        AppDownloadList.append(qMakePair(LinkString,DirectoryString));
    }

    AppChangeLog = ApplicationElement.elementsByTagName("changelog").at(0).toElement().text();

    return true;
}
