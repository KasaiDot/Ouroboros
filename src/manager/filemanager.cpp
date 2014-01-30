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
#include <QFileDialog>
#include <QDebug>

#include "api/apimanager.h"
#include "api/queuemanager.h"
#include "library/historymanager.h"
#include "recognition/mediamanager.h"
#include "ui/guimanager.h"

using namespace Manager;
FileManager File_Manager;

FileManager::FileManager()
{
    //Files
    FileManagerInfo.UserInfoFile = "UserInfo.json";
    FileManagerInfo.UserQueueFile = "Queue.json";
    FileManagerInfo.UserHistoryFile = "History.json";
    FileManagerInfo.MediaFile = "Media.json";

    //Dirs
    FileManagerInfo.DataFolderPath = "/Data/";
    FileManagerInfo.UserFolderPath = FileManagerInfo.DataFolderPath + "Users/";
    FileManagerInfo.DatabaseFolderPath = FileManagerInfo.DataFolderPath + "Database/";
    FileManagerInfo.UserAnimePath = FileManagerInfo.UserFolderPath + "<user>/Anime/"; // <user> can be replaced easily with the user's Username
    FileManagerInfo.DatabaseAnimePath = FileManagerInfo.DatabaseFolderPath + "Anime/";
    FileManagerInfo.DatabaseImagePath = FileManagerInfo.DatabaseFolderPath + "Images/";
    FileManagerInfo.ThemeFolderPath = FileManagerInfo.DataFolderPath + "Theme/";

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
    if(!CurrentUser.isValid()) return false;

    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath + CurrentUser.GetUsername() + "/";
    QString Filename = FileManagerInfo.UserQueueFile;

    QByteArray Data = Queue_Manager.ConstructQueueJsonDocument().toJson();

    if(!WriteDataToFile(Filepath,Filename,Data)) return false;

    return true;
}

/**********************
 * Saves user history
 **********************/
bool FileManager::SaveHistory()
{
    if(!CurrentUser.isValid()) return false;

    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath + CurrentUser.GetUsername() + "/";
    QString Filename = FileManagerInfo.UserHistoryFile;

    QByteArray Data = History_Manager.ConstructHistoryJsonDocument().toJson();

    if(!WriteDataToFile(Filepath,Filename,Data)) return false;

    return true;
}

/*******************************************************
 * Saves Media.json to data folder if there isn't one
 *******************************************************/
