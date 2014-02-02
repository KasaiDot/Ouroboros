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

#include "animeentity.h"

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include "ouroboros/settings.h"
#include "recognition/mediamanager.h"

using namespace Anime;

AnimeEntity::AnimeEntity():
    AnimeSlug(""),
    AnimeStatus(""),
    AnimeUrl(""),
    AnimeTitle(""),
    AnimeAlternateTitle(""),
    AnimeEpisodeCount(ANIMEENTITY_UNKNOWN_ANIME_EPISODE),
    AnimeImage(""),
    AnimeSynopsis(""),
    AnimeShowType(""),
    UserInfoSet(false)
{
}

/***************************************************************************
 * This functions copies values from the entity provided to the new entity
 ***************************************************************************/
AnimeEntity::AnimeEntity(AnimeEntity &Entity)
{
    SetAnimeAlternateTitle(Entity.GetAnimeAlternateTitle());
    SetAnimeEpisodeCount(Entity.GetAnimeEpisodeCount());
    SetAnimeGenres(Entity.GetAnimeGenres());
    SetAnimeImage(Entity.GetAnimeImage());
    SetAnimeShowType(Entity.GetAnimeShowType());
    SetAnimeSlug(Entity.GetAnimeSlug());
    SetAnimeStatus(Entity.GetAnimeStatus());
    SetAnimeSynopsis(Entity.GetAnimeSynopsis());
    SetAnimeTitle(Entity.GetAnimeTitle());
    SetAnimeUrl(Entity.GetAnimeUrl());
    SetRecognitionTitles(Entity.GetRecognitionTitles());
    UserAnimeInformation *Info = Entity.GetUserInfo();
    SetUserInfo(*Info);
}

AnimeEntity::AnimeEntity(const AnimeEntity &Entity)
{
    SetAnimeAlternateTitle(Entity.GetAnimeAlternateTitle());
    SetAnimeEpisodeCount(Entity.GetAnimeEpisodeCount());
    SetAnimeGenres(Entity.GetAnimeGenres());
    SetAnimeImage(Entity.GetAnimeImage());
    SetAnimeShowType(Entity.GetAnimeShowType());
    SetAnimeSlug(Entity.GetAnimeSlug());
    SetAnimeStatus(Entity.GetAnimeStatus());
    SetAnimeSynopsis(Entity.GetAnimeSynopsis());
    SetAnimeTitle(Entity.GetAnimeTitle());
    SetAnimeUrl(Entity.GetAnimeUrl());
    SetRecognitionTitles(Entity.GetRecognitionTitles());
    SetUserInfo(*Entity.GetConstUserInfo());
}


/**************************************************************************
 * This function compares the titles and alternate titles of two entities
 **************************************************************************/
bool AnimeEntity::CompareTitle(AnimeEntity &Entity)
{
    if( (Entity.GetAnimeTitle() == GetAnimeTitle()) ||
            (Entity.GetAnimeAlternateTitle() == GetAnimeTitle()) ||
            (Entity.GetAnimeTitle() == GetAnimeAlternateTitle()) ||
            (Entity.GetAnimeAlternateTitle() == GetAnimeAlternateTitle()))
        return true;

    return false;
}

bool AnimeEntity::operator ==(const AnimeEntity &Other)
{
    //Compare the slugs since they are the key values
    if(this->GetAnimeSlug() == Other.GetAnimeSlug())
        return true;

    return false;
}

/**********************************
 * Parses Json data to the entity
 *********************************/
bool AnimeEntity::ParseAnimeJson(QByteArray Data, bool ContainsUserInfo)
{
    QJsonDocument Doc = QJsonDocument::fromJson(Data);
    QVariantMap UserInfoMap;
    QVariantMap AnimeInfoMap;

    //Set the info maps
    if(ContainsUserInfo)
    {
        UserInfoMap = Doc.toVariant().toMap();
        AnimeInfoMap = UserInfoMap.value("anime").toMap();
    } else {
        AnimeInfoMap = Doc.toVariant().toMap();
    }

    //no slug found, return
    if(!AnimeInfoMap.contains("slug")) return false;

    SetAnimeSlug(AnimeInfoMap.value("slug","").toString());
    SetAnimeStatus(AnimeInfoMap.value("status","").toString());
    SetAnimeUrl(AnimeInfoMap.value("url","").toString());
    SetAnimeTitle(AnimeInfoMap.value("title","").toString());
    SetAnimeAlternateTitle(AnimeInfoMap.value("alternate_title","").toString());
    SetAnimeImage(AnimeInfoMap.value("cover_image","").toString());
    SetAnimeSynopsis(AnimeInfoMap.value("synopsis","").toString());
    SetAnimeShowType(AnimeInfoMap.value("show_type","").toString());
    SetAnimeEpisodeCount(AnimeInfoMap.value("episode_count",ANIMEENTITY_UNKNOWN_ANIME_EPISODE).toInt());

    if(AnimeInfoMap.contains("recognition"))
    {
        QVariantList RecognitionVariantList = AnimeInfoMap.value("recognition").toList();
        QStringList  RecognitionList;
        foreach(QVariant Title, RecognitionVariantList)
            RecognitionList << Title.toString();
        SetRecognitionTitles(RecognitionList);
    }

    //Go through each genre
    if(AnimeInfoMap.contains("genres"))
    {
        QVariantList GenreList = AnimeInfoMap.value("genres").toList();
        foreach(QVariant Genre, GenreList)
        {
            QVariantMap GenreMap = Genre.toMap();
            AddAnimeGenre(GenreMap.value("name").toString());
        }
    }

    if(ContainsUserInfo)
    {
        UserAnimeInformation UserInfo;
        UserInfo.ParseUserMap(UserInfoMap);
        SetUserInfo(UserInfo);
    }

    return true;
}

