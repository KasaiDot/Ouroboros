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

/**************************************************************************
 * This functions sends out a GET call and returns the reply once recieved
 **************************************************************************/
QNetworkReply *ApiManager::DoHttpGet(QNetworkAccessManager *NetworkManager, const QUrl &Url)
{
    QNetworkRequest Request(Url);

    //Set the hummingbird api headers
    Request.setRawHeader("X-Mashape-Authorization",HUMMINGBIRD_API_KEY);

    QNetworkReply *Reply = NetworkManager->get(Request);

    //Wait for the reply
    QEventLoop Loop;
    QObject::connect(Reply,SIGNAL(finished()),&Loop,SLOT(quit()));
    Loop.exec();

    return Reply;
}

/**************************************************************************
 * This functions sends out a POST call and returns the reply once recieved
 **************************************************************************/
QNetworkReply *ApiManager::DoHttpPost(QNetworkAccessManager *NetworkManager, const QUrl &Url, QString Data)
{
    QNetworkRequest Request(Url);

    //Set the hummingbird api headers
    Request.setRawHeader("X-Mashape-Authorization",HUMMINGBIRD_API_KEY);

    QNetworkReply *Reply = NetworkManager->post(Request,Data.toUtf8());

    //Wait for the reply
    QEventLoop Loop;
    QObject::connect(Reply,SIGNAL(finished()),&Loop,SLOT(quit()));
    Loop.exec();

    return Reply;
}


