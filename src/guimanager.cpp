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

#include <QHeaderView>
#include <QMenu>
#include <QInputDialog>

#include "animedatabase.h"
#include "filemanager.h"
#include "queuemanager.h"

using namespace Manager;
GUIManager GUI_Manager;

GUIManager::GUIManager(QObject *parent) :
    QObject(parent)
{
    //Set up the models
    DataModel = new QStandardItemModel(this);
    isMainWindowSet = false;
}

/******************************************************
 * Sets the main window and connects signals and slots
 ******************************************************/
void GUIManager::SetMainWindow(Ouroboros *Main)
{
    MainWindow = Main;
    isMainWindowSet = true;

    //Connect signals and slots
    connect(MainWindow->GetMainTabWidget(),SIGNAL(currentChanged(int)),this,SLOT(TabChanged(int)));

    connect(MainWindow->GetView(Ouroboros::CurrentlyWatching),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::OnHold),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::Completed),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::PlanToWatch),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::Dropped),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));

    //Set defaults
    TabChanged(MainWindow->GetMainTabWidget()->currentIndex());

    //Setup required items
    SetUpDelegates();
    SetUpFilters();
}

/********************************************
 * Populates the model with anime entities
 * May have ot be used in conjunction with a
 * thread to stop GUI freezing
 ********************************************/
void GUIManager::PopulateModel()
{
    if(!isMainWindowSet) return;

    //Clear the model
    DataModel->clear();
    SetModelHeaders();

    QList<Anime::AnimeEntity *> AnimeList = Anime_Database.GetAnimeEntities();

    for(QList<Anime::AnimeEntity *>::const_iterator AnimeIt = AnimeList.begin(); AnimeIt != AnimeList.end(); ++AnimeIt)
    {
        Anime::AnimeEntity *Entity = *AnimeIt;
        if(!Entity->GetAnimeSlug().isEmpty())
            AddAnime(Entity);
    }

}

/******************************************
 * Clears the model and deletes all items
 ******************************************/
void GUIManager::ClearModel()
{
    //We can directly clear the rows on the model, because of the way models work they will automatically
    //delete all items that have been appended to it, so we don't need to worr about manually deleting them
    DataModel->removeRows(0,DataModel->rowCount());
}


/************************************************************************
 * Add a single entity to the list
 * Note: Model should be checked first if it already contains the entity
 * if so, then use UpdateAnime function instead
 ************************************************************************/
void GUIManager::AddAnime(Anime::AnimeEntity *Entity)
{
    if(ModelContains(Entity->GetAnimeTitle())) return;
    QString Slug = Entity->GetAnimeSlug();
    //We need to create 5 seperate items then add them to the Datamodel
    QStandardItem *Item_Name = new QStandardItem(Entity->GetAnimeTitle());
    QStandardItem *Item_Progress = new QStandardItem();
    QStandardItem *Item_Rating = new QStandardItem();
    QStandardItem *Item_Type = new QStandardItem(Entity->GetAnimeShowType());


    float UserRating = Entity->GetUserInfo()->GetRatingValue();
    if(UserRating <= 0)
        Item_Rating->setText("-");
    else
        Item_Rating->setText(QString::number(UserRating));

    //Set the anime slug data on each item
    Item_Name->setData(Slug,ROLE_ANIME_SLUG);
    Item_Progress->setData(Slug,ROLE_ANIME_SLUG);
    Item_Rating->setData(Slug,ROLE_ANIME_SLUG);
    Item_Type->setData(Slug,ROLE_ANIME_SLUG);

    //Set the slug, user episode count, etc
    Item_Name->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);
    Item_Progress->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);
    Item_Progress->setData(Entity->GetAnimeStatus(),ROLE_ANIME_STATUS);

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
    QString Slug = Entity->GetAnimeSlug();

    //Since we don't know if the item is the one containing the title, we have to manually get it and the other items
    QStandardItem *Item_Name = DataModel->item(Item->row(),HEADER_NAME);
    QStandardItem *Item_Progress = DataModel->item(Item->row(),HEADER_PROGRESS);
    QStandardItem *Item_Rating = DataModel->item(Item->row(),HEADER_RATING);
    QStandardItem *Item_Type = DataModel->item(Item->row(),HEADER_SHOW_TYPE);

    QString ItemRating = (Entity->GetUserInfo()->GetRatingValue() <= 0) ? "-" : QString::number(Entity->GetUserInfo()->GetRatingValue());

    //set the text
    Item_Name->setText(Entity->GetAnimeTitle());
    Item_Rating->setText(ItemRating);
    Item_Type->setText(Entity->GetAnimeShowType());

    Item_Rating->setTextAlignment(Qt::AlignHCenter);
    Item_Type->setTextAlignment(Qt::AlignHCenter);

    //Set the anime slug data on each item
    Item_Name->setData(Slug,ROLE_ANIME_SLUG);
    Item_Progress->setData(Slug,ROLE_ANIME_SLUG);
    Item_Rating->setData(Slug,ROLE_ANIME_SLUG);
    Item_Type->setData(Slug,ROLE_ANIME_SLUG);

    //set data
    Item_Name->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);
    Item_Progress->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);
    Item_Progress->setData(Entity->GetAnimeStatus(),ROLE_ANIME_STATUS);

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

    QStandardItem *Item_Name = GetItem(Entity->GetAnimeTitle());
    UpdateAnime(Item_Name,Entity);

}

