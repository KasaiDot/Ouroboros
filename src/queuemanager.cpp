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
#include "user.h"
#include "filemanager.h"
#include "guimanager.h"

using namespace Queue;

QueueManager Queue_Manager;

QueueManager::QueueManager(QObject *parent) :
    QObject(parent),
    Running(false),
    ClearQueueRequested(false),
    ClearQueueSync(false)
{
    DelayTimer = new QTimer(this);
    DelayTimer->setSingleShot(true);

    //Schedule run execution
    connect(DelayTimer,&QTimer::timeout,[=]() {

        //push the items from failed update to the queue
        foreach(QueueItem *Item,FailedUpdates)
        {
            FailedUpdates.removeOne(Item);
            AddItem(Item);
        }
    });
    connect(DelayTimer,&QTimer::timeout,[=]() { QMetaObject::invokeMethod( this, "Run", Qt::QueuedConnection );} );
}

QueueManager::~QueueManager()
{
}

/*******************************************************
 * Constructs a JSON doc of items of type UpdateLibrary
 *******************************************************/
QJsonDocument QueueManager::ConstructQueueJsonDocument()
{
    QJsonArray Array;

    foreach (QueueItem *Item, ItemQueue)
    {
        //if the item is of type update library, then we get the data from it which in this case is the slug
        if(Item->GetItemType() == QueueItem::Item_UpdateLibrary)
            if(!Item->GetData().isEmpty() && !Array.contains(Item->GetData()))
                Array.append(Item->GetData());
    }

    foreach (QueueItem *Item, FailedUpdates)
    {
        //if the item is of type update library, then we get the data from it which in this case is the slug
        if(Item->GetItemType() == QueueItem::Item_UpdateLibrary)
            if(!Item->GetData().isEmpty() && !Array.contains(Item->GetData()))
                Array.append(Item->GetData());
    }

    //Make the doc
    QJsonDocument Doc(Array);
    return Doc;

}

/***********************************************
 * Adds queue items to the queue from JSON file
 ***********************************************/
void QueueManager::ParseQueueJson(QByteArray Data)
{
    QJsonDocument Doc = QJsonDocument::fromJson(Data);
    QVariantList List = Doc.toVariant().toList();

    foreach (QVariant Slug, List)
    {
        if(!Slug.toString().isEmpty() && !Slug.isNull())
            UpdateLibrary(Slug.toString());
    }

}

/***************************
 * Clears all queue items
 **************************/
void QueueManager::ClearQueue(bool PerformSync)
{
    if(DelayTimer->isActive())
        DelayTimer->stop();

    if(Running)
    {
        ClearQueueRequested = true;
        ClearQueueSync = PerformSync;
    }
    else
        ResetQueue(PerformSync);
}

/**************************************
 * Clears the queue and resets timers
 **************************************/
void QueueManager::ResetQueue(bool PerformSync)
{
    foreach (QueueItem *Item, ItemQueue)
    {
        ItemQueue.removeOne(Item);
        disconnect(Item,SIGNAL(Finished(QueueItem*)),this,SLOT(QueueItemFinished(QueueItem*)));
        disconnect(Item,SIGNAL(PopulateModel()),&GUI_Manager,SLOT(PopulateModel()));
        Item->deleteLater();
    }

    foreach (QueueItem *Item, FailedUpdates)
    {
        FailedUpdates.removeOne(Item);
        disconnect(Item,SIGNAL(Finished(QueueItem*)),this,SLOT(QueueItemFinished(QueueItem*)));
        disconnect(Item,SIGNAL(PopulateModel()),&GUI_Manager,SLOT(PopulateModel()));
        Item->deleteLater();
    }

    Running = false;

    if(DelayTimer->isActive())
        DelayTimer->stop();

    if(PerformSync)
    {
        Sync(true);
    }

    ClearQueueRequested = false;
    ClearQueueSync = false;
}

/*****************************************
 * Adds authenticate action to the queue
 *****************************************/
void QueueManager::AuthenticateUser()
{
    //Auth should be first priority, so we add it to the top of the queue
    if(ItemQueue.size() > 0)
    {
        if(ItemQueue.at(0)->GetItemType() != QueueItem::Item_Auth)
        {
            QueueItem *Item = new QueueItem(this,QueueItem::Item_Auth);
            ItemQueue.insert(0,Item);
        }
    } else {
        QueueItem *Item = new QueueItem(this,QueueItem::Item_Auth);
        AddItem(Item);
    }

    StartRunning();

}
/*************************************************
 * Adds all status getlibrary calls to the queue
 *************************************************/
