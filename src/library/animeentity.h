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

//*********************************************************************************************************
#define ANIMEENTITY_UNKNOWN_ANIME_EPISODE 0
#define ANIMEENTITY_UNKNOWN_USER_EPISODE -1
#define ANIMEENTITY_DATE_FORMAT "yyyy-MM-ddThh:mm:ssZ"
//*********************************************************************************************************

#include <QObject>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QMessageBox>
#include <QApplication>

#include "library/animeepisode.h"
#include "ouroboros/common.h"
#include "ouroboros/globals.h"

namespace Anime
{

//Contais user information for an anime entity
class UserAnimeInformation
{
public:
    UserAnimeInformation();

    void ParseUserMap(QVariantMap UserInfoMap);
    bool Update(Anime::AnimeEpisode &Episode);
    bool IsUpdateAllowed(AnimeEpisode &Episode, bool IgnoreUpdateTime);

    //****************************************** Setters and Getter methods *********************************************************************/

    inline void UpdateLastWatched() { SetLastWatched(QDateTime::currentDateTimeUtc());}

    //***************************************************************************************************************/
    int GetEpisodesWatched() const { return EpisodesWatched; }
    inline void SetEpisodesWatched(int EpisodeCount, bool UpdateWatched = false)
    {
        if(((EpisodeCount <= AnimeEpisodes) && (EpisodeCount >= 0)) || (AnimeEpisodes <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE))
            EpisodesWatched = EpisodeCount;
        if(UpdateWatched)
            UpdateLastWatched();
    }

    inline void IncrementEpisodeCount()
    {
        if(EpisodesWatched + 1 <= AnimeEpisodes || AnimeEpisodes <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        {
            //if user is planing to watch and increments episode, then we move it to currently watching
            if(GetStatus() == STATUS_PLAN_TO_WATCH)
                SetStatus(STATUS_CURRENTLY_WATCHING);

            EpisodesWatched++;
            UpdateLastWatched();
        }
    }

    inline void DecrementEpisodeCount()
    {
        if(EpisodesWatched - 1 >= 0) EpisodesWatched--;
        UpdateLastWatched();
    }

    //***************************************************************************************************************/

    QDateTime GetLastWatched() const { return LastWatched; }
    inline void SetLastWatched(QDateTime Time) { LastWatched = Time; }

    //qt's datetime > operator is not very accurate, hence we create our own function
    inline bool LastWatchedLaterThan(QDateTime Time)
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

    //***************************************************************************************************************/

    int GetRewatchedTimes() const { return RewatchedTimes; }
    inline void SetRewatchedTimes(int Amount) { RewatchedTimes = Amount;}

    //***************************************************************************************************************/

    QString GetNotes() const { return Notes;}
    inline void SetNotes(QString NoteString) { Notes = NoteString; }

    //***************************************************************************************************************/

    bool isNotePresent() const { return NotesPresent; }
    inline void SetNotePresent(bool Present) { NotesPresent = Present; }

    //***************************************************************************************************************/

    QString GetStatus() const { return Status; }
    inline void SetStatus(QString Status) { this->Status = Status; }

    //***************************************************************************************************************/

    bool isPrivate() const { return Private; }
    inline void SetPrivate(bool isPrivate) { Private = isPrivate; }

    //***************************************************************************************************************/

    bool isRewatching() const { return Rewatching; }
    inline void SetRewatching(bool isRewatching) { Rewatching = isRewatching; }

    //***************************************************************************************************************/

    QString GetRatingType() const { return RatingType; }
    inline void SetRatingType(QString Type) { RatingType = Type; }

    float GetRatingValue() const { return RatingValue; }
    inline void SetRatingValue(float Rating) { RatingValue = Rating; }

    //***************************************************************************************************************/

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
    float RatingValue;

    //Number of anime episodes
    int AnimeEpisodes;
};

class AnimeEntity
{
public:
    AnimeEntity();
    AnimeEntity(AnimeEntity &Entity);
    AnimeEntity(const AnimeEntity &Entity);

    //Parses anime json files
    bool ParseAnimeJson(QByteArray Data,bool ContainsUserInfo = false);

