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

#include "library_test.h"

#include <QTime>

#include "library/animedatabase.h"
#include "library/animeentity.h"
#include "library/historymanager.h"
#include "library/user.h"

void Library_test::DatabaseAddFunction()
{
    //Create required components
    Anime::AnimeDatabase Database;

    //check if the database has no items
    QCOMPARE(Database.GetDatabaseSize(),0);

    Anime::AnimeEntity Entity;
    QCOMPARE(Entity.GetAnimeSlug(),QString(""));

    //Setup required variables
    QDate Date_Early(1999,1,1);
    QDate Date_Now(2014,1,1);
    QDate Date_Later(2015,1,1);
    QTime TimeNow(1,1,1);
    QTime SecondLater(1,1,2);

    //Database should not add entity with an empty slug
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),0);

    //now add a slug
    Entity.SetAnimeSlug("Slug");
    Entity.GetUserInfo()->SetLastWatched(QDateTime(Date_Early,TimeNow));
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),1);

    //Update the entity so that the later than date is newer
    Entity.GetUserInfo()->SetEpisodesWatched(1);
    Entity.GetUserInfo()->SetLastWatched(QDateTime(Date_Now,TimeNow));
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),1);
    QCOMPARE(Database.GetAnime(Entity.GetAnimeSlug())->GetUserInfo()->GetEpisodesWatched(),1);

    //Check if an unknown episode will be overriden
    Entity.GetUserInfo()->SetEpisodesWatched(ANIMEENTITY_UNKNOWN_USER_EPISODE);
    Entity.GetUserInfo()->SetLastWatched(QDateTime(Date_Later,TimeNow));
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),1);
    QCOMPARE(Database.GetAnime(Entity.GetAnimeSlug())->GetUserInfo()->GetEpisodesWatched(),1);

    //******** Check if it keeps old information (continued from previous test)

    //new anime date is earlier than old anime date
    Entity.GetUserInfo()->SetEpisodesWatched(2);
    Entity.GetUserInfo()->SetLastWatched(QDateTime(Date_Now,TimeNow));
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),1);
    QCOMPARE(Database.GetAnime(Entity.GetAnimeSlug())->GetUserInfo()->GetEpisodesWatched(),1);

    //new anime date is a second later than the old anime date
    Entity.GetUserInfo()->SetEpisodesWatched(3);
    Entity.GetUserInfo()->SetLastWatched(QDateTime(Date_Now,SecondLater));
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),1);
    QCOMPARE(Database.GetAnime(Entity.GetAnimeSlug())->GetUserInfo()->GetEpisodesWatched(),3);

    //new anime date is a few years earlier than the old anime date, but same time
    Entity.GetUserInfo()->SetEpisodesWatched(2);
    Entity.GetUserInfo()->SetLastWatched(QDateTime(Date_Early,TimeNow));
    Database.AddAnime(&Entity);
    QCOMPARE(Database.GetDatabaseSize(),1);
    QCOMPARE(Database.GetAnime(Entity.GetAnimeSlug())->GetUserInfo()->GetEpisodesWatched(),3);

    /****************************************************************************/

}
