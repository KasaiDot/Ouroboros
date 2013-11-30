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

#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#define MEDIAMANAGER_MEDIANOTFOUND -1
#define MEDIAMANAGER_UPDATEDELAY 1000

#include <QObject>
#include <QList>
#include <QtGlobal>
#include <QDebug>
#include <QStringList>
#include <QApplication>

#include "windows.h"
#include "psapi.h"
#include "winuser.h"

/*********************************************************************
 * Credits to erengy(Creator of Taiga) for the media class
 ********************************************************************/

namespace Recognition
{
//Media class
class Media
{
public:
    Media();
    QString Name;
    bool Enabled;
    int Mode;
    QStringList Files;
    QStringList Folders;
    QStringList Keywords;
    HWND WindowHandle;
};

class MediaManager : public QObject
{
    Q_OBJECT
public:
    explicit MediaManager(QObject *parent = 0);

    void Load();
    void Save();

    void RemoveKeywords(QString &Title, int MediaIndex);

    int Index;
    int OldIndex;

    QString CurrentTitle;
    QString NewTitle;

    //Mode methods
    QString GetProcessNameFromHandle(HWND Handle);
    QString GetWindowTitleFromHandle(HWND Handle);

    bool didTitleChange() const { return TitleChanged; }
    void SetTitleChanged(bool Changed) { TitleChanged = Changed;}

    //Different modes of recognition, default is WindowTitle
    enum MediaModes
    {
        MediaMode_WindowTitle
    };

    QList<Media> MediaList;

private:
    bool TitleChanged;
    int MediaTicker;

signals:

public slots:
    int DetectMediaPlayer();
    void DetectAnime();

};

}

//global
extern Recognition::MediaManager Media_Manager;

#endif // MEDIAMANAGER_H
