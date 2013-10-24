#ifndef QUEUEWORKER_H
#define QUEUEWORKER_H

#include <QObject>
#include <QList>

#include "queueitem.h"

namespace Queue
{

class QueueWorker : public QObject
{
    Q_OBJECT
public:
    explicit QueueWorker(QList<QueueItem*> &Queue);

private:
    //Original Queue
    QList<QueueItem*> Queue;

signals:
    void Finished();
    void DeleteQueueItem(QueueItem*);

public slots:
    void Run();

};

}

#endif // QUEUEWORKER_H