/************************************************************************************************************
 * Handles the button click signal recieved from pressing either the "+" or "-" buttons on the progress bar
 ***********************************************************************************************************/
void GUIManager::ProgressBarButtonClicked(QString Slug, ProgressDelegate::Button Type)
{
    bool Update = false;
    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(Slug);

    if(Type == ProgressDelegate::Plus)
    {
        if((Entity->GetUserInfo()->GetEpisodesWatched() + 1 <= Entity->GetAnimeEpisodeCount()) || Entity->GetAnimeEpisodeCount() <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        {
            Entity->GetUserInfo()->IncrementEpisodeCount();
            Update = true;
        }
    } else if(Type == ProgressDelegate::Minus)
    {
        if(Entity->GetUserInfo()->GetEpisodesWatched() - 1 >= 0)
        {
            Entity->GetUserInfo()->DecrementEpisodeCount();
            Update = true;
        }
    }

    //Update the view
    UpdateAnime(Entity);

    //Update hummingbird anime only when needed, to reduce api calls
    if(Update)
        UpdateHummingbirdAnime(Slug);

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

void SetViewHeaderSize(QTreeView *View)
{
    int NameSize = View->fontMetrics().size(Qt::TextSingleLine,"------------ abcdefghijklmnopqrstuvwxyz 1234567890 ------------").width();
    int ProgressSize = View->fontMetrics().size(Qt::TextSingleLine,"---------------------------------- 999/999").width();
    int RatingSize = View->fontMetrics().size(Qt::TextSingleLine,"------- 10 ------").width();
    int TypeSize = View->fontMetrics().size(Qt::TextSingleLine,"---- Movie ----").width();

    View->header()->resizeSection(HEADER_NAME,NameSize);
    View->header()->resizeSection(HEADER_PROGRESS,ProgressSize);
    View->header()->resizeSection(HEADER_RATING,RatingSize);
    View->header()->resizeSection(HEADER_SHOW_TYPE,TypeSize);

}

void GUIManager::SetModelHeaders()
{
    QStringList Headers;
    Headers << "Name" << "Progress" << "Rating" << "Type";

    DataModel->setHorizontalHeaderLabels(Headers);
    DataModel->horizontalHeaderItem(HEADER_RATING)->setTextAlignment(Qt::AlignHCenter);
    DataModel->horizontalHeaderItem(HEADER_SHOW_TYPE)->setTextAlignment(Qt::AlignHCenter);
    DataModel->horizontalHeaderItem(HEADER_PROGRESS)->setTextAlignment(Qt::AlignHCenter);

    if(!isMainWindowSet) return;

    //Resize views
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::CurrentlyWatching));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::OnHold));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::PlanToWatch));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::Completed));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::Dropped));


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
    Filter_Completed->sort(HEADER_NAME);
    Filter_Completed->setFilterRole(ROLE_USER_STATUS);
    Filter_Completed->setFilterRegExp(STATUS_COMPLETED);
    MainWindow->GetView(Ouroboros::Completed)->setModel(Filter_Completed);

    Filter_Dropped = new QSortFilterProxyModel(this);
    Filter_Dropped->setSourceModel(DataModel);
    Filter_Dropped->sort(HEADER_NAME);
    Filter_Dropped->setFilterRole(ROLE_USER_STATUS);
    Filter_Dropped->setFilterRegExp(STATUS_DROPPED);
    MainWindow->GetView(Ouroboros::Dropped)->setModel(Filter_Dropped);

    Filter_CurrentlyWatching = new QSortFilterProxyModel(this);
    Filter_CurrentlyWatching->setSourceModel(DataModel);
    Filter_CurrentlyWatching->sort(HEADER_NAME);
    Filter_CurrentlyWatching->setFilterRole(ROLE_USER_STATUS);
    Filter_CurrentlyWatching->setFilterRegExp(STATUS_CURRENTLY_WATCHING);
    MainWindow->GetView(Ouroboros::CurrentlyWatching)->setModel(Filter_CurrentlyWatching);

    Filter_OnHold = new QSortFilterProxyModel(this);
    Filter_OnHold->setSourceModel(DataModel);
    Filter_OnHold->sort(HEADER_NAME);
    Filter_OnHold->setFilterRole(ROLE_USER_STATUS);
    Filter_OnHold->setFilterRegExp(STATUS_ON_HOLD);
    MainWindow->GetView(Ouroboros::OnHold)->setModel(Filter_OnHold);

    Filter_PlanToWatch = new QSortFilterProxyModel(this);
    Filter_PlanToWatch->setSourceModel(DataModel);
    Filter_PlanToWatch->sort(HEADER_NAME);
    Filter_PlanToWatch->setFilterRole(ROLE_USER_STATUS);
    Filter_PlanToWatch->setFilterRegExp(STATUS_PLAN_TO_WATCH);
    MainWindow->GetView(Ouroboros::PlanToWatch)->setModel(Filter_PlanToWatch);

    //The search filter is special, in the sense that we just need to
    //create it and set it's source model, and when the user types into the search box
    //we filter the string
    Filter_Search = new QSortFilterProxyModel(this);
    Filter_Search->setSourceModel(DataModel);
    Filter_Search->sort(HEADER_NAME);
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
    connect(ProgressBar,SIGNAL(ButtonClicked(QString,ProgressDelegate::Button)),this,SLOT(ProgressBarButtonClicked(QString,ProgressDelegate::Button)));

}

