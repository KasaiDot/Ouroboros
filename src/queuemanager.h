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

#include "queueitem.h"
#include "queueworker.h"

namespace Queue
{

class QueueManager : public QObject
{
    Q_OBJECT
public:
    explicit QueueManager(QObject *parent = 0);
    ~QueueManager();

private:

    //Main Queue
    QList<QueueItem*> ItemQueue;
    QPointer<QueueWorker> Worker;


    void StartRunning();

    bool Running;

    //Used to delay the queue if a new item is added in
    QTimer *DelayTimer;

signals:
    void PopulateModel();

public slots:
    void Run();

    //Basic list functions
    void AddItem(QueueItem* Item);
    void DeleteItem(QueueItem* Item);

    //Api functions
    void AuthenticateUser();
    void GetAnimeLibrary();
    void UpdateLibrary(QString Slug);
};

}

extern Queue::QueueManager Queue_Manager;
#endif // QUEUEMANAGER_H
