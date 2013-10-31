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

#include "guimanager.h"
#include "animedatabase.h"
#include "filemanager.h"

#include <QHeaderView>

using namespace Manager;
GUIManager GUI_Manager;

GUIManager::GUIManager(QObject *parent) :
    QObject(parent)
{
    //Set up the models
    DataModel = new QStandardItemModel(this);
}

/******************************************************
 * Sets the main window and connects signals and slots
 ******************************************************/
void GUIManager::SetMainWindow(Ouroboros *Main)
{
    MainWindow = Main;

    //Connect signals and slots
    connect(MainWindow->GetMainTabWidget(),SIGNAL(currentChanged(int)),this,SLOT(TabChanged(int)));

    //Set defaults
    TabChanged(0);

    //Setup required items
    SetUpDelegates();
    SetUpFilters();
    SetModelHeaders();
}

/********************************************
 * Populates the model with anime entities
 * May have ot be used in conjunction with a
 * thread to stop GUI freezing
 ********************************************/
void GUIManager::PopulateModel()
{
    //Clear the model
    DataModel->clear();

    QList<Anime::AnimeEntity *> AnimeList = Anime_Database.GetAnimeEntities();

    for(QList<Anime::AnimeEntity *>::const_iterator AnimeIt = AnimeList.begin(); AnimeIt != AnimeList.end(); ++AnimeIt)
    {
        Anime::AnimeEntity *Entity = *AnimeIt;
        AddAnime(Entity);
    }

}
/************************************************************************
 * Add a single entity to the list
 * Note: Model should be checked first if it already contains the entity
 * if so, then use UpdateAnime function instead
 ************************************************************************/
void GUIManager::AddAnime(Anime::AnimeEntity *Entity)
{
    //We need to create 5 seperate items then add them to the Datamodel
    QStandardItem *Item_Name = new QStandardItem(Entity->GetAnimeTitle());
    QStandardItem *Item_Progress = new QStandardItem();
    QStandardItem *Item_Rating = new QStandardItem(Entity->GetUserInfo()->GetRatingValue());
    QStandardItem *Item_Type = new QStandardItem(Entity->GetAnimeShowType());

    if(Entity->GetUserInfo()->GetRatingValue() < 0)
        Item_Rating->setText("-");

    //Set the slug, user episode count, etc
    Item_Name->setData(Entity->GetAnimeSlug(),ROLE_ANIME_SLUG);
    Item_Name->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);

    Item_Progress->setData(Entity->GetAnimeEpisodeCount(),ROLE_ANIME_EPISODES);
    Item_Progress->setData(Entity->GetUserInfo()->GetEpisodesWatched(),ROLE_USER_EPISODES);

    Item_Rating->setTextAlignment(Qt::AlignHCenter);
    Item_Type->setTextAlignment(Qt::AlignHCenter);

    //Add items to the model
    QList<QStandardItem *> ItemList;
    ItemList.append(Item_Name);
    ItemList.append(Item_Progress);
    ItemList.append(Item_Rating);
    ItemList.append(Item_Type);

    DataModel->appendRow(ItemList);
}

/***********************************************************************************
 * Updates the entire item in the model
 * Note: This will replace previous values so don't mix entities and their items up
 ***********************************************************************************/
void GUIManager::UpdateAnime(QStandardItem *Item, Anime::AnimeEntity *Entity)
{
    //Since we don't know if the item is the one containing the title, we have to manually get it and the other items
    QStandardItem *Item_Name = DataModel->item(Item->row(),HEADER_NAME);
    QStandardItem *Item_Progress = DataModel->item(Item->row(),HEADER_PROGRESS);
    QStandardItem *Item_Rating = DataModel->item(Item->row(),HEADER_RATING);
    QStandardItem *Item_Type = DataModel->item(Item->row(),HEADER_SHOW_TYPE);

    QString ItemRating = (Entity->GetUserInfo()->GetRatingValue() < 0) ? "-" : QString::number(Entity->GetUserInfo()->GetRatingValue());

    //set the text
    Item_Name->setText(Entity->GetAnimeTitle());
    Item_Rating->setText(ItemRating);
    Item_Type->setText(Entity->GetAnimeShowType());

    Item_Rating->setTextAlignment(Qt::AlignHCenter);
    Item_Type->setTextAlignment(Qt::AlignHCenter);

    //set data
    Item_Name->setData(Entity->GetAnimeSlug(),ROLE_ANIME_SLUG);
    Item_Name->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);

    Item_Progress->setData(Entity->GetAnimeEpisodeCount(),ROLE_ANIME_EPISODES);
    Item_Progress->setData(Entity->GetUserInfo()->GetEpisodesWatched(),ROLE_USER_EPISODES);

    //Save the anime
    File_Manager.SaveAnimeEntity(Entity,true);
}

void GUIManager::UpdateAnime(QModelIndex Index, Anime::AnimeEntity *Entity)
{
    QStandardItem *Item_Name = DataModel->item(Index.row(),HEADER_NAME);
    UpdateAnime(Item_Name,Entity);
}

/*******************************************************************
 * Checks if model contains the anime title,
 * if multiple selections return then it will update the first one
 *******************************************************************/
void GUIManager::UpdateAnime(Anime::AnimeEntity *Entity)
{
    if(!ModelContains(Entity->GetAnimeTitle())) return;

    QStandardItem *Item_Name = DataModel->findItems(Entity->GetAnimeTitle(),Qt::MatchFixedString,HEADER_NAME).at(0);
    UpdateAnime(Item_Name,Entity);

}

/************************************************************************************************************
 * Handles the button click signal recieved from pressing either the "+" or "-" buttons on the progress bar
 ***********************************************************************************************************/