/*****************************************************************
 * These functions construct Json documents for the filemanager
 ****************************************************************/
QJsonDocument AnimeEntity::ConstructAnimeJsonDocument()
{
    QJsonValue NullValue;
    //Values for the json doc
    QJsonValue AlternateTitle(GetAnimeAlternateTitle());
    QJsonValue EpisodeCount(GetAnimeEpisodeCount());
    QJsonValue ImageLink(GetAnimeImage());
    QJsonValue ShowType(GetAnimeShowType());
    QJsonValue Slug(GetAnimeSlug());
    QJsonValue Status(GetAnimeStatus());
    QJsonValue Synopsis(GetAnimeSynopsis());
    QJsonValue Title(GetAnimeTitle());
    QJsonValue Url(GetAnimeUrl());

    //check for null values
    if(GetAnimeAlternateTitle().isEmpty())
        AlternateTitle = NullValue;

    //Make an array for the genre
    QJsonArray GenreArray;
    foreach(QString GenreString, AnimeGenres)
    {
        QJsonValue GenreValue(GenreString);
        QJsonObject GenreObject;
        GenreObject.insert("name",GenreValue); // makes the "name: ..." part

        //Make it in the form
        // {
        //    name: ...
        // }
        QJsonValue GenreObjectValue(GenreObject);

        GenreArray.append(GenreObjectValue);
    }
    QJsonValue Genres(GenreArray);

    //Save recognition titles
    QJsonArray RecognitionArray;
    foreach(QString Title,RecognitionTitles)
    {
        QJsonValue TitleValue(Title);
        RecognitionArray.append(TitleValue);
    }
    QJsonValue RecognitionValue(RecognitionArray);

    //Construct the json file
    QJsonObject EntityObject;
    EntityObject.insert("slug",Slug);
    EntityObject.insert("status",Status);
    EntityObject.insert("url",Url);
    EntityObject.insert("title",Title);
    EntityObject.insert("alternate_title",AlternateTitle);
    EntityObject.insert("episode_count",EpisodeCount);
    EntityObject.insert("cover_image",ImageLink);
    EntityObject.insert("synopsis",Synopsis);
    EntityObject.insert("show_type",ShowType);
    EntityObject.insert("recognition",RecognitionValue);

    if(!GenreArray.isEmpty())
        EntityObject.insert("genres",Genres);

    QJsonDocument AnimeDoc(EntityObject);

    return AnimeDoc;
}

QJsonDocument AnimeEntity::ConstructUserJsonDocument()
{
    QJsonValue NullValue;

    //Values
    QJsonValue Slug(GetAnimeSlug());
    QJsonValue EpisodesWatched(UserInfo.GetEpisodesWatched());
    QJsonValue LastWatched(UserInfo.GetLastWatched().toString(ANIMEENTITY_DATE_FORMAT));
    QJsonValue RewatchedTimes(UserInfo.GetRewatchedTimes());
    QJsonValue Notes(UserInfo.GetNotes());
    QJsonValue NotesPresent(UserInfo.isNotePresent());
    QJsonValue Status(UserInfo.GetStatus());
    QJsonValue Private(UserInfo.isPrivate());
    QJsonValue Rewatching(UserInfo.isRewatching());
    QJsonValue Priority(UserInfo.GetPriority());

    //**********************************************************
    QJsonObject RatingObject;
    RatingObject.insert("type",UserInfo.GetRatingType());
    if(UserInfo.GetRatingValue() > 0)
        RatingObject.insert("value",UserInfo.GetRatingValue());
    else
        RatingObject.insert("value",NullValue);

    QJsonValue Rating(RatingObject);
    //**********************************************************

    //Check for null values
    if(UserInfo.GetNotes().isEmpty())
        Notes = NullValue;

    //Construct the json doc
    QJsonObject UserObject;
    UserObject.insert("slug",Slug);
    UserObject.insert("episodes_watched",EpisodesWatched);
    UserObject.insert("last_watched",LastWatched);
    UserObject.insert("rewatched_times",RewatchedTimes);
    UserObject.insert("notes",Notes);
    UserObject.insert("notes_present",NotesPresent);
    UserObject.insert("status",Status);
    UserObject.insert("private",Private);
    UserObject.insert("rewatching",Rewatching);
    UserObject.insert("rating",Rating);
    UserObject.insert("priority",Priority);

    QJsonDocument UserDoc(UserObject);

    return UserDoc;

}

