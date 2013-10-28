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

#include "animedatabase.h"

#include <QApplication>
#include <QVariantMap>
#include <QVariantList>

using namespace Anime;

//Make class global
AnimeDatabase Anime_Database;

AnimeDatabase::AnimeDatabase(QObject *parent) :
    QObject(parent)
{
}

AnimeDatabase::~AnimeDatabase()
{
    //Delete the anime entires since they are not part of the Qt parent heirarchy
    foreach(AnimeEntity *Anime, Database.values())
    {
        QString Slug = Database.key(Anime);
        Database.remove(Slug);
        delete Anime;
    }
}

/*****************************************************
 * Adds anime to the database
 * if anime already exists it will overwrite the data
 *****************************************************/
void AnimeDatabase::AddAnime(AnimeEntity *Anime)
{
    //if anime exits then check if the last watched date is more recent
    if(Database.contains(Anime->GetAnimeSlug()))
    {
        AnimeEntity *OldAnime = GetAnime(Anime->GetAnimeSlug());
        if(OldAnime->GetUserInfo().GetLastWatched() < Anime->GetUserInfo().GetLastWatched())
        {
            //Move the userinfo of the old anime to the new one, incase the new anime information has been updated
            UserAnimeInformation OldInfo = Anime->GetUserInfo();
            Anime->SetUserInfo(OldInfo);
        }
    }

    Database.insert(Anime->GetAnimeSlug(),Anime);
}

/**************************************************************
 * Removes anime from the database
 * If databse doesn't contain the anime it will return false
 **************************************************************/
bool AnimeDatabase::RemoveAnime(QString Slug)
{
    if(!Database.contains(Slug)) return false;
    Database.remove(Slug);
    return true;
}

bool AnimeDatabase::RemoveAnime(AnimeEntity *Anime)
{
    if(!Database.contains(Anime->GetAnimeSlug())) return false;
    Database.remove(Anime->GetAnimeSlug());
    return true;
}


/*******************************************************
 * Retrieves anime from the database based on the slug
 * Should be used in conjunction with 'Contains'
 *******************************************************/
AnimeEntity* AnimeDatabase::GetAnime(QString Slug) const
{
    if(Database.contains(Slug))
        return Database.value(Slug);

    return nullptr;
}

/*******************************************************
 * Retrieves anime from the database based on the title
 * This may take time depending on the size of the list.
 * If no slug is found then we return 'unknown slug'
 *******************************************************/
QString AnimeDatabase::GetAnimeSlug(QString Title)
{
    QString Slug = QString(ANIMEDATABASE_UKNOWN_SLUG);

    for(auto it = Database.begin(); it != Database.end(); ++it)
    {
        AnimeEntity *Anime = it.value();
        if((Anime->GetAnimeTitle().toLower() == Title) || (Anime->GetAnimeAlternateTitle().toLower() == Title))
            Slug = Anime->GetAnimeSlug();
    }

    return Slug;
}

/********************************************
 * Gets the json data from the library
 * or from file and turns it into an entity
 *******************************************/
void AnimeDatabase::ParseJson(QByteArray Data)
{
    QJsonDocument Doc = QJsonDocument::fromJson(Data);
    QVariantMap UserInfoMap = Doc.toVariant().toMap();
    QVariantMap AnimeInfoMap = UserInfoMap.value("anime").toMap();

    //Check wether we have a slug value
    if(!AnimeInfoMap.contains("slug")) return;

    //User info
    UserAnimeInformation UserInfo;
    UserInfo.SetEpisodesWatched(UserInfoMap.value("episodes_watched",0).toInt());
    UserInfo.SetLastWatched(QDateTime::fromString(UserInfoMap.value("last_watched",QDateTime::currentDateTime().toString("yyyy-dd-MMThh:mm:ssZ")).toString(),"yyyy-dd-MMThh:mm:ssZ"));
    UserInfo.SetRewatchedTimes(UserInfoMap.value("rewatched_times",0).toInt());
    UserInfo.SetNotes(UserInfoMap.value("notes","").toString());
    UserInfo.SetNotePresent(UserInfoMap.value("notes_present",false).toBool());
    UserInfo.SetStatus(UserInfoMap.value("status","unknown").toString());
    UserInfo.SetPrivate(UserInfoMap.value("private",false).toBool());
    UserInfo.SetRewatching(UserInfoMap.value("rewatching",false).toBool());
    UserInfo.SetRatingType(UserInfoMap.value("rating").toMap().value("type","basic").toString());
    UserInfo.SetRatingValue(UserInfoMap.value("rating").toMap().value("value",-1).toInt());

    //Anime info
    AnimeEntity *Entity = new AnimeEntity();
    Entity->SetAnimeSlug(AnimeInfoMap.value("slug","").toString());
    Entity->SetAnimeStatus(AnimeInfoMap.value("status","unknown").toString());
    Entity->SetAnimeUrl(AnimeInfoMap.value("url","").toString());
    Entity->SetAnimeTitle(AnimeInfoMap.value("title","").toString());
    Entity->SetAnimeAlternateTitle(AnimeInfoMap.value("alternate_title","").toString());
    Entity->SetAnimeEpisodeCount(AnimeInfoMap.value("episode_count",-1).toInt());
    Entity->SetAnimeImage(AnimeInfoMap.value("cover_image","").toString());
    Entity->SetAnimeSynopsis(AnimeInfoMap.value("synopsis","").toString());
    Entity->SetAnimeShowType(AnimeInfoMap.value("show_type","").toString());

    //Go through each genre
    if(AnimeInfoMap.contains("genres"))
    {
        QVariantList GenreList = AnimeInfoMap.value("genres").toList();
        foreach(QVariant Genre, GenreList)
        {
            QVariantMap GenreMap = Genre.toMap();
            Entity->AddAnimeGenre(GenreMap.value("name").toString());
        }
    }

    Entity->SetUserInfo(UserInfo);

    //Now add it to the list
    AddAnime(Entity);

}
