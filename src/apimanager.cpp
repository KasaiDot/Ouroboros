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

using namespace Manager;

//Make class global
ApiManager Api_Manager;

ApiManager::ApiManager(QObject *parent) :
    QObject(parent)
{
}

/*************************************************************************
 * Processes Reply recieved through QNetworkManager based on the ApiCall
 ************************************************************************/
ApiManager::ApiReturnStatus ApiManager::ProcessReply(QNetworkReply *Reply, ApiManager::ApiCall Call)
{
    if(Reply->error() != QNetworkReply::NoError)
    {
        return Reply_Error;
        delete Reply;
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
        Request.setRawHeader("X-Mashape-Authorization",HUMMINGBIRD_API_KEY);

    QNetworkReply *Reply = NetworkManager->get(Request);

    //Wait for the reply
    QEventLoop Loop;
    QObject::connect(Reply,SIGNAL(finished()),&Loop,SLOT(quit()));
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
        Request.setRawHeader("X-Mashape-Authorization",HUMMINGBIRD_API_KEY);

    QNetworkReply *Reply = NetworkManager->post(Request,Data.toUtf8());

    //Wait for the reply
    QEventLoop Loop;
    QObject::connect(Reply,SIGNAL(finished()),&Loop,SLOT(quit()));
    Loop.exec();

    return Reply;
}


