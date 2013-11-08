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

#include "filemanager.h"

#include <QFile>
#include <QDir>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariantMap>
#include <QDebug>
#include <QScopedPointer>

#include "apimanager.h"
#include "queuemanager.h"

using namespace Manager;
FileManager File_Manager;

FileManager::FileManager()
{
    //Files
    FileManagerInfo.UserInfoFile = "UserInfo.json";
    FileManagerInfo.UserQueueFile = "Queue.json";

    //Dirs
    FileManagerInfo.DataFolderPath = "/Data/";
    FileManagerInfo.UserFolderPath = FileManagerInfo.DataFolderPath + "Users/";
    FileManagerInfo.DatabaseFolderPath = FileManagerInfo.DataFolderPath + "Database/";
    FileManagerInfo.UserAnimePath = FileManagerInfo.UserFolderPath + "<user>/Anime/"; // <user> can be replaced easily with the user's Username
    FileManagerInfo.DatabaseAnimePath = FileManagerInfo.DatabaseFolderPath + "Anime/";
    FileManagerInfo.DatabaseImagePath = FileManagerInfo.DatabaseFolderPath + "Images/";

}

/********************************************************
 * Checks if Directory exists, if not then it creates it
 *******************************************************/
void FileManager::CheckDir(QString Filepath)
{
    if(!QDir(Filepath).exists())
        QDir().mkpath(Filepath);
}

/**************************
 * Checks if a file exists
 **************************/
bool FileManager::FileExists(QString Filename)
{
    return QFile(Filename).exists();
}

bool FileManager::FileExists(QString Filepath, QString Filename)
{
   return FileExists(Filepath.append(Filename));
}

/********************************************************
 * Saves user information such as username and password
 *******************************************************/
bool FileManager::SaveUserInformation()
{
    if(!CurrentUser.isValid()) return false;

    //Create the JSON file
    //We first have to create JsonValues which will hold the values for the fields
    QJsonValue UsernameField(CurrentUser.GetUsername());
    QJsonValue PasswordField(QString(CurrentUser.GetBase64Password()));

    //Then we add those to a json object
    QJsonObject JsonObject;
    JsonObject.insert("Username",UsernameField);
    JsonObject.insert("Password",PasswordField);

    //We then convert it to a document
    QJsonDocument JsonDoc(JsonObject);

    //Prepare file
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.DataFolderPath ;
    QString Filename = FileManagerInfo.UserInfoFile;

    if(!WriteDataToFile(Filepath,Filename,JsonDoc.toJson())) return false;

    return true;
}

/***********************************************
 * Cycles through every entity in the database
 * and calls SaveAnimeEntity
 **********************************************/
void FileManager::SaveAnimeDatabase()
{
    QList<Anime::AnimeEntity*> AnimeList = Anime_Database.GetAnimeEntities();
    for(QList<Anime::AnimeEntity*>::Iterator AnimeIt = AnimeList.begin(); AnimeIt != AnimeList.end(); ++AnimeIt)
    {
        SaveAnimeEntity((*AnimeIt));
    }

}

/*******************************
 * Save an anime entity to file
 ******************************/
bool FileManager::SaveAnimeEntity(Anime::AnimeEntity *Entity, bool SaveUserInfo)
{
    //Filename will always be the slug of the anime
    QString Filename = Entity->GetAnimeSlug().append(".json");

    //UserInfo
    if(SaveUserInfo)
    {
        if(!CurrentUser.isValid()) return false;
        QJsonDocument UserJson = Entity->ConstructUserJsonDocument();

        //prepare file
        QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserAnimePath;
        Filepath.replace(QString("<user>"),CurrentUser.GetUsername());

        if(!WriteDataToFile(Filepath,Filename,UserJson.toJson())) return false;
    }

    //AnimeInfo
    QJsonDocument AnimeJson = Entity->ConstructAnimeJsonDocument();
    QString AnimeFilepath = QApplication::applicationDirPath() + FileManagerInfo.DatabaseAnimePath;

    if(!WriteDataToFile(AnimeFilepath,Filename,AnimeJson.toJson())) return false;

    //Save the image
    SaveAnimeImage(Entity);

    return true;

}

/*****************************************************
 * Gets anime image and saves it to the Image folder
 *****************************************************/
bool FileManager::SaveAnimeImage(Anime::AnimeEntity *Entity)
{
    QString ImageFilePath = QApplication::applicationDirPath() + FileManagerInfo.DatabaseImagePath;
    QString ImageFileName = Entity->GetAnimeSlug() + ".png";

    CheckDir(ImageFilePath);

    if(!QFile::exists(QString(ImageFilePath + ImageFileName)))
    {
        QScopedPointer<QNetworkAccessManager> NetworkManager(new QNetworkAccessManager);
        QNetworkReply *Reply = Api_Manager.GetAnimeImage(NetworkManager.data(),QUrl(Entity->GetAnimeImage()));
        if(Reply->error()) return false;

        QByteArray ImageData = Reply->readAll();
        Reply->deleteLater();

        QFile Image(ImageFilePath.append(ImageFileName));
        if(!Image.open(QIODevice::WriteOnly))
            return false;

        Image.write(ImageData);
        Image.close();
    }

    return true;
}

