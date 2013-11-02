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

#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QScopedPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#include "private.h"

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

        Api_Success
    };

    //Api calls
    ApiReturnStatus Authenticate();
    ApiReturnStatus GetLibrary();

    QNetworkReply* GetAnimeImage(QNetworkAccessManager *NetworkManager, QUrl const &Url);

private:

    //Types of api calls
    enum ApiCall
    {
        Call_Auth,
        Call_GetLibrary
    };

    QNetworkReply* DoHttpGet(QNetworkAccessManager *NetworkManager, QUrl const &Url, bool HummingbirdHeader = true);
    QNetworkReply* DoHttpPost(QNetworkAccessManager *NetworkManager, QUrl const &Url, QString Data, bool HummingbirdHeader = true);

    ApiReturnStatus ProcessReply(QNetworkReply *Reply, ApiCall Call);

signals:

public slots:

};

}

extern Manager::ApiManager Api_Manager;

#endif // APIMANAGER_H
