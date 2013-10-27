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

#include "queuemanager.h"

#include <QTimer>

#include "threadmanager.h"

using namespace Queue;

QueueManager Queue_Manager;

QueueManager::QueueManager(QObject *parent) :
    QObject(parent),
    Running(false)
{
    //Setup a timer to run the queue every 10 minutes
    //Note: currently giving errors
    QTimer *RunTimer = new QTimer(this);
    connect(RunTimer,SIGNAL(timeout()),this,SLOT(Run()));
    RunTimer->start(600000);
}

QueueManager::~QueueManager()
{
    //Delete worker if it exists
    if(Worker)
        Worker->deleteLater();
}

/******************************************************
 * Creates a thread and makes a worker to asynchronously
 * run each item
 *****************************************************/
void QueueManager::Run()
{
    if(Running) return;

    Running = true;
    QThread *Thread = Thread_Manager.GetThread();
    Worker = new QueueWorker(ItemQueue);
    Worker->moveToThread(Thread);

    //Connect signals and slots
    connect(Thread,SIGNAL(started()),Worker,SLOT(Run()));
    connect(Worker,SIGNAL(DeleteQueueItem(QueueItem*)),this,SLOT(DeleteItem(QueueItem*)));
    connect(Worker,SIGNAL(Finished()),Thread,SLOT(quit()));
    connect(Worker,SIGNAL(Finished()),Worker,SLOT(deleteLater()));

    //Make sure not to manually delete the thread, instead send it to the thread manager to delete
    connect(Thread,&QThread::finished,[=](){
                                             Thread_Manager.DeleteThread(Thread);
                                             Running = false;
                                           });

}

/***********************************************************
 * Adds item to the queue, if it already exists it returns
 ***********************************************************/
void QueueManager::AddItem(QueueItem *Item)
{
    if(ItemQueue.contains(Item)) return;
    ItemQueue.append(Item);
}

/*******************************************************
 * Deletes item from queue if it exists
 ******************************************************/
void QueueManager::DeleteItem(QueueItem *Item)
{
    if(ItemQueue.contains(Item))
        ItemQueue.removeOne(Item);

    Item->deleteLater();
}
