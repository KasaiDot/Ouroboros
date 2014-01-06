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

#include "dialog_animeinformation.h"
#include "ui_dialog_animeinformation.h"

#include "manager/filemanager.h"
#include "manager/stylemanager.h"

Dialog_AnimeInformation::Dialog_AnimeInformation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_AnimeInformation),
    ShowMyInfo(false)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() |= Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->ActionListWidget,SIGNAL(currentRowChanged(int)),ui->stackedWidget,SLOT(setCurrentIndex(int)));
    ui->ActionListWidget->setCurrentRow(0);

    this->setStyleSheet(QString(Style_Manager.GetStyle()));

    //Connect signals and slots
    connect(&Style_Manager,&Manager::StyleManager::StyleChanged,[=](QByteArray NewStyle){
        this->setStyleSheet(QString(NewStyle));
    });
}

Dialog_AnimeInformation::~Dialog_AnimeInformation()
{
    delete ui;
}

/*********************************************************
 * Extracts information from the anime and sets the info
 ********************************************************/
void Dialog_AnimeInformation::ParseAnime(Anime::AnimeEntity &Entity, bool ShowMyInfo)
{
    this->Entity = &Entity;
    this->ShowMyInfo = ShowMyInfo;

    //download the image if not availiable
    File_Manager.SaveAnimeImage(&Entity);

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
        this->ui->Url->setText(QString("<a href=\"%1\"> Click Me </a>").arg(Entity.GetAnimeUrl()));
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

    //Set recogntition titles
    QString RecognitionString;
    foreach(QString Title,Entity.GetRecognitionTitles())
    {
        RecognitionString.append(Title + ",");
    }
    if(!RecognitionString.isEmpty())
        RecognitionString.chop(1); //Remove the trailing comma

    ui->AlternateRecognitionTitles->setText(RecognitionString);

    if(ShowMyInfo)
    {
        ui->ActionListWidget->addItem("My Information");

        Anime::UserAnimeInformation *UserInfo = Entity.GetUserInfo();

        //Set the episodes
        int MaxEpisodes = (Entity.GetAnimeEpisodeCount() == ANIMEENTITY_UNKNOWN_ANIME_EPISODE) ? 999 : Entity.GetAnimeEpisodeCount();
        int CurrentEpisode = (UserInfo->GetEpisodesWatched() == ANIMEENTITY_UNKNOWN_USER_EPISODE) ? 0 : UserInfo->GetEpisodesWatched();
        ui->EpisodeSpinBox->setMaximum(MaxEpisodes);
        ui->EpisodeSpinBox->setValue(CurrentEpisode);

        //set the rewatching checkbox
        if(UserInfo->GetStatus() == STATUS_COMPLETED)
        {
            ui->RewatchingCheckBox->setChecked(UserInfo->isRewatching());
        } else {
            ui->RewatchingCheckBox->setEnabled(false);
        }

        //Set the status and score
        ui->StatusComboBox->setCurrentIndex(GetStatusIndex(UserInfo->GetStatus()));
        ui->ScoreComboBox->setCurrentIndex(UserInfo->GetRatingValue() * 2);

        Episode = CurrentEpisode;
        Status = UserInfo->GetStatus();
        Score = UserInfo->GetRatingValue();
        Rewatching = UserInfo->isRewatching();
    }


}

/****************************************
 * Rewatching checkbox clicked by user
 ****************************************/
void Dialog_AnimeInformation::on_RewatchingCheckBox_toggled(bool Checked)
{
    if(Checked)
    {
        ui->EpisodeSpinBox->setValue(0);
        ui->StatusComboBox->setEnabled(false);
    } else {
        int CurrentEpisode = (Entity->GetUserInfo()->GetEpisodesWatched() == ANIMEENTITY_UNKNOWN_USER_EPISODE) ? 0 : Entity->GetUserInfo()->GetEpisodesWatched();
        ui->EpisodeSpinBox->setValue(CurrentEpisode);
        ui->StatusComboBox->setEnabled(true);
    }
}

/***********************
 * User clicked ok
 ***********************/
void Dialog_AnimeInformation::on_MainButtonBox_accepted()
{
    //Set the recognition names
    QStringList RecognitionNames = ui->AlternateRecognitionTitles->text().split(",");
    Entity->SetRecognitionTitles(RecognitionNames);

    if(ShowMyInfo)
    {
        //Set user info
        if(ui->RewatchingCheckBox->isChecked() != Rewatching)
            Entity->GetUserInfo()->SetRewatching(ui->RewatchingCheckBox->isChecked());

        if(ui->EpisodeSpinBox->value() != Episode)
            Entity->GetUserInfo()->SetEpisodesWatched(ui->EpisodeSpinBox->value(),true);

        if(GetStatusName(ui->StatusComboBox->currentIndex()) != Status)
            Entity->GetUserInfo()->SetStatus(GetStatusName(ui->StatusComboBox->currentIndex()));

        if(static_cast<float>(ui->ScoreComboBox->currentIndex()) / 2 != Score)
            Entity->GetUserInfo()->SetRatingValue(static_cast<float>(ui->ScoreComboBox->currentIndex()) / 2);

        emit UpdateAnime(Entity);
    }

}
