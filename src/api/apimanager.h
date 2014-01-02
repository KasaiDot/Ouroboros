/*
**  Ouroboros, Hummingbird.me Desktop App
**  Copyright (C) 2014, Mikunj Varsani
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

#ifndef APIMANAGER_H
#define APIMANAGER_H

//*********************************************************************************************************
//Api call urls
#define API_URL_AUTH "https://hummingbirdv1.p.mashape.com/users/authenticate"
#define API_URL_GETLIBRARY "https://hummingbirdv1.p.mashape.com/users/<username>/library?status=<status>&auth_token=<key>"
#define API_URL_GETANIME "https://hummingbirdv1.p.mashape.com/anime/<slug>"
#define API_URL_UPDATELIBRARY "https://hummingbirdv1.p.mashape.com/libraries/<slug>"
#define API_URL_SEARCH "https://hummingbirdv1.p.mashape.com/search/anime?query=<searchtext>&auth_token=<key>"
//*********************************************************************************************************

#include <QObject>
#include <QScopedPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#include "ouroboros/private.h"

namespace Manager
{
class ApiManager : public QObject
{
    Q_OBJECT
public:
    explicit ApiManager(QObject *parent = 0);

    //Api return codes
    enum ApiReturnStatus
    {
        Reply_Error,
        Reply_Timeout,
        Reply_UnknownError,
        Api_Success,
        Api_Failure,
        Api_AlreadyAuth,
        Api_InvalidCredentials,
        Api_NotAuthed
    };

    //Api calls
    ApiReturnStatus Authenticate();
    ApiReturnStatus GetLibrary(QString Status);
    ApiReturnStatus GetAnime(QString Slug);
    ApiReturnStatus UpdateLibrary(QString Slug);
    QByteArray Search(QString String);

    QNetworkReply* GetAnimeImage(QNetworkAccessManager *NetworkManager, QUrl const &Url);

private:

    //Types of api calls
    enum ApiCall
    {
        Call_Auth,
        Call_GetLibrary,
        Call_UpdateLibrary
    };

    QNetworkReply* DoHttpGet(QNetworkAccessManager *NetworkManager, QUrl const &Url, bool HummingbirdHeader = true);
    QNetworkReply* DoHttpPost(QNetworkAccessManager *NetworkManager, QUrl const &Url, QString Data, bool HummingbirdHeader = true);

    ApiReturnStatus ProcessReply(QNetworkReply *Reply, ApiCall Call);

signals:
    void ChangeStatus(QString String, int Timeout = 0);

public slots:

};

}

extern Manager::ApiManager Api_Manager;

#endif // APIMANAGER_H
