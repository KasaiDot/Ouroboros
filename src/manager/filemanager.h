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

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>

#include "library/animedatabase.h"
#include "library/animeentity.h"
#include "library/user.h"
#include "ouroboros/settings.h"

namespace Manager
{

class FileManager
{
public:
    FileManager();

    //Contains dir paths, file names etc
    struct FileInfo
    {
        //Files
        QString UserInfoFile;
        QString UserQueueFile;
        QString UserHistoryFile;
        QString MediaFile;

        //Dirs
        QString DataFolderPath;
        QString UserFolderPath;
        QString UserAnimePath;
        QString DatabaseFolderPath;
        QString DatabaseAnimePath;
        QString DatabaseImagePath;
        QString StyleFolderPath;
    } FileManagerInfo;

    //Checks if directory exists, if not then it creates one
    void CheckDir(QString Filepath);
    bool FileExists(QString Filename);
    bool FileExists(QString Filepath,QString Filename);

    /************************* Save Functions ******************************/
    bool SaveSettings();
    bool SaveUserInformation(); //Save user name and password

    //Each anime entity will have its' own file with user info
    //Using this method, we don't have to save all of the anime entities if just one has to be saved
    //Same thing applies for loading, if there is an error loading an entity, then we don't have to load everything, instead we just load that entity
    void SaveAnimeDatabase();
    bool SaveAnimeEntity(Anime::AnimeEntity *Entity, bool SaveUserInfo = true);
    bool SaveApiResponse(QString Response, QString Filename); //Used for saving anime lists
    bool SaveAnimeImage(Anime::AnimeEntity *Entity);
    bool SaveQueue();
    bool SaveHistory();
    bool SaveMedia();

    bool LoadSettings();
    bool LoadUserInformation();
    bool LoadAnimeDatabase();
    bool LoadAnimeEntity(QString Slug);
    QByteArray GetAnimeImage(QString Slug);
    bool LoadQueue();
    bool LoadHistory();
    bool LoadMedia();
    QByteArray LoadStyle(QString Filename);

    QStringList GetStyleList();

    void DeleteDirectory(QString Path);

private:
    bool WriteDataToFile(QString Filepath,QString Filename,QByteArray Data);
    QByteArray ReadFile(QString Filepath,QString Filename);

};

}
extern Manager::FileManager File_Manager;

#endif // FILEMANAGER_H
