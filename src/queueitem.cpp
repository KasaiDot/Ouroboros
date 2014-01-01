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

#include "queueitem.h"

#include <QDebug>

#include "apimanager.h"
#include "guimanager.h"
#include "animedatabase.h"

using namespace Queue;

QueueItem::QueueItem(QObject *parent, ItemType Type) :
    QObject(parent),
    Type(Type),
    Data("")
{
    //We need to generate an id for the item
    Id = RandomValue(0,9999);
}

QueueItem::QueueItem(QObject *parent, QueueItem::ItemType Type, QString Data) :
    QObject(parent),
    Type(Type)
{
    //We need to generate an id for the item
    Id = RandomValue(0,9999);

    this->Data = Data;
}

/************************************
 * Sends Api call based on the item
 ************************************/
void QueueItem::Run()
{
    //the default return code is success
    int ApiReturnCode = Manager::ApiManager::Api_Success;

    //by default we have no errors when calling the run function
    Error = ItemReturn_Success;

    switch(Type)
    {
        case Item_Auth:
            ApiReturnCode = Api_Manager.Authenticate();
        break;

        case Item_GetLibrary:
            if(isDataSet())
                ApiReturnCode = Api_Manager.GetLibrary(Data);
            else
                Error = ItemReturn_NoData;
        break;

        case Item_UpdateLibrary:
            if(isDataSet())
            {
                if(Anime_Database.GetAnime(Data)->GetUserInfo()->GetEpisodesWatched() > ANIMEENTITY_UNKNOWN_USER_EPISODE)
                    ApiReturnCode = Api_Manager.UpdateLibrary(Data);
            }
            else
                Error = ItemReturn_NoData;
        break;

        case Item_PopulateModel:
            emit PopulateModel();
        break;
    }

    //Error checking
    if(ApiReturnCode == Manager::ApiManager::Reply_UnknownError)
        Error = ItemReturn_ReplyUnknownError;

    if(ApiReturnCode == Manager::ApiManager::Api_NotAuthed)
        Error = ItemReturn_NotAuthed;

    if(ApiReturnCode == Manager::ApiManager::Api_InvalidCredentials)
        Error = ItemReturn_AuthFail;

    if(ApiReturnCode == Manager::ApiManager::Api_Failure)
        Error = ItemReturn_ApiFail;

    if(ApiReturnCode == Manager::ApiManager::Reply_Error || ApiReturnCode == Manager::ApiManager::Reply_Timeout)
        Error = ItemReturn_Fail;

    //we have now finished
    emit Finished(this);
}

/*************************************************
 * This operator overload compares the Id and the
 * type of the item to determine if it is the
 * same as another.
 *************************************************/
bool QueueItem::operator ==(const QueueItem &Other)
{
    if((this->GetId() == Other.GetId()) && (this->GetItemType() == Other.GetItemType()))
        return true;

    return false;
}