void QueueManager::GetAnimeLibrary()
{
    QueueItem *CurrentlyWatching = new QueueItem(this,QueueItem::Item_GetLibrary,STATUS_CURRENTLY_WATCHING);
    QueueItem *OnHold = new QueueItem(this,QueueItem::Item_GetLibrary,STATUS_ON_HOLD);
    QueueItem *PlanToWatch = new QueueItem(this,QueueItem::Item_GetLibrary,STATUS_PLAN_TO_WATCH);
    QueueItem *Dropped = new QueueItem(this,QueueItem::Item_GetLibrary,STATUS_DROPPED);
    QueueItem *Completed = new QueueItem(this,QueueItem::Item_GetLibrary,STATUS_COMPLETED);
    QueueItem *PopulateModel = new QueueItem(this,QueueItem::Item_PopulateModel);

    AddItem(CurrentlyWatching);
    AddItem(Completed);
    AddItem(OnHold);
    AddItem(PlanToWatch);
    AddItem(Dropped);
    AddItem(PopulateModel);

    StartRunning();
}

/*****************************************
 * Adds anime update request to the queue
 *****************************************/
void QueueManager::UpdateLibrary(QString Slug)
{
    //Don't update it if it's not in the database
    if(!Anime_Database.Contains(Slug)) return;

    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(Slug);

    //We don't want to sync if the episode count is -1
    if(Entity->GetUserInfo()->GetEpisodesWatched() <= ANIMEENTITY_UNKNOWN_USER_EPISODE) return;

    bool SlugFound = false;
    //Check if we have updates pending for the same anime
    for(QList<QueueItem*>::const_iterator i = ItemQueue.begin(); i != ItemQueue.end(); ++i)
    {
        if((*i)->GetData() == Slug && !Running)
            SlugFound = true;
    }

    if(!SlugFound)
    {
        QueueItem *Item = new QueueItem(this,QueueItem::Item_UpdateLibrary,Slug);
        AddItem(Item);
    }
    StartRunning();
}

/***************************************
 * Starts syncing app with hummingbird
 ***************************************/
void QueueManager::Sync(bool LoadQueueFromFile)
{
    AuthenticateUser();
    GetAnimeLibrary();
    if(LoadQueueFromFile)
        File_Manager.LoadQueue();
}

/*********************************************************
 * Starts a timer and if any other function calls
 * this function then a delay will be added to the timer
 * once timer stops then the run function is called
 *********************************************************/
void QueueManager::StartRunning()
{
    int InitialDelay = 3000;
    int AdditionalDelay = 1000;
    if(DelayTimer->isActive())
        DelayTimer->setInterval(InitialDelay + AdditionalDelay);
    else
    {
        DelayTimer->setInterval(InitialDelay);
        DelayTimer->start();
    }

}

/******************************************************
 * Creates a thread and makes a worker to asynchronously
 * run each item
 *****************************************************/
void QueueManager::Run()
{
    if(Running || ItemQueue.size() <= 0) return;

    Running = true;

    ItemQueue.head()->Run();

}

/*****************************************************
 * Catched signal sent by Queueitem when it finished
 *****************************************************/
void QueueManager::QueueItemFinished(QueueItem *Item)
{
    Running = false;

    //Reset the queue and return
    if(ClearQueueRequested)
    {
        ResetQueue(ClearQueueSync);
        return;
    }

    //Check for errors
    if(Item->Error != QueueItem::ItemReturn_Success)
    {
        if(Item->Error == QueueItem::ItemReturn_NotAuthed)
        {
            CurrentUser.SetAuthenticated(false);
            AuthenticateUser();
            return;
        }

        if(Item->Error == QueueItem::ItemReturn_AuthFail)
        {
            return;
        }
    }

    if(Item->Error == QueueItem::ItemReturn_ReplyUnknownError || Item->Error == QueueItem::ItemReturn_Fail)
    {
        if(Item->GetItemType() == QueueItem::Item_UpdateLibrary)
        {
            UpdateFailed(Item);
        } else {
            DeleteItem(Item);
        }

    }else if(Item->Error == QueueItem::ItemReturn_ApiFail || Item->Error == QueueItem::ItemReturn_NoData || Item->Error == QueueItem::ItemReturn_Success) {

        DeleteItem(Item);

    }

    //Run the next item
    Run();
}

/***********************************************************
 * Adds item to the queue, if it already exists it returns
 ***********************************************************/
void QueueManager::AddItem(QueueItem *Item)
{
    if(ItemQueue.contains(Item)) return;

    //Connect signal and slots
    connect(Item,SIGNAL(Finished(QueueItem*)),this,SLOT(QueueItemFinished(QueueItem*)));
    connect(Item,SIGNAL(PopulateModel()),&GUI_Manager,SLOT(PopulateModel()));

    //Add the item
    ItemQueue.enqueue(Item);
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

/*********************************************
 * Pushes the item to the back of the queue
 ********************************************/
void QueueManager::RePushItem(QueueItem *Item)
{
    if(ItemQueue.contains(Item))
        ItemQueue.removeOne(Item);

    AddItem(Item);
}

/*************************************************************
 * Adds failed updates to the FailedUpdates Queue
 *************************************************************/
void QueueManager::UpdateFailed(QueueItem *Item)
{
    if(ItemQueue.contains(Item))
    {
        ItemQueue.removeOne(Item);
        FailedUpdates.append(Item);
    }
}