    //Compares the titles of two entities, if they are the same it returns true
    bool CompareTitle(AnimeEntity &Entity);

    //Override '==' operator so that we can compare entities
    bool operator ==(const AnimeEntity &Other);

    //Constructs json documents for saving
    QJsonDocument ConstructAnimeJsonDocument();
    QJsonDocument ConstructUserJsonDocument();

    //constructs json for update
    QJsonObject ConstructUpdateJsonObject();

    //****************************************** Setters and Getter methods *********************************************************************/

    UserAnimeInformation *GetUserInfo() { return &UserInfo; } //Return a pointer as to not make a copy of the information
    const UserAnimeInformation *GetConstUserInfo() const { return &UserInfo; }
    inline void SetUserInfo(UserAnimeInformation const &Info)
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

    //***************************************************************************************************************/


    QString GetAnimeSlug() const { return AnimeSlug; }
    inline void SetAnimeSlug(QString Slug) { AnimeSlug = Slug; }

    //***************************************************************************************************************/

    QString GetAnimeStatus() const { return AnimeStatus; }
    inline void SetAnimeStatus(QString Status) { AnimeStatus = Status; }

    //***************************************************************************************************************/

    QString GetAnimeUrl() const { return AnimeUrl; }
    inline void SetAnimeUrl(QString Url) { AnimeUrl = Url; }

    //***************************************************************************************************************/

    QString GetAnimeTitle() const { return AnimeTitle; }
    inline void SetAnimeTitle(QString Title) { AnimeTitle = Title; CleanAllTitles(); }

    QString GetAnimeAlternateTitle() const { return AnimeAlternateTitle; }
    inline void SetAnimeAlternateTitle(QString AlternateTitle) {  AnimeAlternateTitle = AlternateTitle; CleanAllTitles(); }

    //***************************************************************************************************************/

    int GetAnimeEpisodeCount() const { return AnimeEpisodeCount; }
    inline void SetAnimeEpisodeCount(int EpisodeCount) { if(EpisodeCount >= 0) AnimeEpisodeCount = EpisodeCount; }

    //***************************************************************************************************************/

    QString GetAnimeImage() const { return AnimeImage; }
    inline void SetAnimeImage(QString Image) { AnimeImage = Image; }

    //***************************************************************************************************************/

    QString GetAnimeSynopsis() const { return AnimeSynopsis; }
    inline void SetAnimeSynopsis(QString Synopsis) { AnimeSynopsis = Synopsis; }

    //***************************************************************************************************************/

    QString GetAnimeShowType() const { return AnimeShowType; }
    inline void SetAnimeShowType(QString ShowType) { AnimeShowType = ShowType; }

    //***************************************************************************************************************/

    QStringList GetAnimeGenres() const { return AnimeGenres; }
    inline void SetAnimeGenres(QStringList Genres) { AnimeGenres = Genres; }
    inline void AddAnimeGenre(QString Genre) { if(!Genre.isEmpty()) AnimeGenres.append(Genre); }

    //***************************************************************************************************************/

    bool isUserInfoSet() const { return UserInfoSet; }

    //******************************************* Recognition **************************************************************/
    void CleanAllTitles();
    inline QStringList GetCleanTitles() const { return CleanTitles; }

    inline void SetRecognitionTitles(QStringList List) { RecognitionTitles = List; CleanAllTitles(); }
    inline QStringList GetRecognitionTitles() const { return RecognitionTitles; }

    //***************************************************************************************************************/


private:

    /*** Anime specific information ***/
    QString AnimeSlug;
    QString AnimeStatus;
    QString AnimeUrl;
    QString AnimeTitle;
    QString AnimeAlternateTitle;

    //recognition
    QStringList CleanTitles;
    QStringList RecognitionTitles;

    int AnimeEpisodeCount;
    QString AnimeImage;
    QString AnimeSynopsis;
    QString AnimeShowType;
    QStringList AnimeGenres;

    UserAnimeInformation UserInfo;
    bool UserInfoSet;

};

}

#endif // ANIMEENTITY_H