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

#ifndef ANIMEENTITY_H
#define ANIMEENTITY_H

#define ANIMEENTITY_UNKNOWNE_EPISODE -1

#include <QObject>
#include <QStringList>
#include <QDateTime>

namespace Anime
{

//Contais user information for an anime entity
class UserAnimeInformation
{
public:
    UserAnimeInformation();

    //****************************************** Setters and Getter methods *********************************************************************/
    int GetEpisodesWatched() const { return EpisodesWatched; }
    inline void SetEpisodesWatched(int EpisodeCount) { if((EpisodeCount <= AnimeEpisodes) && (EpisodeCount >= 0)) EpisodesWatched = EpisodeCount; }
    inline void IncrementEpisodeCount() { if(EpisodesWatched + 1 <= AnimeEpisodes) EpisodesWatched++; }
    inline void DecrementEpisodeCount() { if(EpisodesWatched - 1 >= 0) EpisodesWatched--; }

    QDateTime GetLastWatched() const { return LastWatched; }
    inline void SetLastWatched(QDateTime Time) { LastWatched = Time; }

    int GetRewatchedTimes() const { return RewatchedTimes; }
    inline void SetRewatchedTimes(int Amount) { RewatchedTimes = Amount; }

    QString GetNotes() const { return Notes;}
    inline void SetNotes(QString NoteString) { Notes = NoteString; }

    bool isNotePresent() const { return NotesPresent; }
    inline void SetNotePresent(bool Present) { NotesPresent = Present; }

    QString GetStatus() const { return Status; }
    inline void SetStatus(QString Stat) { Status = Stat; }

    bool isPrivate() const { return Private; }
    inline void SetPrivate(bool isPrivate) { Private = isPrivate; }

    bool isRewatching() const { return Rewatching; }
    inline void SetRewatching(bool isRewatching) { Rewatching = isRewatching; }

    QString GetRatingType() const { return RatingType; }
    inline void SetRatingType(QString Type) { RatingType = Type; }

    int GetRatingValue() const { return RatingValue; }
    inline void SetRatingValue(int Rating) { RatingValue = Rating; }

    inline void SetAnimeEpisodes(int Count) { AnimeEpisodes = Count; }

private:

    /*** User specific information ***/
    int EpisodesWatched;
    QDateTime LastWatched;
    int RewatchedTimes;
    QString Notes;
    bool NotesPresent;
    QString Status;
    //QString Id;
    bool Private;
    bool Rewatching;
    QString RatingType;
    int RatingValue;

    //Number of anime episodes
    int AnimeEpisodes;

};

class AnimeEntity
{
public:
    AnimeEntity();
    AnimeEntity(const AnimeEntity &Entity);

    //Compares the titles of two entities, if they are the same it returns true
    bool CompareTitle(AnimeEntity &Entity);

    // Override '==' operator so that we can compare entities
    bool operator ==(const AnimeEntity &Other);

    //****************************************** Setters and Getter methods *********************************************************************/

    UserAnimeInformation GetUserInfo() const { return UserInfo; }
    inline void SetUserInfo(UserAnimeInformation Info)
    {
        UserInfo = Info;
        //Check that the current episode is less than the anime episodes
        if(GetAnimeEpisodeCount() == ANIMEENTITY_UNKNOWNE_EPISODE) return;
        if(UserInfo.GetEpisodesWatched() > GetAnimeEpisodeCount())
            UserInfo.SetEpisodesWatched(GetAnimeEpisodeCount());
        Info.SetAnimeEpisodes(GetAnimeEpisodeCount());
    }

    QString GetAnimeSlug() const { return AnimeSlug; }
    inline void SetAnimeSlug(QString Slug) { AnimeSlug = Slug; }

    QString GetAnimeStatus() const { return AnimeStatus; }
    inline void SetAnimeStatus(QString Status) { AnimeStatus = Status; }

    QString GetAnimeUrl() const { return AnimeUrl; }
    inline void SetAnimeUrl(QString Url) { AnimeUrl = Url; }

    QString GetAnimeTitle() const { return AnimeTitle; }
    inline void SetAnimeTitle(QString Title) { AnimeTitle = Title; }

    QString GetAnimeAlternateTitle() const { return AnimeAlternateTitle; }
    inline void SetAnimeAlternateTitle(QString AlternateTitle) { AnimeAlternateTitle = AlternateTitle; }

    int GetAnimeEpisodeCount() const { return AnimeEpisodeCount; }
    inline void SetAnimeEpisodeCount(int EpisodeCount) { if(EpisodeCount > 0) AnimeEpisodeCount = EpisodeCount; }

    QString GetAnimeImage() const { return AnimeImage; }
    inline void SetAnimeImage(QString Image) { AnimeImage = Image; }

    QString GetAnimeSynopsis() const { return AnimeSynopsis; }
    inline void SetAnimeSynopsis(QString Synopsis) { AnimeSynopsis = Synopsis; }

    QString GetAnimeShowType() const { return AnimeShowType; }
    inline void SetAnimeShowType(QString ShowType) { AnimeShowType = ShowType; }

    QStringList GetAnimeGenres() const { return AnimeGenres; }
    inline void SetAnimeGenres(QStringList Genres) { AnimeGenres = Genres; }


private:

    /*** Anime specific information ***/
    QString AnimeSlug;
    QString AnimeStatus;
    QString AnimeUrl;
    QString AnimeTitle;
    QString AnimeAlternateTitle;
    int AnimeEpisodeCount;
    QString AnimeImage;
    QString AnimeSynopsis;
    QString AnimeShowType;
    QStringList AnimeGenres;

    UserAnimeInformation UserInfo;

};

}

#endif // ANIMEENTITY_H
