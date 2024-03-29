/*
**  Ouroboros, Hummingbird.me Desktop App
**  Copyright (C) 2014, Mikunj Varsani
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

#include "thememanager.h"

#include "manager/filemanager.h"

using namespace Manager;
ThemeManager Theme_Manager;

ThemeManager::ThemeManager() :
    QObject(0),
    CurrentTheme(""),
    CurrentFileName("")
{
}

/************************************************
 * Loads the list of files inside theme folder
 ***********************************************/
void ThemeManager::LoadThemeList()
{
    FileList = File_Manager.GetThemeList();
    emit ThemeListLoaded();
}

/**********************************************
 *  Loads a stylesheet into the program
 *********************************************/
void ThemeManager::LoadTheme(QString Filename)
{
    if(Filename.isEmpty() || Filename.isNull()) ClearTheme();

    QByteArray Data = File_Manager.LoadTheme(Filename);
    if(Data.isNull() || Data.isEmpty()) return;

    CurrentTheme = QString(Data);
    CurrentFileName = Filename;

    emit ThemeChanged(CurrentTheme);
}
