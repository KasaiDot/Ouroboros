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
