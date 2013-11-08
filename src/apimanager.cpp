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

#include "apimanager.h"

#include <QEventLoop>
#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "user.h"
#include "animedatabase.h"
#include "guimanager.h"

using namespace Manager;

//Make class global
ApiManager Api_Manager;

ApiManager::ApiManager(QObject *parent) :
    QObject(parent)
{
}

/*****************************
 * Authenticates current user
 *****************************/
ApiManager::ApiReturnStatus ApiManager::Authenticate()
{
    emit ChangeStatus("Authenticating ...");
    if(CurrentUser.isAuthenticated())
    {
        emit ChangeStatus("Already Authenticated");
        return Api_AlreadyAuth;
    }

    //create network manager
    QScopedPointer<QNetworkAccessManager> NetworkManager(new QNetworkAccessManager);
    QString Data = QString(CurrentUser.BuildAuthJsonDocument().toJson());

    QNetworkReply *Reply = DoHttpPost(NetworkManager.data(),QUrl(QString(API_URL_AUTH)),Data);
    return ProcessReply(Reply,Call_Auth);
}

/*************************************
 * Gets users library based on status
 *************************************/
ApiManager::ApiReturnStatus ApiManager::GetLibrary(QString Status)
{
    emit ChangeStatus("Getting Library: " + Status);

    //create network manager
    QScopedPointer<QNetworkAccessManager> NetworkManager(new QNetworkAccessManager);
    QString Url = API_URL_GETLIBRARY;
    Url.replace("<username>",CurrentUser.GetUsername());
    Url.replace("<status>",Status);
    Url.replace("<key>",CurrentUser.GetAuthKey());

    QNetworkReply *Reply = DoHttpGet(NetworkManager.data(),QUrl(Url));
    return ProcessReply(Reply,Call_GetLibrary);
}

/*********************************************
 * updates anime in users hummingbird library
 *********************************************/
ApiManager::ApiReturnStatus ApiManager::UpdateLibrary(QString Slug)
{
    QString Title = Anime_Database.GetAnime(Slug)->GetAnimeTitle();
    emit ChangeStatus("Updating: " + Title);

    QScopedPointer<QNetworkAccessManager> NetworkManager(new QNetworkAccessManager);
    QString Url = API_URL_UPDATELIBRARY;
    Url.replace("<slug>",Slug);

    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(Slug);
    QJsonObject Object = Entity->ConstructUpdateJsonObject();
    Object.insert("auth_token",CurrentUser.GetAuthKey());

    QJsonDocument Doc(Object);
    QString Data = QString(Doc.toJson());

    QNetworkReply *Reply = DoHttpPost(NetworkManager.data(),QUrl(Url),Data);
    return ProcessReply(Reply,Call_UpdateLibrary);

}

/******************************
 * Gets anime image file
 ******************************/
QNetworkReply *ApiManager::GetAnimeImage(QNetworkAccessManager *NetworkManager, const QUrl &Url)
{
    return DoHttpGet(NetworkManager,Url,false);
}

/*************************************************************************
 * Processes Reply recieved through QNetworkManager based on the ApiCall
 ************************************************************************/
