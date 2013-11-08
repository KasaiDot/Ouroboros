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

#include "animeentity.h"

#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

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
    UserInfo(UserAnimeInformation())
{
}

/***************************************************************************
 * This functions copies values from the entity provided to the new entity
 ***************************************************************************/
AnimeEntity::AnimeEntity( AnimeEntity &Entity)
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
    UserAnimeInformation *Info = Entity.GetUserInfo();
    SetUserInfo(*Info);
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
    QJsonValue LastWatched(UserInfo.GetLastWatched().toString("yyyy-dd-MMThh:mm:ssZ"));
    QJsonValue RewatchedTimes(UserInfo.GetRewatchedTimes());
    QJsonValue Notes(UserInfo.GetNotes());
    QJsonValue NotesPresent(UserInfo.isNotePresent());
    QJsonValue Status(UserInfo.GetStatus());
    QJsonValue Private(UserInfo.isPrivate());
    QJsonValue Rewatching(UserInfo.isRewatching());

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

//******************************************************************************************************

UserAnimeInformation::UserAnimeInformation():
     EpisodesWatched(-1),
     LastWatched(QDate::currentDate(),QTime::currentTime()),
     RewatchedTimes(-1),
     Notes(""),
     NotesPresent(false),
     Status(""),
     Private(false),
     Rewatching(false),
     RatingType(""),
     RatingValue(0)
{
}
