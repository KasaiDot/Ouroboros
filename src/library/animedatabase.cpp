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

#include "animedatabase.h"

#include <QApplication>
#include <QVariantMap>
#include <QVariantList>

#include "library/historymanager.h"
#include "manager/filemanager.h"
#include "ui/guimanager.h"

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
        if(OldAnime->GetUserInfo()->isLastWatchedRecentThan(Anime->GetUserInfo()->GetLastWatched()))
        {
            //Move the userinfo of the old anime to the new one, incase the anime information (not user information) has been updated
            UserAnimeInformation *OldInfo = OldAnime->GetUserInfo();
            QStringList RecognitionTitles = OldAnime->GetRecognitionTitles();
            Anime->SetUserInfo(*OldInfo);
            Anime->SetRecognitionTitles(RecognitionTitles);

            //Check if the new anime episode count increased compared to old one, if so move that anime to currently watching
            if(OldInfo->GetStatus() == STATUS_COMPLETED)
            {
                if((Anime->GetAnimeEpisodeCount() > OldAnime->GetAnimeEpisodeCount()) &&
                        (Anime->GetAnimeEpisodeCount() != ANIMEENTITY_UNKNOWN_ANIME_EPISODE || OldAnime->GetAnimeEpisodeCount() != ANIMEENTITY_UNKNOWN_ANIME_EPISODE))
                {
                    //move the anime to currently watching list
                    Anime->GetUserInfo()->SetStatus(STATUS_CURRENTLY_WATCHING);
                }
            }

            //Save the new anime
            File_Manager.SaveAnimeEntity(Anime);

            //since the old information is not newer than the information we got from the api, we check to see if the anime episode count is valid
        }else{
            if(Anime->GetUserInfo()->GetEpisodesWatched() <= ANIMEENTITY_UNKNOWN_USER_EPISODE)
            {
                //if the episode count returned was unknown, then we set the episode count of the old anime
                Anime->GetUserInfo()->SetEpisodesWatched(OldAnime->GetUserInfo()->GetEpisodesWatched());
            }

            //Move the priorities over
            Anime->GetUserInfo()->SetPriority(OldAnime->GetUserInfo()->GetPriority());
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

AnimeEntity *AnimeDatabase::GetAnime(AnimeEpisode &Episode, bool Strict)
{
    for(auto it = Database.begin(); it != Database.end(); ++it)
    {
        AnimeEntity *Anime = it.value();
        if(CompareEpisode(Episode,Anime,Strict))
            return GetAnime(Episode.Slug);
    }

    return nullptr;
}

/***************************************************
 * Compares the anime with the episode provided
 ****************************************************/
bool AnimeDatabase::CompareEpisode(AnimeEpisode &Episode, AnimeEntity *Entity, bool Strict)
{
    //Since detection works with clean titles, we leave it if it is empty
    if(Episode.CleanTitle.isEmpty()) return false;

    bool Found = CompareCleanTitles(Episode,Entity,Strict);

    if(!Found) return false;

    //Assume user is watching episode 1 if the anime is one episode long
    if(Episode.Number.isEmpty() && Entity->GetAnimeEpisodeCount() == 1)
        Episode.Number = "1";

    Episode.Slug = Entity->GetAnimeSlug();
    return true;
}

/*********************************************************************
 * Compares clean titles of the episode to that of the anime entity
 **********************************************************************/
bool AnimeDatabase::CompareCleanTitles(AnimeEpisode &Episode, AnimeEntity *Entity, bool Strict)
{

    //Check if the entity contains clean titles
    if(!Entity->GetCleanTitles().size() > 0) Entity->CleanAllTitles();

    //Check again to see if it has any clean titles
    if(!Entity->GetCleanTitles().size() > 0) return false;

    //Compare all the titles in the anime with the episode clean title
    foreach (QString CleanTitle, Entity->GetCleanTitles())
    {
        //Compare with Title + number
        if(Strict && Entity->GetAnimeEpisodeCount() == 1 && !Episode.Number.isEmpty())
        {
            if(QString(Episode.CleanTitle.trimmed() + Episode.Number).trimmed() == CleanTitle.trimmed())
            {
                Episode.Title.append(Episode.Number);
                Episode.Number.clear();
                return true;
            }
        }

        //Compare with title
        if(Strict)
        {
            if(CleanTitle.trimmed() == Episode.CleanTitle.trimmed()) return true;
        } else {
            if(CleanTitle.trimmed().contains(Episode.CleanTitle.trimmed(),Qt::CaseInsensitive)) return true;
        }
    }

    //Failed
    return false;
}


/*******************************************************
 * Retrieves anime from the database based on the title
 * This may take time depending on the size of the list.
 * If no slug is found then we return 'unknown slug'
 *******************************************************/
QString AnimeDatabase::GetAnimeSlug(QString Title, bool Strict)
{
    QString Slug = QString(ANIMEDATABASE_UKNOWN_SLUG);

    for(auto it = Database.begin(); it != Database.end(); ++it)
    {
        AnimeEntity *Anime = it.value();
        QString TrimmedTitle = Title.trimmed();
        QString TrimmedAnimeTitle = Anime->GetAnimeTitle().trimmed();
        QString TrimmedAnimeAlternateTitle = Anime->GetAnimeAlternateTitle().trimmed();

        if(Strict)
        {
            if((TrimmedAnimeTitle == TrimmedTitle) || (TrimmedAnimeAlternateTitle == TrimmedTitle))
                Slug = Anime->GetAnimeSlug();
        } else {

            if(TrimmedAnimeTitle.toLower() == TrimmedTitle.toLower() || TrimmedAnimeAlternateTitle.toLower() == TrimmedTitle.toLower() ||
                    TrimmedAnimeTitle.contains(TrimmedTitle,Qt::CaseInsensitive) || TrimmedAnimeAlternateTitle.contains(TrimmedTitle,Qt::CaseInsensitive))
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
    GUI_Manager.UpdateOnlineLibrary(Entity->GetAnimeSlug());

    QString Action = "Watched Episode: %1";
    History_Manager.AddHistoryItem(Entity->GetAnimeTitle(),Action.arg(QString::number(Entity->GetUserInfo()->GetEpisodesWatched())),QDateTime::currentDateTime().toString(HISTORY_DATEFORMAT));
}