/*********************************************************
 * Calls the api function to update anime to hummingbird
 *********************************************************/
void GUIManager::UpdateHummingbirdAnime(QString AnimeSlug)
{
    Queue_Manager.UpdateLibrary(AnimeSlug);
}

/************************************************** Context Menus************************************************************************/

/**********************************************************
 * Menu for when user right clicks on an item in the view
 **********************************************************/
void GUIManager::ShowViewItemComtextMenu(const QPoint &Pos)
{
    QModelIndex Index = CurrentView->indexAt(Pos);
    if(Index.row() < 0) return;

    QString Slug = Index.data(ROLE_ANIME_SLUG).toString();
    //Get the anime slug
    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(Slug);

    //Constuct the menu
    QMenu Menu;
    QMenu EditMenu;
    EditMenu.setTitle("Edit");

    //Menu data
    QString Data_EditEpisode = "Edit_Episodes";

    //*************Actions************************

    //***** Edit Menu

    //Edit user episodes
    EditMenu.addAction(MainWindow->GetAction(Ouroboros::EditUserEpisodes)->icon(),MainWindow->GetAction(Ouroboros::EditUserEpisodes)->text())->setData(Data_EditEpisode);

    //Edit status
    QMenu StatusMenu;
    StatusMenu.setTitle("Status");

    QActionGroup *StatusGroup = new QActionGroup(this);
    StatusGroup->setExclusive(true);

    QAction Action_CurrentlyWatching(this);
    Action_CurrentlyWatching.setText("Currently watching");
    Action_CurrentlyWatching.setCheckable(true);
    Action_CurrentlyWatching.setActionGroup(StatusGroup);
    Action_CurrentlyWatching.setData(STATUS_CURRENTLY_WATCHING);

    QAction Action_Dropped(this);
    Action_Dropped.setText("Dropped");
    Action_Dropped.setCheckable(true);
    Action_Dropped.setActionGroup(StatusGroup);
    Action_Dropped.setData(STATUS_DROPPED);

    QAction Action_OnHold(this);
    Action_OnHold.setText("On hold");
    Action_OnHold.setCheckable(true);
    Action_OnHold.setActionGroup(StatusGroup);
    Action_OnHold.setData(STATUS_ON_HOLD);

    QAction Action_Completed(this);
    Action_Completed.setText("Completed");
    Action_Completed.setCheckable(true);
    Action_Completed.setActionGroup(StatusGroup);
    Action_Completed.setData(STATUS_COMPLETED);

    QAction Action_PlanToWatch(this);
    Action_PlanToWatch.setText("Plan to watch");
    Action_PlanToWatch.setCheckable(true);
    Action_PlanToWatch.setActionGroup(StatusGroup);
    Action_PlanToWatch.setData(STATUS_PLAN_TO_WATCH);

    StatusMenu.addAction(&Action_CurrentlyWatching);
    StatusMenu.addAction(&Action_Completed);
    StatusMenu.addAction(&Action_OnHold);
    StatusMenu.addAction(&Action_PlanToWatch);
    StatusMenu.addAction(&Action_Dropped);

    //Set the currently checked one to be users status
    QString CurrentStatus = Entity->GetUserInfo()->GetStatus();
    if(CurrentStatus == STATUS_CURRENTLY_WATCHING)
    {
        Action_CurrentlyWatching.setChecked(true);
    } else if(CurrentStatus == STATUS_COMPLETED)
    {
        Action_Completed.setChecked(true);
    } else if(CurrentStatus == STATUS_DROPPED)
    {
        Action_Dropped.setChecked(true);
    } else if(CurrentStatus == STATUS_ON_HOLD)
    {
        Action_OnHold.setChecked(true);
    } else if(CurrentStatus == STATUS_PLAN_TO_WATCH)
    {
        Action_PlanToWatch.setChecked(true);
    }

    EditMenu.addMenu(&StatusMenu);

    //Edit rating
    float UserRating = Entity->GetUserInfo()->GetRatingValue();

    QMenu RatingMenu;
    RatingMenu.setTitle("Rating");

    QActionGroup *RatingGroup = new QActionGroup(this);
    RatingGroup->addAction("0")->setCheckable(true);
    RatingGroup->addAction("0.5")->setCheckable(true);
    RatingGroup->addAction("1")->setCheckable(true);
    RatingGroup->addAction("1.5")->setCheckable(true);
    RatingGroup->addAction("2")->setCheckable(true);
    RatingGroup->addAction("2.5")->setCheckable(true);
    RatingGroup->addAction("3")->setCheckable(true);
    RatingGroup->addAction("3.5")->setCheckable(true);
    RatingGroup->addAction("4")->setCheckable(true);
    RatingGroup->addAction("4.5")->setCheckable(true);
    RatingGroup->addAction("5")->setCheckable(true);

    foreach(QAction *Action,RatingGroup->actions())
    {
        if(Action->text() == QString::number(UserRating))
        {
            Action->setChecked(true);
        }
    }

    RatingMenu.addActions(RatingGroup->actions());
    EditMenu.addMenu(&RatingMenu);

   //****** Main menu

    //Add edit menu
    Menu.addMenu(&EditMenu);

    QAction *Action = Menu.exec(QCursor::pos());

    if(!Action) return;

    //Edit episode clicked
    if(Action->data().toString() == Data_EditEpisode)
    {
        EditUserEpisodes(Entity);
    }

    //Change status clicked
    if(StatusGroup->checkedAction()->data().toString() != CurrentStatus)
    {
        //User changed status
        Entity->GetUserInfo()->SetStatus(StatusGroup->checkedAction()->data().toString());

        if(ModelContains(Entity->GetAnimeTitle()))
            UpdateAnime(Entity);
    }

    //Changed rating clicked
    if(RatingGroup->checkedAction()->text() != QString::number(UserRating))
    {
        //we have to check the "-" selection aswell
        float NewRating = RatingGroup->checkedAction()->text().toFloat();

        Entity->GetUserInfo()->SetRatingValue(NewRating);

        if(ModelContains(Entity->GetAnimeTitle()))
            UpdateAnime(Entity);
    }

    UpdateHummingbirdAnime(Slug);
}

/************************************************* View info edit functions ******************************************/

/*************************************************
 * Creates a input box for user to type episodes
 ************************************************/
bool GUIManager::EditUserEpisodes(Anime::AnimeEntity *Entity)
{
    bool Ok;
    int MinVal = 0;
    int MaxVal = (Entity->GetAnimeEpisodeCount() <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE) ? 999 : Entity->GetAnimeEpisodeCount();
    int EpisodesWatched = QInputDialog::getInt(MainWindow,"Set episodes watched","Episodes: ",Entity->GetUserInfo()->GetEpisodesWatched(),MinVal,MaxVal,1,&Ok);

    if(!Ok) return false;

    Entity->GetUserInfo()->SetEpisodesWatched(EpisodesWatched,true);

    if(ModelContains(Entity->GetAnimeTitle()))
        UpdateAnime(Entity);

    return true;
}

