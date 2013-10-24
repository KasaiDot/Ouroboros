#include "queueworker.h"

using namespace Queue;

QueueWorker::QueueWorker(QList<QueueItem *> &Queue) :
    Queue(Queue)
{

}

void QueueWorker::Run()
{
    foreach (QueueItem* Item, Queue)
    {
        int Code = Item->Run();

        if(Code == QueueItem::ItemReturn_Success)
            emit DeleteQueueItem(Item);
    }

    emit Finished();
}
