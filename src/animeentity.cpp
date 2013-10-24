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

using namespace Anime;

AnimeEntity::AnimeEntity():
    AnimeSlug(""),
    AnimeStatus(""),
    AnimeUrl(""),
    AnimeTitle(""),
    AnimeAlternateTitle(""),
    AnimeEpisodeCount(-1),
    AnimeImage(""),
    AnimeSynopsis(""),
    AnimeShowType(""),
    UserInfo(UserAnimeInformation())
{
}

/***************************************************************************
 * This functions copies values from the entity provided to the new entity
 ***************************************************************************/
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
    SetUserInfo(Entity.GetUserInfo());
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
     RatingValue(-1)
{
}
