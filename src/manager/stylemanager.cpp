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

#include "stylemanager.h"

#include "filemanager.h"

using namespace Manager;
StyleManager Style_Manager;

StyleManager::StyleManager() :
    QObject(0),
    CurrentStyle(""),
    CurrentFileName("")
{
}

/************************************************
 * Loads the list of files inside theme folder
 ***********************************************/
void StyleManager::LoadStyleList()
{
    FileList = File_Manager.GetStyleList();
}

/**********************************************
 *  Loads a stylesheet into the program
 *********************************************/
void StyleManager::LoadStyle(QString Filename)
{
    if(Filename.isEmpty() || Filename.isNull()) ClearStyle();

    QByteArray Data = File_Manager.LoadStyle(Filename);
    if(Data.isNull() || Data.isEmpty()) return;

    CurrentStyle = Data;
    CurrentFileName = Filename;

    emit StyleChanged(CurrentStyle);
}
