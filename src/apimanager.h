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

private:

    //Types of api calls
    enum ApiCall
    {
        Call_Auth,
        Call_GetLibrary
    };

    QNetworkReply* DoHttpGet(QNetworkAccessManager *NetworkManager, QUrl const &Url);
    QNetworkReply* DoHttpPost(QNetworkAccessManager *NetworkManager, QUrl const &Url, QString Data);

    ApiReturnStatus ProcessReply(QNetworkReply *Reply, ApiCall Call);



signals:

public slots:

};

}

extern Manager::ApiManager Api_Manager;

#endif // APIMANAGER_H