void GUIManager::ProgressBarButtonClicked(QModelIndex Index, ProgressDelegate::Button Type)
{

    QString AnimeSlug = DataModel->item(Index.row(),HEADER_NAME)->data(ROLE_ANIME_SLUG).toString();
    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(AnimeSlug);

    if(Type == ProgressDelegate::Plus)
    {
        Entity->GetUserInfo()->IncrementEpisodeCount();
    } else if(Type == ProgressDelegate::Minus)
    {
        Entity->GetUserInfo()->DecrementEpisodeCount();
    }

    //Update the view
    UpdateAnime(Index,Entity);
}

/*********************************
 * Selects the row the item is in
 *********************************/
void GUIManager::SelectRow(QModelIndex Index)
{
    //Since we know that that when the user clicks on the progress bar, he/she will always be looking at the CurrentView
    //so we can just set it's current index to that of the index passed in
    CurrentView->setCurrentIndex(Index);
}

/************************************************
 * When tab changes, this sets the current view
 ************************************************/
void GUIManager::TabChanged(int Tab)
{
    switch(Tab)
    {
        case TAB_COMPLETED:
            CurrentView = MainWindow->GetView(Ouroboros::Completed);
        break;

        case TAB_CURRENTLY_WATCHING:
            CurrentView = MainWindow->GetView(Ouroboros::CurrentlyWatching);
        break;

        case TAB_DROPPED:
            CurrentView = MainWindow->GetView(Ouroboros::Dropped);
        break;

        case TAB_ON_HOLD:
            CurrentView = MainWindow->GetView(Ouroboros::OnHold);
        break;

        case TAB_PLAN_TO_WATCH:
            CurrentView = MainWindow->GetView(Ouroboros::PlanToWatch);
        break;
    }
}

/**********************************
 * Sets the headers on the views
 *********************************/
void GUIManager::SetModelHeaders()
{
    QStringList Headers;
    Headers.append(QString("Name"));
    Headers.append(QString("Progress"));
    Headers.append(QString("Rating"));
    Headers.append(QString("Type"));
    //Headers << "Name" << "Progress" << "Rating" << "Type";

    DataModel->setHorizontalHeaderLabels(Headers);
}

/*********************************************
 * Sets up filters needed for views
 *********************************************/
void GUIManager::SetUpFilters()
{

    //TODO: Link filters to view

    //Set up filters for the ui
    //Basically each view will be linked to the model, but we will filter it based on the user status
    //Filters will only get data from ROLE_USER_STATUS role
    Filter_Completed = new QSortFilterProxyModel(this);
    Filter_Completed->setSourceModel(DataModel);
    Filter_Completed->setFilterRole(ROLE_USER_STATUS);
    Filter_Completed->setFilterRegExp(FILTERTEXT_COMPLETED);
    MainWindow->GetView(Ouroboros::Completed)->setModel(Filter_Completed);

    Filter_Dropped = new QSortFilterProxyModel(this);
    Filter_Dropped->setSourceModel(DataModel);
    Filter_Dropped->setFilterRole(ROLE_USER_STATUS);
    Filter_Dropped->setFilterRegExp(FILTERTEXT_DROPPED);
    MainWindow->GetView(Ouroboros::Dropped)->setModel(Filter_Dropped);

    Filter_CurrentlyWatching = new QSortFilterProxyModel(this);
    Filter_CurrentlyWatching->setSourceModel(DataModel);
    Filter_CurrentlyWatching->setFilterRole(ROLE_USER_STATUS);
    Filter_CurrentlyWatching->setFilterRegExp(FILTERTEXT_CURRENTLY_WATCHING);
    MainWindow->GetView(Ouroboros::CurrentlyWatching)->setModel(Filter_CurrentlyWatching);

    Filter_OnHold = new QSortFilterProxyModel(this);
    Filter_OnHold->setSourceModel(DataModel);
    Filter_OnHold->setFilterRole(ROLE_USER_STATUS);
    Filter_OnHold->setFilterRegExp(FILTERTEXT_ON_HOLD);
    MainWindow->GetView(Ouroboros::OnHold)->setModel(Filter_OnHold);

    Filter_PlanToWatch = new QSortFilterProxyModel(this);
    Filter_PlanToWatch->setSourceModel(DataModel);
    Filter_PlanToWatch->setFilterRole(ROLE_USER_STATUS);
    Filter_PlanToWatch->setFilterRegExp(FILTERTEXT_PLAN_TO_WATCH);
    MainWindow->GetView(Ouroboros::PlanToWatch)->setModel(Filter_PlanToWatch);

    //The search filter is special, in the sense that we just need to
    //create it and set it's source model, and when the user types into the search box
    //we filter the string
    Filter_Search = new QSortFilterProxyModel(this);
    Filter_Search->setSourceModel(DataModel);
    Filter_Search->setFilterKeyColumn(HEADER_NAME); //We want to search the names of the anime
}

/***********************************************
 * Sets up the delegates required for the view
 * such as the progress bar for the episodes
 ***********************************************/
void GUIManager::SetUpDelegates()
{

    //TODO: Add delegate to the views

    //Progress bar
    ProgressDelegate *ProgressBar = new ProgressDelegate(this);

    MainWindow->GetView(Ouroboros::CurrentlyWatching)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::OnHold)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::Completed)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::Dropped)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::PlanToWatch)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);

    //connect signals and slots
    connect(ProgressBar,SIGNAL(ButtonClicked(QModelIndex,ProgressDelegate::Button)),this,SLOT(ProgressBarButtonClicked(QModelIndex,ProgressDelegate::Button)));

}
