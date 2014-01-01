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

#ifndef ANIMEDATABASE_H
#define ANIMEDATABASE_H

//*********************************************************************************************************
#define ANIMEDATABASE_UKNOWN_SLUG "Unknown Slug"
//*********************************************************************************************************

#include <QObject>
#include <QMap>

#include "animeentity.h"
#include "animeepisode.h"
#include "globals.h"

namespace Anime
{
class AnimeDatabase : public QObject
{
    Q_OBJECT
public:
    explicit AnimeDatabase(QObject *parent = 0);
    ~AnimeDatabase();

    //Basic database functions
    void AddAnime(AnimeEntity* Anime);
    void NewAnime(AnimeEntity* Anime, QString UserStatus);
    AnimeEntity* RemoveAnime(QString Slug, bool Delete = true);
    bool RemoveAnime(AnimeEntity* Anime, bool Delete = true);

    AnimeEntity* GetAnime(QString Slug) const;
    AnimeEntity* GetAnime(Anime::AnimeEpisode &Episode, bool Strict = false);

    bool CompareEpisode(Anime::AnimeEpisode &Episode, AnimeEntity *Entity, bool Strict);
    bool CompareCleanTitles(Anime::AnimeEpisode &Episode,AnimeEntity *Entity, bool Strict);

    inline int GetDatabaseSize() const { return Database.size(); }
    inline bool Contains(QString Slug) const { return Database.contains(Slug); }
    void ClearDatabase();

    //Note: May take out and replace with more specific functions
    inline QMap<QString,AnimeEntity*> *GetDatabase() { return &Database; }
    inline QList<AnimeEntity*> GetAnimeEntities() { return Database.values(); }

    //Removes item from the DB and returns it
    AnimeEntity* TakeAnime(QString Slug) { return Database.take(Slug); }

    //Get the slug of an anime based on the title/alternate title
    QString GetAnimeSlug(QString Title, bool Strict = false);

    //Parses JSON data recieved from the api and creates a list
    void ParseJson(QByteArray Data);
    void ParseMultipleJson(QByteArray Data);

    //updates a specific entity with the episode within the database
    void UpdateEntity(AnimeEpisode &Episode,AnimeEntity *Entity);
    void UpdateEntity(AnimeEpisode &Episode,QString Slug);


private:

    //Database of the anime. Each anime has a slug which is used as the key
    QMap<QString,AnimeEntity*> Database;

signals:

public slots:

};

}

//Make the class a singleton meaning we can access it anywhere just by including the header
extern Anime::AnimeDatabase Anime_Database;

#endif // ANIMEDATABASE_H
