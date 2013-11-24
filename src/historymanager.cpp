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

#include "historymanager.h"
#include "dialog_history.h"

using namespace Manager;
HistoryManager History_Manager;

HistoryManager::HistoryManager(QObject *parent) :
    QObject(parent)
{
}

/******************************
 * Adds an item to the stack
 ******************************/
void HistoryManager::AddHistoryItem(HistoryItem &Item)
{
    if(HistoryStack.size() + 1 > HISTORY_MAXSIZE)
        HistoryStack.pop_back(); //remove the last item

    HistoryStack.push_front(Item); //push the item to the front of the history stack
}

void HistoryManager::AddHistoryItem(QString AnimeTitle, QString Action, QString DateAndTime)
{
    HistoryItem Item = {AnimeTitle,Action,DateAndTime};
    AddHistoryItem(Item);
}

/**************************************************
 * Removes an item at a given index in the stack
 **************************************************/
void HistoryManager::RemoveHistoryItem(int Index)
{
    if((HistoryStack.size() - 1 < Index) || (Index < 0)) return;
    HistoryStack.remove(Index);
}

/************************************
 * Removes all items from the stack
 ***********************************/
void HistoryManager::ClearHistory()
{
    HistoryStack.clear();
}

/*******************************
 * Displays the history dialog
 *******************************/
void HistoryManager::ShowHistoryDialog()
{
    Dialog_History History(HistoryStack);

    //connect signals and slots

    History.exec();
}

/*****************************************************
 * Creates a JSON file containing history item data
 *****************************************************/
QJsonDocument HistoryManager::ConstructHistoryJsonDocument()
{

}

/***************************************************************
 * Adds history items based off the JSON data read from file
 ***************************************************************/
void HistoryManager::ParseHistoryJson(QByteArray &Data)
{
    if(Data.isEmpty() || Data.isNull()) return;

    //Clear the history first
    ClearHistory();

}