ApiManager::ApiReturnStatus ApiManager::ProcessReply(QNetworkReply *Reply, ApiManager::ApiCall Call)
{
    int StatusTimeout = 5000;

    //Check for timeout or errors
    if(Reply->property("Timeout").toBool())
    {
        emit ChangeStatus("Reply Timeout",StatusTimeout);
        Reply->deleteLater();
        return Reply_Timeout;
    }

    if(Reply->error() == QNetworkReply::UnknownNetworkError)
    {
        emit ChangeStatus("Unknown Reply Error",StatusTimeout);
        Reply->deleteLater();
        return Reply_UnknownError;
    }

    if(Reply->error() != QNetworkReply::AuthenticationRequiredError && Reply->error() != QNetworkReply::NoError)
    {
        emit ChangeStatus("Reply Error",StatusTimeout);
        Reply->deleteLater();
        return Reply_Error;
    }

    //Read the reply
    QByteArray ReplyData = Reply->readAll();

    //Check if user is authenticated and has the right auth token
    if(ReplyData.contains("Invalid credentials") || Reply->error() == QNetworkReply::AuthenticationRequiredError)
    {
        emit ChangeStatus("Authentication error",StatusTimeout);
        Reply->deleteLater();
        return Api_InvalidCredentials;
    }

    if(ReplyData.contains("Invalid authentication token"))
    {
        Reply->deleteLater();
        return Api_NotAuthed;
    }

    //Check for other errors
    if(ReplyData.isEmpty()
            || ReplyData.contains("Invalid JSON Object")
            || ReplyData.contains("\"error\":"))
    {
        emit ChangeStatus("Uknown Error",StatusTimeout);
        Reply->deleteLater();
        return Api_Failure;
    }

    switch (Call)
    {
        case Call_Auth:
            {
                //remove ""
                QString AuthKey = ReplyData.remove(0,1);
                AuthKey.remove(AuthKey.size() - 1,1);

                //Set the key
                CurrentUser.SetAuthKey(AuthKey);
                CurrentUser.SetAuthenticated(true);

                emit ChangeStatus("Completed",StatusTimeout);
            }
        break;

        case Call_GetLibrary:
            Anime_Database.ParseMultipleJson(ReplyData);
            emit ChangeStatus("Completed",StatusTimeout);
        break;

        case Call_UpdateLibrary:
            emit ChangeStatus("Completed",StatusTimeout);
        break;
    }

    return Api_Success;
}

/***************************************************************************
 * This functions sends out a GET call and returns the reply once recieved
 ***************************************************************************/
QNetworkReply *ApiManager::DoHttpGet(QNetworkAccessManager *NetworkManager, const QUrl &Url, bool HummingbirdHeader)
{
    QNetworkRequest Request(Url);

    //Set the hummingbird api headers
    if(HummingbirdHeader)
    {
        Request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        Request.setRawHeader("X-Mashape-Authorization",HUMMINGBIRD_API_KEY);
    }

    QNetworkReply *Reply = NetworkManager->get(Request);
    Reply->setProperty("Timeout",false);

    //Wait for the reply and add a timeout period
    QTimer Timer;
    Timer.setSingleShot(true);
    Timer.setInterval(10000);

    QEventLoop Loop;
    QObject::connect(&Timer,SIGNAL(timeout()),&Loop,SLOT(quit()));
    QObject::connect(&Timer,&QTimer::timeout,[=]() { Reply->setProperty("Timeout",true); });
    QObject::connect(Reply,SIGNAL(finished()),&Loop,SLOT(quit()));
    Timer.start();
    Loop.exec();

    return Reply;
}

/***************************************************************************
 * This functions sends out a POST call and returns the reply once recieved
 ***************************************************************************/
QNetworkReply *ApiManager::DoHttpPost(QNetworkAccessManager *NetworkManager, const QUrl &Url, QString Data, bool HummingbirdHeader)
{
    QNetworkRequest Request(Url);

    //Set the hummingbird api headers
    if(HummingbirdHeader)
    {
        Request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        Request.setRawHeader("X-Mashape-Authorization",HUMMINGBIRD_API_KEY);
    }

    QNetworkReply *Reply = NetworkManager->post(Request,Data.toUtf8());
    Reply->setProperty("Timeout",false);

    //Wait for the reply and add a timeout period
    QTimer Timer;
    Timer.setSingleShot(true);
    Timer.setInterval(10000);

    QEventLoop Loop;
    QObject::connect(&Timer,SIGNAL(timeout()),&Loop,SLOT(quit()));
    QObject::connect(&Timer,&QTimer::timeout,[=]() { Reply->setProperty("Timeout",true); });
    QObject::connect(Reply,SIGNAL(finished()),&Loop,SLOT(quit()));
    Timer.start();
    Loop.exec();

    return Reply;
}


