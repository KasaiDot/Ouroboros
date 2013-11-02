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

#include <QTimer>
#include <QDebug>

#include "animeentity.h"
#include "settings.h"
#include "filemanager.h"
#include "queuemanager.h"
#include "guimanager.h"

Ouroboros::Ouroboros(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Ouroboros)
{
    ui->setupUi(this);

    SetViewLayouts();

    //Set the ui for the manager
    GUI_Manager.SetMainWindow(this);

    //Generate a random seed
    QTime CurrentTime = QTime::currentTime();
    qsrand((uint)CurrentTime.msec());

    //Setup a timer to run the queue every 10 minutes
    QTimer *RunTimer = new QTimer(this);
    connect(RunTimer,SIGNAL(timeout()),&Queue_Manager,SLOT(Run()));
    RunTimer->start(600000);

    /** Testing save **/

    CurrentUser.SetUserDetails("User","pass");

    Anime::AnimeEntity *a = new Anime::AnimeEntity();
    a->SetAnimeSlug("cowboy-bepop");
    a->SetAnimeStatus("Finished Airing");
    a->SetAnimeUrl("/anime/cowboy-bepop");
    a->SetAnimeTitle("Cowboy Bepop");
    a->SetAnimeEpisodeCount(120);
    a->SetAnimeImage("http://static.hummingbird.me/anime/cover_images/000/000/001/thumb/1.png?1375232521");
    a->SetAnimeSynopsis("Enter a world in the distant future,where Bounty Hunters roam the solar system.Spike and Jet,bounty hunting partners,set out on journeys in an ever struggling effort to win bounty rewards to survive.\r\nWhile traveling,they meet up with other very interesting people.Could Faye,the beautiful and ridiculously poor gambler,Edward,the computer genius,and Ein,the engineered dog be a good addition to the group?");
    a->SetAnimeShowType("TV");
    a->AddAnimeGenre("Action");
    a->AddAnimeGenre("Adventure");
    a->AddAnimeGenre("Comedy");
    a->AddAnimeGenre("Drama");
    a->AddAnimeGenre("Sci-Fi");
    a->AddAnimeGenre("Space");

    Anime::UserAnimeInformation UserInfo;
    UserInfo.SetEpisodesWatched(100);
    UserInfo.SetLastWatched(QDateTime::fromString(QString("2013-10-12T03:41:27Z"),"yyyy-dd-MMThh:mm:ssZ"));
    UserInfo.SetStatus("on-hold");
    UserInfo.SetPrivate(false);
    UserInfo.SetRewatching(false);
    UserInfo.SetRewatchedTimes(0);
    UserInfo.SetRatingType("advanced");
    UserInfo.SetRatingValue(-1);

    a->SetUserInfo(UserInfo);

    Anime_Database.AddAnime(a);
    File_Manager.SaveAnimeDatabase();

    GUI_Manager.PopulateModel();

    //File_Manager.SaveAnimeEntity(a);

    //File_Manager.LoadAnimeDatabase();
    //qDebug() << Anime_Database.GetDatabaseSize();


}

Ouroboros::~Ouroboros()
{
    delete ui;
}

/*****************************************************
 * Function sets the view so it fills up the tab page
 *****************************************************/
void Ouroboros::SetViewLayouts()
{
    this->setCentralWidget(ui->MainTabWidget);
    ui->MainTabWidget->widget(TAB_CURRENTLY_WATCHING)->setLayout(ui->Layout_CurrentlyWatching);
    ui->MainTabWidget->widget(TAB_COMPLETED)->setLayout(ui->Layout_Completed);
    ui->MainTabWidget->widget(TAB_DROPPED)->setLayout(ui->Layout_Dropped);
    ui->MainTabWidget->widget(TAB_ON_HOLD)->setLayout(ui->Layout_OnHold);
    ui->MainTabWidget->widget(TAB_PLAN_TO_WATCH)->setLayout(ui->Layout_PlanToWatch);

    ui->View_Completed->header()->setSectionsMovable(false);
    ui->View_CurrentlyWatching->header()->setSectionsMovable(false);
    ui->View_Dropped->header()->setSectionsMovable(false);
    ui->View_OnHold->header()->setSectionsMovable(false);
    ui->View_PlanToWatch->header()->setSectionsMovable(false);
}

/************************************ Getter functions ****************************************/
QTabWidget *Ouroboros::GetMainTabWidget()
{
    return ui->MainTabWidget;
}

/*************************
 * Returns main views
 ************************/
QTreeView *Ouroboros::GetView(Ouroboros::Views Type)
{
    switch(Type)
    {
        case CurrentlyWatching:
            return ui->View_CurrentlyWatching;
        break;

        case OnHold:
           return ui->View_OnHold;
        break;

        case PlanToWatch:
            return ui->View_PlanToWatch;
        break;

        case Dropped:
            return ui->View_Dropped;
        break;

        case Completed:
            return ui->View_Completed;
        break;
    }

    return  nullptr;
}

/************************
 * Returns Actions
 ************************/
QAction *Ouroboros::GetAction(Ouroboros::Actions Type)
{
    switch (Type)
    {
        case EditUserEpisodes:
            return ui->Action_EditUserEpisodes;
        break;
    }

    return nullptr;
}
