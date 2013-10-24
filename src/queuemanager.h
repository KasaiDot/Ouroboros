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
    QueueWorker *Worker;

    bool Running;

signals:

public slots:

    void Run();

    //Basic list functions
    void AddItem(QueueItem* Item);
    void DeleteItem(QueueItem* Item);

};

}

extern Queue::QueueManager Queue_Manager;
#endif // QUEUEMANAGER_H
