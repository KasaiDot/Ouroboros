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

#ifndef QUEUEMANAGER_H
#define QUEUEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QQueue>
#include <QJsonDocument>
#include <QJsonArray>

#include "api/queueitem.h"

namespace Queue
{

class QueueManager : public QObject
{
    Q_OBJECT
public:
    explicit QueueManager(QObject *parent = 0);
    ~QueueManager();

    int GetQueueSize() const { return (ItemQueue.size() + FailedUpdates.size()); }

    //Json parsing
    QJsonDocument ConstructQueueJsonDocument();
    void ParseQueueJson(QByteArray Data);

private:

    //Main Queue
    QQueue<QueueItem*> ItemQueue;
    QQueue<QueueItem*> FailedUpdates;

    bool Running;
    bool ClearQueueRequested;
    bool ClearQueueSync;

    //Used to delay the queue if a new item is added in
    QTimer *DelayTimer;


signals:

public slots:

    //Clears the queue
    void ClearQueue(bool PerformSync = false);
    void ResetQueue(bool PerformSync = false);

    //Starts the queue timer
    void StartRunning();

    //Handles queue item once finished
    void QueueItemFinished(QueueItem *Item);

    //Basic list functions
    void AddItem(QueueItem* Item, bool PushFront = false);
    void DeleteItem(QueueItem* Item);
    void RePushItem(QueueItem* Item);
    void UpdateFailed(QueueItem* Item);

    //checks whether an item in the queue contains given data
    bool ItemContainsData(QString Data);

    //Api functions
    void AuthenticateUser();
    void GetAnimeLibrary();
    void UpdateLibrary(QString Slug);
    void Sync(bool LoadQueueFromFile = false);

private slots:
    //Runs through the queue
    void Run();

};

}

extern Queue::QueueManager Queue_Manager;
#endif // QUEUEMANAGER_H
