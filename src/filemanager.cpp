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
    FileManagerInfo.UserAnimePath = FileManagerInfo.UserFolderPath + "<user>/Anime"; // <user> can be replaced easily with the user's Username
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