/****************************************
 * Builds JSON file for api update call
 ****************************************/
QJsonObject AnimeEntity::ConstructUpdateJsonObject()
{
    QJsonValue NullValue;
    QJsonObject Object;

    Object.insert("status",UserInfo.GetStatus());
    Object.insert("privacy",UserInfo.isPrivate());
    Object.insert("rewatched_times",UserInfo.GetRewatchedTimes());
    Object.insert("episodes_watched",UserInfo.GetEpisodesWatched());
    Object.insert("increment_episodes",false);

    if(UserInfo.GetRatingValue() <= 0)
        Object.insert("rating",NullValue);
    else
        Object.insert("rating",UserInfo.GetRatingValue());

    if(UserInfo.GetNotes().isEmpty())
        Object.insert("notes",NullValue);
    else
        Object.insert("notes",UserInfo.GetNotes());

    return Object;
}

/******************************
 * Sets the user information
 * ***************************/
void AnimeEntity::SetUserInfo(const UserAnimeInformation &Info)
{
    UserInfo = Info;
    UserInfoSet = true;

    //Set the anime episode count of the information
    if(GetAnimeEpisodeCount() <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE) return;
    UserInfo.SetAnimeEpisodes(GetAnimeEpisodeCount());

    //Check that the current episode is less than the anime episodes
    if(UserInfo.GetEpisodesWatched() > GetAnimeEpisodeCount())
        UserInfo.SetEpisodesWatched(GetAnimeEpisodeCount());
}

/***********************************************************************
 * Cleans all anime titles and appends them to the cleantitles list
 ***********************************************************************/
void AnimeEntity::CleanAllTitles()
{
    CleanTitles.clear();

    //Add user recognition names first
    foreach (QString Title, RecognitionTitles)
    {
        QString CleanedTitle = Title;
        CleanTitle(CleanedTitle);

        //Make sure it is not blank or it may cause recognition errors
        if(!(CleanedTitle.isNull() || CleanedTitle.isEmpty()))
            CleanTitles << CleanedTitle;
    }


    //Add the anime clean titles last
    QString CleanAnimeTitle = AnimeTitle;
    QString CleanAlternateTitle = AnimeAlternateTitle;

    //Clean titles
    CleanTitle(CleanAnimeTitle);
    CleanTitle(CleanAlternateTitle);

    //Add it to the clean titles
    if((!CleanAnimeTitle.isNull() || CleanAnimeTitle.isEmpty()))
        CleanTitles << CleanAnimeTitle;

    if((!CleanAlternateTitle.isNull() || CleanAlternateTitle.isEmpty()))
        CleanTitles << CleanAlternateTitle;

}

//******************************************************************************************************

