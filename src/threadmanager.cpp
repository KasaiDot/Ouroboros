#include "threadmanager.h"

#include <QApplication>

using namespace Manager;

//Make class global
ThreadManager Thread_Manager;

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{
}

ThreadManager::~ThreadManager()
{
    //Go through the active thread list and send the quit signal to the threads
    foreach(QThread *Thread, ActiveThreadList)
    {
        Thread->quit();
        Thread->wait();

        DeleteThread(Thread);
    }
}

/***********************************************************************
 * This function creates a new thread and returns it to the application
 ***********************************************************************/
QThread *ThreadManager::GetThread()
{
    QThread *Thread = new QThread(this);
    ActiveThreadList.append(Thread);

    return Thread;
}

/*************************************************************************
 * This function deletes a thread assuming quit() has been already called
 *************************************************************************/
void ThreadManager::DeleteThread(QThread *Thread)
{
    //Check if the thread exists in the list
    if(ActiveThreadList.contains(Thread))
    {
        int ThreadIndex = ActiveThreadList.indexOf(Thread);
        ActiveThreadList.removeAt(ThreadIndex);
    }

    //Wait for the thread to finish internal jobs then delete it
    Thread->deleteLater();
}
