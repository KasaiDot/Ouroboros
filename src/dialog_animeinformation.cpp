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

#include "dialog_animeinformation.h"
#include "ui_dialog_animeinformation.h"

#include "filemanager.h"

Dialog_AnimeInformation::Dialog_AnimeInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_AnimeInformation)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() |= Qt::MSWindowsFixedSizeDialogHint);
}

Dialog_AnimeInformation::~Dialog_AnimeInformation()
{
    delete ui;
}

/*********************************************************
 * Extracts information from the anime and sets the info
 ********************************************************/
void Dialog_AnimeInformation::ParseAnime(Anime::AnimeEntity &Entity)
{
    QString Slug = Entity.GetAnimeSlug();
    QByteArray ImageData = File_Manager.GetAnimeImage(Slug);
    int ImageWidth = 200;
    int ImageHeight = 290;

    if(!(ImageData.isNull() || ImageData.isEmpty()))
    {
        //Set the image
        QPixmap Pixmap(ImageWidth,ImageHeight);
        Pixmap.loadFromData(ImageData);
        ui->ImageFrame->setPixmap(Pixmap);
    }

    //Set the text
    this->ui->Title->setText(Entity.GetAnimeTitle());

    if(!Entity.GetAnimeAlternateTitle().isEmpty())
        this->ui->AlternativeTitle->setText(Entity.GetAnimeAlternateTitle());

    if(!Entity.GetAnimeStatus().isEmpty())
        this->ui->Status->setText(Entity.GetAnimeStatus());

    if(!Entity.GetAnimeShowType().isEmpty())
        this->ui->Type->setText(Entity.GetAnimeShowType());

    if(Entity.GetAnimeEpisodeCount() > ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        this->ui->Episodes->setText(QString::number(Entity.GetAnimeEpisodeCount()));

    if(!Entity.GetAnimeUrl().isEmpty())
    {
        QString Url = "http://hummingbird.me" + Entity.GetAnimeUrl();
        this->ui->Url->setText(QString("<a href=\"%1\"> Click Me </a>").arg(Url));
    }

    if(!Entity.GetAnimeSynopsis().isEmpty())
        this->ui->Synopsis->setText(Entity.GetAnimeSynopsis());

    //Go through each genre and comma seperate them
    if(Entity.GetAnimeGenres().size() > 0)
    {
        QString GenreList;
        foreach (QString Genre, Entity.GetAnimeGenres())
        {
            GenreList.append(Genre + ",");
        }
        //chop of the trailing ,
        GenreList.chop(1);

        this->ui->Genres->setText(GenreList);
    }

}
