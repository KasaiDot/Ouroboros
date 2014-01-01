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
