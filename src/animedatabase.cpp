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

#include "filemanager.h"
#include "guimanager.h"
#include "historymanager.h"

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
    ClearDatabase();
}

/***************************************************
 * Removes and deletes all anime from the database
 **************************************************/
void AnimeDatabase::ClearDatabase()
{
    foreach(AnimeEntity *Anime, Database.values())
    {
       RemoveAnime(Anime);
    }
}

/*****************************************************
 * Adds anime to the database
 * if anime already exists it will overwrite the data
 *****************************************************/
void AnimeDatabase::AddAnime(AnimeEntity *Anime)
{
    if(Anime->GetAnimeSlug().isEmpty()) return;

    //if anime exits then check if the last watched date is more recent
    if(Database.contains(Anime->GetAnimeSlug()))
    {
        //get the old anime from the database since we haven't replaced it
        AnimeEntity *OldAnime = GetAnime(Anime->GetAnimeSlug());

        //check if the old informations' last watched was later than the last watched returned by the api
        if(OldAnime->GetUserInfo()->LastWatchedLaterThan(Anime->GetUserInfo()->GetLastWatched()))
        {
            //Move the userinfo of the old anime to the new one, incase the anime information (not user information) has been updated
            UserAnimeInformation *OldInfo = OldAnime->GetUserInfo();
            Anime->SetUserInfo(*OldInfo);
        }
    }

    //replace the old anime
    Database.insert(Anime->GetAnimeSlug(),Anime);
}

/*******************************************************
 * Adds anime to database with custom user information
 *******************************************************/
void AnimeDatabase::NewAnime(AnimeEntity *Anime, QString UserStatus)
{
    UserAnimeInformation Info;
    Info.SetEpisodesWatched(0,true);
    Info.SetStatus(UserStatus);
    Info.SetRewatchedTimes(0);
    Info.SetRatingType("advanced"); //default is advanced

    if(UserStatus == STATUS_COMPLETED && Anime->GetAnimeEpisodeCount() > ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        Info.SetEpisodesWatched(Anime->GetAnimeEpisodeCount());

    Anime->SetUserInfo(Info);
    AddAnime(Anime);
}

/**************************************************************
 * Removes anime from the database
 * If databse doesn't contain the anime it will return false
 **************************************************************/
AnimeEntity* AnimeDatabase::RemoveAnime(QString Slug, bool Delete)
{
    if(!Database.contains(Slug)) return nullptr;

    AnimeEntity *Anime = GetAnime(Slug);
    RemoveAnime(Anime,Delete);

    if(Delete)
        return nullptr;

    return Anime;
}

bool AnimeDatabase::RemoveAnime(AnimeEntity *Anime, bool Delete)
{
    if(!Database.contains(Anime->GetAnimeSlug())) return false;
    Database.remove(Anime->GetAnimeSlug());
    if(Delete) delete Anime;
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
QString AnimeDatabase::GetAnimeSlug(QString Title, bool isCleanTitle, bool Strict)
{
    QString Slug = QString(ANIMEDATABASE_UKNOWN_SLUG);

    for(auto it = Database.begin(); it != Database.end(); ++it)
    {
        AnimeEntity *Anime = it.value();
        if(Strict)
        {
            if((Anime->GetAnimeTitle(isCleanTitle).trimmed() == Title.trimmed()) || (Anime->GetAnimeAlternateTitle(isCleanTitle).trimmed() == Title.trimmed()))
                Slug = Anime->GetAnimeSlug();
        } else {

            if(Anime->GetAnimeTitle(isCleanTitle).toLower() == Title.toLower() || Anime->GetAnimeAlternateTitle(isCleanTitle).toLower() == Title.toLower() ||
                    Anime->GetAnimeTitle(isCleanTitle).contains(Title,Qt::CaseInsensitive) || Anime->GetAnimeAlternateTitle(isCleanTitle).contains(Title,Qt::CaseInsensitive))
                Slug = Anime->GetAnimeSlug();
        }
    }

    return Slug;
}

/********************************************
 * Gets the json data from the library
 * or from file and turns it into an entity
 *******************************************/
void AnimeDatabase::ParseJson(QByteArray Data)
{
    AnimeEntity *Entity = new AnimeEntity();
    if(!Entity->ParseAnimeJson(Data,true))
    {
        delete Entity;
        return;
    }

    //Now add it to the list
    AddAnime(Entity);

}

/*************************************************
 * Splits json data up and calls parse json
 * used when you have multiple anime information
 *************************************************/
void AnimeDatabase::ParseMultipleJson(QByteArray Data)
{
    QJsonDocument MainDoc = QJsonDocument::fromJson(Data);
    QVariantList AnimeList = MainDoc.toVariant().toList();

    foreach(QVariant Variant,AnimeList)
    {
       QJsonDocument Doc = QJsonDocument::fromVariant(Variant);
       ParseJson(Doc.toJson());
    }
}

/***********************************************************
 * Updates anime entity based on the episode provided
 ***********************************************************/
void AnimeDatabase::UpdateEntity(AnimeEpisode &Episode, QString Slug) { UpdateEntity(Episode,GetAnime(Slug)); }
void AnimeDatabase::UpdateEntity(AnimeEpisode &Episode, AnimeEntity *Entity)
{
   if(!Entity->GetUserInfo()->Update(Episode)) return;

   //Refresh the model
   GUI_Manager.UpdateAnime(Entity);
   GUI_Manager.UpdateHummingbirdAnime(Entity->GetAnimeSlug());

   QString Action = "Watched Episode: %1";
   History_Manager.AddHistoryItem(Entity->GetAnimeTitle(),Action.arg(QString::number(Entity->GetUserInfo()->GetEpisodesWatched())),QDateTime::currentDateTime().toString(HISTORY_DATEFORMAT));
}

