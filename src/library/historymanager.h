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

#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

//*********************************************************************************************************
#define HISTORY_MAXSIZE 1500
#define HISTORY_DATEFORMAT "hh:mm:ss dd/MM/yy"
//*********************************************************************************************************

#include <QObject>
#include <QStack>
#include <QJsonDocument>

namespace Manager
{

//A history item used in the stack
struct HistoryItem
{
    QString AnimeTitle;
    QString Action;
    QString DateAndTime;
};

class HistoryManager : public QObject
{
    Q_OBJECT
public:
    explicit HistoryManager(QObject *parent = 0);

private:
    //The history stack
    //A Stack is used because we want the most recent items to be on top
    QStack<Manager::HistoryItem> HistoryStack;

public slots:
    //Stack functions
    void AddHistoryItem(HistoryItem &Item, bool PushFront = true);
    void AddHistoryItem(QString AnimeTitle,QString Action, QString DateAndTime, bool PushFront = true);
    void RemoveHistoryItem(int Index);
    void ClearHistory();

    //History dialog functions
    void ShowHistoryDialog();

    //save and load functions
    QJsonDocument ConstructHistoryJsonDocument();
    void ParseHistoryJson(QByteArray &Data);

};

}

extern Manager::HistoryManager History_Manager;

#endif // HISTORYMANAGER_H
