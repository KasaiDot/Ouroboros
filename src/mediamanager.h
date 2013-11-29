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

#include <QObject>
#include <QLinkedList>
#include <QtGlobal>
#include <QDebug>
#include <QStringList>
#include <QApplication>

#include "windows.h"
#include "psapi.h"
#include "winuser.h"

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
    QStringList RemoveKeywords;
    HWND WindowHandle;
};

class MediaManager : public QObject
{
    Q_OBJECT
public:
    explicit MediaManager(QObject *parent = 0);

    void Load(HWND Handle);
    void Save();

    int Index;
    int OldIndex;

    QString CurrentTitle;
    QString NewTitle;

    //Mode methods
    QString GetProcessNameFromHandle(HWND Handle);
    QString GetWindowTitleFromHandle(HWND Handle);

    //Different modes of recognition, default is WindowTitle
    enum MediaModes
    {
        MediaMode_WindowTitle
    };

    QLinkedList<Media> MediaList;

private:
    bool TitleChanged;

signals:

public slots:
    int DetectAnime();

};

}

//global
extern Recognition::MediaManager Media_Manager;

#endif // MEDIAMANAGER_H
