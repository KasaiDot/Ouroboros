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

    //Connect signals and slots
    connect(&Api_Manager,SIGNAL(ChangeStatus(QString)),this,SLOT(ChangeStatus(QString)));

    CurrentUser.SetUserDetails(TEST_USER,QByteArray(TEST_PASS).toBase64());

    //Load local database
    File_Manager.LoadAnimeDatabase();
    GUI_Manager.PopulateModel();


    Queue_Manager.AuthenticateUser();
    Queue_Manager.GetAnimeLibrary();


}

Ouroboros::~Ouroboros()
{
    //Save
    File_Manager.SaveUserInformation();
    File_Manager.SaveAnimeDatabase();

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

    ui->View_Completed->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_CurrentlyWatching->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_Dropped->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_OnHold->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
    ui->View_PlanToWatch->sortByColumn(HEADER_NAME,Qt::AscendingOrder);
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

/**************************************************
 * Changes the status on the bottom of the screen
 **************************************************/
void Ouroboros::ChangeStatus(QString Status)
{
    ui->MainStatusBar->showMessage(Status);
}

/****************** Action Triggers ********************************/
void Ouroboros::on_Action_Synchronize_Anime_triggered()
{
    Queue_Manager.GetAnimeLibrary();
}