bool FileManager::SaveMedia()
{
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.DataFolderPath;
    QString Filename = FileManagerInfo.MediaFile;

    if(QFile(QString(Filepath).append(Filename)).exists()) return false;

    QByteArray Data = QJsonDocument::fromJson(Media_Manager.ConstructJson()).toJson(); //This will format the code properley

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

/************************************
 * Gets the anime image from file
 ***********************************/
QByteArray FileManager::GetAnimeImage(QString Slug)
{
    QString ImageFilePath = QApplication::applicationDirPath() + FileManagerInfo.DatabaseImagePath;
    QString ImageFileName = Slug + ".png";

    CheckDir(ImageFilePath);

    //Check if image exists, if not get it and save it
    if(!QFile::exists(QString(ImageFilePath + ImageFileName)))
    {
        if(Anime_Database.Contains(Slug))
            SaveAnimeImage(Anime_Database.GetAnime(Slug));
    }

    //Read the file and return the contents
    return ReadFile(ImageFilePath,ImageFileName);

}

/*********************************************************************
 * Gets slugs from queue file and appends them to queue for updating
 *********************************************************************/
bool FileManager::LoadQueue()
{
    if(!CurrentUser.isValid()) return false;

    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath + CurrentUser.GetUsername() + "/";
    QString Filename = FileManagerInfo.UserQueueFile;

    QByteArray Data = ReadFile(Filepath,Filename);
    if(Data.isNull() || Data.isEmpty()) return false;

    Queue_Manager.ParseQueueJson(Data);

    return true;
}

/***************************************************
 * Loads all history item from the user directory
 ***************************************************/
bool FileManager::LoadHistory()
{
    if(!CurrentUser.isValid()) return false;

    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.UserFolderPath + CurrentUser.GetUsername() + "/";
    QString Filename = FileManagerInfo.UserHistoryFile;

    QByteArray Data = ReadFile(Filepath,Filename);
    if(Data.isNull() || Data.isEmpty()) return false;

    History_Manager.ParseHistoryJson(Data);

    return true;
}

/****************************************************
 * Loads the media file needed for the media manager
 ****************************************************/
bool FileManager::LoadMedia()
{
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.DataFolderPath;
    QString Filename = FileManagerInfo.MediaFile;

    QByteArray Data = ReadFile(Filepath,Filename);
    if(Data.isNull() || Data.isEmpty()) return false;

    Media_Manager.Load(Data);

    return true;
}

/**********************************************
 *  Loads a style file and return the data
 * ********************************************/
QByteArray FileManager::LoadTheme(QString Filename)
{
    QString Filepath = QApplication::applicationDirPath() + FileManagerInfo.ThemeFolderPath;
    return ReadFile(Filepath,Filename);
}

/**********************************************************
 *  Gets all the list of files inside the theme folder
 **********************************************************/
QStringList FileManager::GetThemeList()
{
    return QDir(QApplication::applicationDirPath() + FileManagerInfo.ThemeFolderPath).entryList(QDir::Files,QDir::Name);
}

/***********************************************************
 * Combines all user information of an anime into one file
 ***********************************************************/
bool FileManager::ExportAnime()
{
    //First get the file path
    QFileDialog Dialog;
    Dialog.setAcceptMode(QFileDialog::AcceptSave);
    Dialog.setNameFilter("Json File (*.json)");
    if(Dialog.exec() == QFileDialog::Rejected) return false;

    QString Path = Dialog.selectedFiles().at(0);
    QFileInfo File(Path);

    //Seperate the file name and the path so we can use them in the save function
    QString Filepath = File.absolutePath() + QDir::separator();
    QString Filename = File.fileName();

    //Get all the entities
    QList<Anime::AnimeEntity *> AnimeList = Anime_Database.GetAnimeEntities();
    QJsonArray MainArray;

    //Go through each entity and add it to the list
    foreach(Anime::AnimeEntity *Entity, AnimeList)
    {
        //We have to combine the arrays before parsing it
        QJsonObject UserObject(QJsonObject::fromVariantMap(Entity->ConstructUserJsonDocument().toVariant().toMap()));
        QJsonObject AnimeObject(QJsonObject::fromVariantMap(Entity->ConstructAnimeJsonDocument().toVariant().toMap()));

        QJsonValue AnimeValue(AnimeObject);
        UserObject.insert("anime",AnimeValue);

        //Add it to the main array
        QJsonValue ObjectValue(UserObject);
        MainArray.append(ObjectValue);
    }

    return WriteDataToFile(Filepath,Filename,QJsonDocument(MainArray).toJson());
}

/***************************************
 * Imports all user data to the list
 ***************************************/
bool FileManager::ImportAnime()
{
    //Clear the slugs in import list
    Anime_Database.ClearAnimeSlugsInImportList();

    //First get the file path
    QFileDialog Dialog;
    Dialog.setNameFilter("Json File (*.json)");
    if(Dialog.exec() == QFileDialog::Rejected) return false;

    QString Path = Dialog.selectedFiles().at(0);
    QFileInfo File(Path);

    //Seperate the file name and the path so we can use them in the save function
    QString Filepath = File.absolutePath() + QDir::separator();
    QString Filename = File.fileName();

    //Make sure user knows that anime data will be overrided
    if(QMessageBox::warning(0,"Are you sure you want to import?","Importing the data selected may override existing data in the list, are you sure you want to proceed?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::No)
        return false;

    QByteArray Data = ReadFile(Filepath,Filename);
    if(Data.isNull() || Data.isEmpty()) return false;

    Anime_Database.ParseMultipleJson(Data,false,true);

    //Push anime to the update queue
    QList<QString> AnimeSlugsInImportList = Anime_Database.GetAnimeSlugsInImportList();
    foreach (QString Slug, AnimeSlugsInImportList)
    {
        Queue_Manager.UpdateLibrary(Slug);
    }

    GUI_Manager.PopulateModel();
    return true;

}

/****************************************************************
 * Deletes the anime information file inside user anime folder
 ****************************************************************/
void FileManager::DeleteAnimeEntityFile(QString Slug)
{
    //We just want to delete it from the user folder, so that offline information of the anime is still available
    if(!CurrentUser.isValid()) return;


    //Construct the paths
    QString UserFilePath = QApplication::applicationDirPath() + FileManagerInfo.UserAnimePath;
    UserFilePath.replace(QString("<user>"),CurrentUser.GetUsername());

    QString Filename = Slug + ".json";

    FM_DeleteFile(UserFilePath,Filename);
}

/********************************************
 * Deletes a directory and all of its files
 *******************************************/
void FileManager::DeleteDirectory(QString Path)
{
    if(!QDir(Path).exists()) return;
    QDir(Path).removeRecursively();
}

/****************************************************************
 * Deletes a specific file
 ***************************************************************/
void FileManager::FM_DeleteFile(QString AbsolutePath)
{
    if(!QFile(AbsolutePath).exists()) return;
    QFile(AbsolutePath).remove();
}

void FileManager::FM_DeleteFile(QString Filepath, QString Filename)
{
    FM_DeleteFile(Filepath.append(Filename));
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