UserAnimeInformation::UserAnimeInformation():
    EpisodesWatched(ANIMEENTITY_UNKNOWN_USER_EPISODE),
    LastWatched(QDateTime::currentDateTime()),
    RewatchedTimes(-1),
    Notes(""),
    Status(""),
    Private(false),
    Rewatching(false),
    RatingType(""),
    RatingValue(0),
    Priority(PRIORITY_NORMAL),
    AnimeEpisodeCount(ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
{
}

/**********************************
 * Parses user data to the class
 *********************************/
void UserAnimeInformation::ParseUserMap(QVariantMap UserInfoMap)
{
    SetEpisodesWatched(UserInfoMap.value("episodes_watched",ANIMEENTITY_UNKNOWN_USER_EPISODE).toInt());
    SetLastWatched(QDateTime::fromString(UserInfoMap.value("last_watched",QDateTime::currentDateTimeUtc().toString(ANIMEENTITY_DATE_FORMAT)).toString(),ANIMEENTITY_DATE_FORMAT));
    SetRewatchedTimes(UserInfoMap.value("rewatched_times",0).toInt());
    SetNotes(UserInfoMap.value("notes","").toString());
    SetStatus(UserInfoMap.value("status","").toString());
    SetPrivate(UserInfoMap.value("private",false).toBool());
    SetRewatching(UserInfoMap.value("rewatching",false).toBool());
    SetRatingType(UserInfoMap.value("rating").toMap().value("type","simple").toString());
    SetRatingValue(UserInfoMap.value("rating").toMap().value("value",0).toFloat());
    SetPriority(UserInfoMap.value("priority",PRIORITY_NORMAL).toInt());
}

/****************************************************
 * Updates user info based on the episode watched
 ****************************************************/
bool UserAnimeInformation::Update(AnimeEpisode &Episode)
{
    if(!IsUpdateAllowed(Episode,false))
        return false;

    Episode.Processed = true;

    //Set the episode count
    SetEpisodesWatched(Episode.GetEpisodeNumberHigh(),true);
    if(GetStatus() == STATUS_PLAN_TO_WATCH || GetStatus() == STATUS_ON_HOLD)
        SetStatus(STATUS_CURRENTLY_WATCHING);

    return true;
}

/***************************************************************
 * Checks if updating from the episode is allowed
 ***************************************************************/
bool UserAnimeInformation::IsUpdateAllowed(Anime::AnimeEpisode &Episode, bool IgnoreUpdateTime)
{
    if (Episode.Processed)
        return false;

    if (!IgnoreUpdateTime)
        if (Settings.Recognition.Delay > Media_Manager.MediaTicker)
            if (Media_Manager.MediaTicker > -1)
                return false;

    //Don't update if we've completed and are not rewatching
    if (GetStatus() == STATUS_COMPLETED && !isRewatching())
        return false;

    int EpisodeNumber = Episode.GetEpisodeNumberHigh();

    //Check if episode is valid
    if ((EpisodeNumber < 0) || (EpisodeNumber < GetEpisodesWatched()) || (EpisodeNumber == GetEpisodesWatched() && AnimeEpisodeCount != 1) || (EpisodeNumber > AnimeEpisodeCount && AnimeEpisodeCount <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE))
        return false;

    return true;
}

/******************************************************************
 * Sets the amount of episodes watched and updates last watched
 ******************************************************************/
void UserAnimeInformation::SetEpisodesWatched(int EpisodeCount, bool UpdateWatched)
{
    if(((EpisodeCount <= AnimeEpisodeCount) && (EpisodeCount >= 0)) || (AnimeEpisodeCount <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE))
        EpisodesWatched = EpisodeCount;

    if(UpdateWatched)
        UpdateLastWatched();
}

/*********************************
 *  Increments episode count
 *********************************/
void UserAnimeInformation::IncrementEpisodeCount()
{
    if(EpisodesWatched + 1 <= AnimeEpisodeCount || AnimeEpisodeCount <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
    {
        //if user is planing to watch(or anime is on hold) and increments episode, then we move it to currently watching
        if(GetStatus() == STATUS_PLAN_TO_WATCH || STATUS_ON_HOLD)
            SetStatus(STATUS_CURRENTLY_WATCHING);

        EpisodesWatched++;
        UpdateLastWatched();
    }
}

/******************************
 *  Decrements episode count
 ******************************/
void UserAnimeInformation::DecrementEpisodeCount()
{
    if(EpisodesWatched - 1 >= 0)
    {
        EpisodesWatched--;
        UpdateLastWatched();
    }
}

/****************************************************************************************************
 *  Checks if the current last watched date is more recent than the time provided
 *  This is used instead of the default > operator provided by QDateTime because of accuracy issues
 *
 *  TODO: Might simplify this to use time_t (seconds that have passes since 1970-01-01 00:00:00)
 *****************************************************************************************************/
bool UserAnimeInformation::isLastWatchedRecentThan(QDateTime Time)
{
    QDate CurDate = GetLastWatched().date();
    QDate ArgDate = Time.date();
    QTime CurTime = GetLastWatched().time();
    QTime ArgTime = Time.time();

    //Check if the current date is later than the argument date, if it is then we are 100% sure it is later
    bool DateGreater = (CurDate > ArgDate);
    if(DateGreater)
        return true;

    //Since the date isn't greater, we check to see if it is the current day, in which case we compare the times then
    if(CurDate == ArgDate)
    {
        bool HourGreater = CurTime.hour() >= ArgTime.hour();
        bool HourEqual = CurTime.hour() == ArgTime.hour();

        bool MinuteGreater = CurTime.minute() >= ArgTime.minute();
        bool MinuteEqual = CurTime.minute() == ArgTime.minute();

        bool SecondsGreater = CurTime.second() > ArgTime.second();

        if(!HourGreater)
            return false;
        if(HourGreater && !HourEqual)
            return true;
        if(HourGreater && HourEqual)
        {
            if(!MinuteGreater)
                return false;
            if(MinuteGreater && !MinuteEqual)
                return true;
            if(MinuteGreater && MinuteEqual)
                if(SecondsGreater)
                    return true;
        }
    }

    //Neither date or time is later than arg time
    return false;
}
