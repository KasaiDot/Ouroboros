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

#include "queueworker.h"

#include <QDebug>
#include <QList>

using namespace Queue;

QueueWorker::QueueWorker(QList<QueueItem *> &Queue) :
    Queue(Queue)
{
}

/*******************************************************
 * Runs through each item in the queue
 * If the item was run successfully it will delete it
 * if not then it will stay in the queue
 *******************************************************/
void QueueWorker::Run()
{
    for(QList<QueueItem*>::const_iterator i = Queue.begin(); i != Queue.end(); ++i)
    {
        QueueItem *Item = (*i);
        int Code = Item->Run();

        if(Code == QueueItem::ItemReturn_Success)
            emit DeleteQueueItem(Item);
        if(Code == QueueItem::ItemReturn_AuthFail)
            break;
        if(Code == QueueItem::ItemReturn_NotAuthed)
        {
            emit AuthUser();
            break;
        }
        if(Code == QueueItem::ItemReturn_NoData)
            emit DeleteQueueItem(Item);
    }

    emit Finished();
}
