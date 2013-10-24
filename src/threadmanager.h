#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QThread>
#include <QDebug>

namespace Manager
{

class ThreadManager : public QObject
{
    Q_OBJECT
public:
    explicit ThreadManager(QObject *parent = 0);
    ~ThreadManager();

    QThread* GetThread();

private:

    // Hold the list of currently active threads
    QList<QThread*> ActiveThreadList;

signals:

public slots:
    void DeleteThread(QThread* Thread);
};

}

extern Manager::ThreadManager Thread_Manager;

#endif // THREADMANAGER_H
