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
#include <QDebug>

using namespace Manager;
FileManager File_Manager;

FileManager::FileManager()
{
    //Files
    FileManagerInfo.UserInfoFile = "Info.json";

    //Dirs
    FileManagerInfo.UserFolderPath = "/Users/";
    FileManagerInfo.DatabaseFolderPath = "/Database/";
    FileManagerInfo.UserAnimePath = FileManagerInfo.UserFolderPath + "<user>/Anime/"; // <user> can be replaced easily with the user's Username
    FileManagerInfo.DatabaseAnimePath = FileManagerInfo.DatabaseFolderPath + "Anime/";

}

/********************************************************
 * Checks if Directory exists, if not then it creates it
 *******************************************************/
void FileManager::CheckDir(QString Filepath)
{
    if(!QDir(Filepath).exists())
        QDir().mkpath(Filepath);
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
    QJsonValue PasswordField(CurrentUser.GetBase64Password());

    //Then we add those to a json object
    QJsonObject JsonObject;
    JsonObject.insert("Username",UsernameField);
    JsonObject.insert("Password",PasswordField);

    //We then convert it to a document
    QJsonDocument JsonDoc(JsonObject);

    //Prepare file
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath + CurrentUser.GetUsername() + "/" ;
    QString Filename = FileManagerInfo.UserInfoFile;

    //Make sure directory exists
    CheckDir(Filepath);

    QFile File(Filepath.append(Filename));
    if(!File.open(QIODevice::WriteOnly))
    {
        return false;
    }

    //Write the json doc to file
    File.write(JsonDoc.toJson());
    File.close();

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
        QJsonDocument UserJson = Entity->BuildUserJsonDocument();

        //prepare file
        QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserAnimePath;
        Filepath.replace(QString("<user>"),CurrentUser.GetUsername());

        CheckDir(Filepath);

        QFile UserFile(Filepath.append(Filename));
        if(!UserFile.open(QIODevice::WriteOnly))
            return false;

        UserFile.write(UserJson.toJson());
        UserFile.close();
    }

    //AnimeInfo
    QJsonDocument AnimeJson = Entity->BuildAnimeJsonDocument();
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.DatabaseAnimePath;

    CheckDir(Filepath);

    QFile AnimeFile(Filepath.append(Filename));
    if(!AnimeFile.open(QIODevice::WriteOnly))
        return false;

    AnimeFile.write(AnimeJson.toJson());
    AnimeFile.close();

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
        qDebug() << File.completeBaseName();
        QString Slug = File.completeBaseName(); //Gets the name of the file without the .json
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

    QFile UserFile(UserFilePath.append(Filename));
    QFile AnimeFile(AnimeFilePath.append(Filename));

    //Check wether they exist
    //TODO: if anime file is not found, then get it via the api
    if(!(UserFile.exists() || AnimeFile.exists())) return false;

    //Read files
    if(!UserFile.open(QIODevice::ReadOnly)) return false;
    QByteArray UserArray = UserFile.readAll();
    UserFile.close();

    if(!AnimeFile.open(QIODevice::ReadOnly)) return false;
    QByteArray AnimeArray = AnimeFile.readAll();
    AnimeFile.close();

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
