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
