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
