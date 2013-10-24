#ifndef ANIMEDATABASE_H
#define ANIMEDATABASE_H

#define ANIMEDATABASE_UKNOWN_SLUG "Unknown Slug"

#include <QObject>
#include <QMap>

#include "animeentity.h"

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
    bool RemoveAnime(QString Slug);
    bool RemoveAnime(AnimeEntity* Anime);
    AnimeEntity* GetAnime(QString Slug) const;
    inline int GetDatabaseSize() const { return Database.size(); }
    inline bool Contains(QString Slug) const { return Database.contains(Slug); }

    //Note: May take out and replace with more specific functions
    inline QMap<QString,AnimeEntity*> *GetDatabase() { return &Database; }

    //Removes item from the DB and returns it
    AnimeEntity* TakeAnime(QString Slug) { return Database.take(Slug); }

    //Get the slug of an anime based on the title/alternate title
    QString GetAnimeSlug(QString Title);

    //Parses JSON data recieved from the api and creates a list
    void ParseJson(QString Data);

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
