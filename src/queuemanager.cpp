#include "queuemanager.h"

#include "threadmanager.h"

using namespace Queue;

QueueManager Queue_Manager;

QueueManager::QueueManager(QObject *parent) :
    QObject(parent),
    Running(false)
{
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
    connect(Thread,&QThread::finished,[=](){ Thread_Manager.DeleteThread(Thread); Running = false;});

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
