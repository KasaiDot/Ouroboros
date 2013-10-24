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
 *
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