/*******************************************************************
 * Saves the slug of the queue item that is of type update library
 ******************************************************************/
bool FileManager::SaveQueue()
{
    if(Queue_Manager.GetQueueSize() <= 0) return false;

    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath ;
    QString Filename = FileManagerInfo.UserQueueFile;

    QByteArray Data = Queue_Manager.ConstructQueueJsonDocument().toJson();

    if(!WriteDataToFile(Filepath,Filename,Data)) return false;

    return true;
}

/*************************
 * Loads user information
 *************************/
bool FileManager::LoadUserInformation()
{
    //Prepare file
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.DataFolderPath ;
    QString Filename = FileManagerInfo.UserInfoFile;

    QByteArray Data = ReadFile(Filepath,Filename);
    if(Data.isNull() || Data.isEmpty()) return false;


    QVariantMap UserMap = QJsonDocument::fromJson(Data).toVariant().toMap();
    if(UserMap.size() <= 0) return false;

    if(!(UserMap.contains("Username") || UserMap.contains("Password"))) return false;

    QString Username = UserMap.value("Username").toString();
    QByteArray Password = UserMap.value("Password").toByteArray();

    CurrentUser.SetUserDetails(Username,Password);
    CurrentUser.SetAuthenticated(false);
    CurrentUser.SetAuthKey("");

    return true;
}

/**************************************************************
 * Gets the list of all the anime in the /<user>/Anime folder
 * and uses the filename to call LoadAnimeEntity
 *************************************************************/
bool FileManager::LoadAnimeDatabase()
{
    if(!CurrentUser.isValid()) return false;

    //Get File List
    QString DirPath = QApplication::applicationDirPath() + FileManagerInfo.UserAnimePath;
    DirPath.replace(QString("<user>"),CurrentUser.GetUsername());
    QDir UserDir(DirPath);

    if(!UserDir.exists()) return false;

    QStringList Filters;
    Filters << "*.json";
    QFileInfoList FileList = UserDir.entryInfoList(Filters);
    if(FileList.size() == 0) return false;

    foreach (QFileInfo File, FileList)
    {
        QString Slug = File.completeBaseName(); //Gets the name of the file without the .json
        if(!Anime_Database.Contains(Slug))
            LoadAnimeEntity(Slug);
    }

    return true;
}

/*******************************************************
 * Gets anime info and user info from file
 * and combines them, then passes them to the database
 ******************************************************/
bool FileManager::LoadAnimeEntity(QString Slug)
{
    if(!CurrentUser.isValid()) return false;

    //Get the user and anime files
    QString Filename = Slug + ".json";

    QString UserFilePath = QApplication::applicationDirPath() + FileManagerInfo.UserAnimePath;
    UserFilePath.replace(QString("<user>"),CurrentUser.GetUsername());

    QString AnimeFilePath = QApplication::applicationDirPath() + FileManagerInfo.DatabaseAnimePath;

    //Check wether they exist
    //TODO: if anime file is not found, then get it via the api
    if(!(FileExists(UserFilePath,Filename) || FileExists(AnimeFilePath,Filename))) return false;

    //Read files
    QByteArray UserArray = ReadFile(UserFilePath,Filename);
    if(UserArray.isNull() || UserArray.isEmpty()) return false;

    QByteArray AnimeArray = ReadFile(AnimeFilePath,Filename);
    if(AnimeArray.isNull() || AnimeArray.isEmpty()) return false;

    //We have to combine the arrays before parsing it
    QJsonObject UserObject(QJsonObject::fromVariantMap(QJsonDocument::fromJson(UserArray).toVariant().toMap()));
    QJsonObject AnimeObject(QJsonObject::fromVariantMap(QJsonDocument::fromJson(AnimeArray).toVariant().toMap()));

    QJsonValue AnimeValue(AnimeObject);
    UserObject.insert("anime",AnimeValue);

    //Now we can pass it to the database to handle
    QJsonDocument Doc(UserObject);
    Anime_Database.ParseJson(Doc.toJson());

    return true;
}

/*********************************************************************
 * Gets slugs from queue file and appends them to queue for updating
 *********************************************************************/
bool FileManager::LoadQueue()
{
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath ;
    QString Filename = FileManagerInfo.UserQueueFile;

    QByteArray Data = ReadFile(Filepath,Filename);
    if(Data.isNull() || Data.isEmpty()) return false;

    Queue_Manager.ParseQueueJson(Data);

    return true;
}

/********************************
 * Saves data to given filename
 ********************************/
bool FileManager::WriteDataToFile(QString Filepath, QString Filename, QByteArray Data)
{
    CheckDir(Filepath);

    QFile File(Filepath.append(Filename));

    if(!File.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;

    File.write(Data);
    File.close();

    return true;
}

/*************************************
 * Reads and returns contents of file
 *************************************/
QByteArray FileManager::ReadFile(QString Filepath, QString Filename)
{
    QByteArray Data;

    CheckDir(Filepath);

    QFile File(Filepath.append(Filename));

    if(!File.exists()) return Data;
    if(!File.open(QIODevice::ReadOnly)) return Data;

    //Read data
    Data = File.readAll();

    File.close();

    return Data;
}
