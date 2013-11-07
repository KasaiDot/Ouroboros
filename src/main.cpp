/*
**  Ouroboros, Hummingbird.me Desktop App
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

#include "ouroboros.h"
#include "globals.h"

#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QtXml>
#include <QMessageBox>

/***********************************
 * Writes application version info
 ***********************************/
void WriteAppInfo(QString Filename, QString MajorVersion, QString MinorVersion)
{
    QString FileName = QApplication::applicationDirPath() + QDir::separator() + Filename;

    if(FileName.isEmpty()) return;

    QFile File(FileName);

    if (!File.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open file";
        return; //Cannot open file
    }

    /*if file is successfully opened then create XML*/
    QXmlStreamWriter* XmlWriter = new QXmlStreamWriter();
    XmlWriter->setAutoFormatting(true);

    /* set the file */
    XmlWriter->setDevice(&File);

    /* Writes a document start with the XML version number version. */
    XmlWriter->writeStartDocument();

    XmlWriter->writeStartElement("Info");

    /* Now write the version */
    XmlWriter->writeStartElement("Major_Version");
    XmlWriter->writeCharacters(MajorVersion);
    XmlWriter->writeEndElement();

    XmlWriter->writeStartElement("Minor_Version");
    XmlWriter->writeCharacters(MinorVersion);
    XmlWriter->writeEndElement();

    XmlWriter->writeEndElement();
    XmlWriter->writeEndDocument();

    File.flush();
    File.close();

    delete XmlWriter;
}

/***************************************
 * Checks for .upd file for the updater
 ****************************************/
void CheckUpdaterFiles(QString UpdaterFilename)
{
    QString OldFilename = QApplication::applicationDirPath() + QDir::separator() + UpdaterFilename;
    QString Filename = OldFilename + ".upd";

    QFile RFile(Filename);
    if(!RFile.exists()) return;
    if(RFile.size() == 0) return;

    /* Check if file exists */
    QFile OFile(OldFilename);
    if(OFile.exists())
    {
        /* Rename it to [Filename].bak */
        QString NewFilename = OldFilename + ".bak";
        OFile.rename(OldFilename,NewFilename);
    }

    /* Now rename the actual file */
    if(RFile.rename(Filename,OldFilename))
    {
        /* Remove .bak file */
        QString BakFile = OldFilename + ".bak";
        QFile Bak(BakFile);
        Bak.remove();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WriteAppInfo(APP_LOCAL_VERSION_FILENAME,QString::number(APP_MAJOR_VERSION),QString::number(APP_MINOR_VERSION));
    CheckUpdaterFiles(APP_UPDATER);

    bool Args = false;

    if(!APP_DEBUG)
    {
        if(argc < 2)
        {
            Args = true;
            QString UpdaterPath = QApplication::applicationDirPath() + QDir::separator() + APP_UPDATER;

            /* Check if an updater exists */
            QFile Updater(UpdaterPath);
            if(!Updater.exists())
            {
                if(QMessageBox::critical(0,"Error","Cannot find Ouroboros Updater.exe, please re-install Ouroboros") == QMessageBox::Ok)
                    return 0;
            }

            QProcess::startDetached(UpdaterPath,QStringList());
        }
    }

    if(!Args)
    {
        Ouroboros Window;
        Window.show();
        Window.raise();
        Window.activateWindow();
        return a.exec();
    }

    return 0;
}
