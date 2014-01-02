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

#include "historymanager.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariant>

#include "ui/dialog_history.h"

using namespace Manager;
HistoryManager History_Manager;

HistoryManager::HistoryManager(QObject *parent) :
    QObject(parent)
{
}

/******************************
 * Adds an item to the stack
 ******************************/
void HistoryManager::AddHistoryItem(HistoryItem &Item, bool PushFront)
{
    if(HistoryStack.size() + 1 > HISTORY_MAXSIZE)
        HistoryStack.pop_back(); //remove the last item

    if(PushFront) HistoryStack.push_front(Item); //push the item to the front of the history stack
    else HistoryStack.push_back(Item);
}

void HistoryManager::AddHistoryItem(QString AnimeTitle, QString Action, QString DateAndTime, bool PushFront)
{
    HistoryItem Item = {AnimeTitle,Action,DateAndTime};
    AddHistoryItem(Item, PushFront);
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
    //the document will be in this format
    /*
     *[
     *  {
     *      title:
     *      action:
     *      time:
     *  }
     *]
     */
    QJsonArray MainArray;

    foreach (HistoryItem Item, HistoryStack)
    {
        QJsonObject ItemObject;

        //values
        QJsonValue Title(Item.AnimeTitle);
        QJsonValue Action(Item.Action);
        QJsonValue Time(Item.DateAndTime);

        //add values to the object
        ItemObject.insert("title",Title);
        ItemObject.insert("action",Action);
        ItemObject.insert("time",Time);

        //add object to array
        MainArray.append(ItemObject);
    }

    return QJsonDocument(MainArray);
}

/***************************************************************
 * Adds history items based off the JSON data read from file
 ***************************************************************/
void HistoryManager::ParseHistoryJson(QByteArray &Data)
{
    if(Data.isEmpty() || Data.isNull()) return;

    //Clear the history first
    ClearHistory();

    QJsonDocument Doc = QJsonDocument::fromJson(Data);
    QVariantList MainArray = Doc.toVariant().toList();

    foreach (QVariant Variant, MainArray)
    {
        QVariantMap ItemMap = Variant.toMap();

        //Read the values
        QString Title = ItemMap.value("title").toString();
        QString Action = ItemMap.value("action").toString();
        QString Time = ItemMap.value("time").toString();

        //append the history item to the back of the queue since the most recent
        //history items will be at the top
        AddHistoryItem(Title,Action,Time, false);
    }
}
