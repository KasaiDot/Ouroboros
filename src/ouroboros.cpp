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

#include "ouroboros.h"
#include "ui_ouroboros.h"

#include <QTime>
#include <QDebug>

#include "animeentity.h"

Ouroboros::Ouroboros(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Ouroboros)
{
    ui->setupUi(this);

    //Generate a random seed
    QTime CurrentTime = QTime::currentTime();
    qsrand((uint)CurrentTime.msec());

    Anime::AnimeEntity *a = new Anime::AnimeEntity();
    a->SetAnimeTitle("test");
    a->SetAnimeSlug("slug");

    Anime_Database.AddAnime(a);
    qDebug() << Anime_Database.GetDatabaseSize();

}

Ouroboros::~Ouroboros()
{
    delete ui;
}
