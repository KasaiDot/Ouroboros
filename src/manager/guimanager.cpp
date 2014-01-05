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

#include "guimanager.h"

#include <QHeaderView>
#include <QMenu>
#include <QInputDialog>
#include <QToolBar>

#include "api/queuemanager.h"
#include "library/historymanager.h"
#include "manager/filemanager.h"
#include "recognition/mediamanager.h"
#include "ui/dialog_search.h"
#include "ui/dialog_animeinformation.h"

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

    //connects signal when user double clicks an item
    ConnectDoubleClickSignal(MainWindow->GetView(Ouroboros::CurrentlyWatching));
    ConnectDoubleClickSignal(MainWindow->GetView(Ouroboros::OnHold));
    ConnectDoubleClickSignal(MainWindow->GetView(Ouroboros::Completed));
    ConnectDoubleClickSignal(MainWindow->GetView(Ouroboros::PlanToWatch));
    ConnectDoubleClickSignal(MainWindow->GetView(Ouroboros::Dropped));
    ConnectDoubleClickSignal(MainWindow->GetView(Ouroboros::Search));

    //connect context menus
    connect(MainWindow->GetView(Ouroboros::CurrentlyWatching),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::OnHold),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::Completed),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::PlanToWatch),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::Dropped),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));
    connect(MainWindow->GetView(Ouroboros::Search),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowViewItemComtextMenu(QPoint)));

    //Set defaults
    TabChanged(MainWindow->GetMainTabWidget()->currentIndex());

    //Setup required items
    SetUpDelegates();
    SetUpFilters();
    SetUpSearchBox(SearchBox);
}

/*********************************************************************
 * Sets up the signals and slots of the search box and other things
 ********************************************************************/
void GUIManager::SetUpSearchBox(CustomGui::EraseableLineEdit *SearchBox)
{
    //make the search box
    SearchBox = new CustomGui::EraseableLineEdit(MainWindow);
    SearchBox->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    SearchBox->setMinimumWidth(200);
    SearchBox->setMaximumWidth(200);
    SearchBox->setPlaceholderText("Search anime");
    SearchBox->setContentsMargins(5,5,5,5);
    SearchBox->setToolTip("Search anime in local list");

    //Add a spacer widget so the search box is at the end of the toolbar
    QWidget *Spacer = new QWidget(MainWindow);
    Spacer->setMaximumHeight(20);
    Spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    Spacer->setVisible(true);
    Spacer->setObjectName("ToolBarFiller");
    MainWindow->GetMainToolBar()->addWidget(Spacer);

    //add it to the toolbar
    QAction *Action = MainWindow->GetMainToolBar()->addWidget(SearchBox);
    MainWindow->GetMainToolBar()->insertSeparator(Action);

    //Connect signals and slots
    connect(SearchBox,SIGNAL(textChanged(QString)),Filter_Search,SLOT(setFilterFixedString(QString)));
    connect(SearchBox,&QLineEdit::textChanged,[=](){
        if(SearchBox->text().size() > 0)
            ChangeTab(TAB_SEARCH);
    });

    //show search dialog when enter is pressed
    connect(SearchBox,&QLineEdit::returnPressed,[=]() {
        ShowSearchDialog(SearchBox->text());
    });

}

/********************************************
 * Populates the model with anime entities
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

    //Set the name of the tabs
    RenameTabs();

}

/******************************************
 * Clears the model and deletes all items
 ******************************************/
void GUIManager::ClearModel()
{
    //We can directly clear the rows on the model, because of the way models work they will automatically
    //delete all items that have been appended to it, so we don't need to worr about manually deleting them
    DataModel->removeRows(0,DataModel->rowCount());

    //Reset the tab names
    RenameTabs();
}


/************************************************************************
 * Add a single entity to the list
 * Note: Model should be checked first if it already contains the entity
 * if so, then use UpdateAnime function instead
 ************************************************************************/
void GUIManager::AddAnime(Anime::AnimeEntity *Entity)
{
    if(ModelContains(Entity->GetAnimeTitle())) return;
    //We need to create 5 seperate items then add them to the Datamodel
    QStandardItem *Item_Name = new QStandardItem();
    QStandardItem *Item_Progress = new QStandardItem();
    QStandardItem *Item_Rating = new QStandardItem();
    QStandardItem *Item_Type = new QStandardItem();

    //set the data
    SetAnimeItemData(Item_Name,Item_Progress,Item_Rating,Item_Type,Entity);

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

    //Set the data
    SetAnimeItemData(Item_Name,Item_Progress,Item_Rating,Item_Type,Entity);

    //Save the anime
    File_Manager.SaveAnimeEntity(Entity,true);

    //Change the item count of the tabs
    RenameTabs();
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

/************************************************
 * Sets all anime data required for the views
 ************************************************/
void GUIManager::SetAnimeItemData(QStandardItem *Item_Name, QStandardItem *Item_Progress, QStandardItem *Item_Rating, QStandardItem *Item_Type, Anime::AnimeEntity *Entity)
{
    //create the icon and set it
    QString IconLocation = (Entity->GetAnimeStatus() == ANIME_STATUS_CURRENTLY_AIRING) ? ":/Resources/circle-green.png" : ":/Resources/circle-blue.png";
    QIcon AnimeIcon(IconLocation);
    Item_Name->setData(AnimeIcon,Qt::DecorationRole);

    //Set the anime slug data on each item
    QString Slug = Entity->GetAnimeSlug();
    Item_Name->setData(Slug,ROLE_ANIME_SLUG);
    Item_Progress->setData(Slug,ROLE_ANIME_SLUG);
    Item_Rating->setData(Slug,ROLE_ANIME_SLUG);
    Item_Type->setData(Slug,ROLE_ANIME_SLUG);

    //Set the slug, user episode count, etc
    Item_Name->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);
    Item_Progress->setData(Entity->GetUserInfo()->GetStatus(),ROLE_USER_STATUS);

    Item_Progress->setData(Entity->GetAnimeEpisodeCount(),ROLE_ANIME_EPISODES);
    Item_Progress->setData(Entity->GetUserInfo()->GetEpisodesWatched(),ROLE_USER_EPISODES);

    //set the display text
    QString ItemRating = (Entity->GetUserInfo()->GetRatingValue() <= 0) ? "-" : QString::number(Entity->GetUserInfo()->GetRatingValue());

    Item_Name->setText(Entity->GetAnimeTitle());
    Item_Rating->setText(ItemRating);
    Item_Type->setText(Entity->GetAnimeShowType());

    //Set alignment
    Item_Rating->setTextAlignment(Qt::AlignCenter);
    Item_Type->setTextAlignment(Qt::AlignCenter);

    //hide text in progress bar column
    Item_Progress->font().setPixelSize(1);
    Item_Progress->setTextAlignment(Qt::AlignCenter);
}

/************************************************************************************************************
 * Handles the button click signal recieved from pressing either the "+" or "-" buttons on the progress bar
 ***********************************************************************************************************/
void GUIManager::ProgressBarButtonClicked(QString Slug, CustomGui::ProgressDelegate::Button Type)
{
    bool Update = false;
    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(Slug);

    if(Type == CustomGui::ProgressDelegate::Plus)
    {
        if((Entity->GetUserInfo()->GetEpisodesWatched() + 1 <= Entity->GetAnimeEpisodeCount()) || Entity->GetAnimeEpisodeCount() <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        {
            Entity->GetUserInfo()->IncrementEpisodeCount();
            Update = true;
        }
    } else if(Type == CustomGui::ProgressDelegate::Minus)
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

    QString Action = "Watched Episode: %1";
    History_Manager.AddHistoryItem(Entity->GetAnimeTitle(),Action.arg(QString::number(Entity->GetUserInfo()->GetEpisodesWatched())),QDateTime::currentDateTime().toString(HISTORY_DATEFORMAT));

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

    case TAB_SEARCH:
        CurrentView = MainWindow->GetView(Ouroboros::Search);
        break;
    }
}

/***************************************
 * Programatically changes current tab
 ***************************************/
void GUIManager::ChangeTab(int Tab)
{
    MainWindow->GetMainTabWidget()->setCurrentIndex(Tab);
}

/*******************************
 * Adds item count to the tab
 *******************************/
void GUIManager::RenameTabs()
{
    RenameTab(TAB_CURRENTLY_WATCHING,TABNAME_CURRENTLY_WATCHING,Filter_CurrentlyWatching->rowCount());
    RenameTab(TAB_COMPLETED,TABNAME_COMPLETED,Filter_Completed->rowCount());
    RenameTab(TAB_ON_HOLD,TABNAME_ON_HOLD,Filter_OnHold->rowCount());
    RenameTab(TAB_PLAN_TO_WATCH,TABNAME_PLAN_TO_WATCH,Filter_PlanToWatch->rowCount());
    RenameTab(TAB_DROPPED,TABNAME_DROPPED,Filter_Dropped->rowCount());
}

void GUIManager::RenameTab(int TabIndex,QString TabName, int Count)
{
    QString NewName = QString(TabName + " (%1)").arg(QString::number(Count));
    MainWindow->GetMainTabWidget()->tabBar()->setTabText(TabIndex,NewName);
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
    DataModel->horizontalHeaderItem(HEADER_RATING)->setTextAlignment(Qt::AlignCenter);
    DataModel->horizontalHeaderItem(HEADER_SHOW_TYPE)->setTextAlignment(Qt::AlignCenter);
    DataModel->horizontalHeaderItem(HEADER_PROGRESS)->setTextAlignment(Qt::AlignCenter);

    if(!isMainWindowSet) return;

    //Resize views
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::CurrentlyWatching));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::OnHold));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::PlanToWatch));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::Completed));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::Dropped));
    SetViewHeaderSize(MainWindow->GetView(Ouroboros::Search));


}

/*********************************************
 * Sets up filters needed for views
 *********************************************/
void GUIManager::SetUpFilters()
{
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
    Filter_Search->setFilterCaseSensitivity(Qt::CaseInsensitive);
    Filter_Search->sort(HEADER_NAME);
    Filter_Search->setFilterKeyColumn(HEADER_NAME); //We want to search the names of the anime
    MainWindow->GetView(Ouroboros::Search)->setModel(Filter_Search);
}

/***********************************************
 * Sets up the delegates required for the view
 * such as the progress bar for the episodes
 ***********************************************/
void GUIManager::SetUpDelegates()
{
    //Progress bar
    CustomGui::ProgressDelegate *ProgressBar = new CustomGui::ProgressDelegate(this, MainWindow->AnimeProgressStyle);

    MainWindow->GetView(Ouroboros::CurrentlyWatching)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::OnHold)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::Completed)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::Dropped)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::PlanToWatch)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);
    MainWindow->GetView(Ouroboros::Search)->setItemDelegateForColumn(HEADER_PROGRESS,ProgressBar);

    //connect signals and slots
    connect(ProgressBar,SIGNAL(ButtonClicked(QString,CustomGui::ProgressDelegate::Button)),this,SLOT(ProgressBarButtonClicked(QString,CustomGui::ProgressDelegate::Button)));
}

/*********************************************************
 * Calls the api function to update anime to hummingbird
 *********************************************************/
void GUIManager::UpdateHummingbirdAnime(QString AnimeSlug)
{
    Queue_Manager.UpdateLibrary(AnimeSlug);
}

/*************************************************************************
 * Connects the double click signals of the view to the handle function
 *************************************************************************/
void GUIManager::ConnectDoubleClickSignal(QTreeView *View)
{
    connect(View,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(HandleDoubleClickSignal(QModelIndex)));
}

/*********************************************
 * Handles double click signal
 * Currently just displays anime information
 *********************************************/
void GUIManager::HandleDoubleClickSignal(QModelIndex Index)
{
    if(Index.row() < 0) return;
    QString Slug = Index.data(ROLE_ANIME_SLUG).toString();

    if(!Anime_Database.Contains(Slug)) return;

    Anime::AnimeEntity *Entity = Anime_Database.GetAnime(Slug);

    //show anime information
    ShowAnimeInformationDialog(*Entity);
}

/*****************************************************************
 * Sets up GUI for when user starts/finishes watching an anime
 *****************************************************************/
void GUIManager::StartWatching(Anime::AnimeEpisode &Episode, Anime::AnimeEntity *Entity)
{
    //Add anime to database if it doesn't exists
    if(!Anime_Database.Contains(Entity->GetAnimeSlug())) Anime_Database.AddAnime(Entity);

    //Change the status
    MainWindow->PlayStatus = Ouroboros::PLAYSTATUS_PLAYING;

    //Show tray message
    //TODO: Add more information
    if(Settings.Recognition.NotifyEpisodeRecognised)
    {
        QString Message;
        Message.append(QString("Watching: %1 \n").arg(Episode.Title));
        Message.append(QString("Episode: %1 \n").arg(Episode.GetEpisodeNumberHigh()));
        emit ShowTrayMessage("Anime Detected",Message);
    }

    //Update anime if update delay is zero and we don't wait for media player to close
    if(Settings.Recognition.Delay == 0 && !Settings.Recognition.WaitForMPClose)
        Anime_Database.UpdateEntity(Episode,Entity);
}

void GUIManager::FinishWatching(Anime::AnimeEpisode &Episode, Anime::AnimeEntity *Entity)
{
    //Change the status
    MainWindow->PlayStatus = Ouroboros::PLAYSTATUS_STOPPED;

    //Set the slug
    Episode.Slug = ANIMEDATABASE_UKNOWN_SLUG;
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

    //Construct the menus with mainwindow as the parent so that they inherit stylesheets
    QMenu Menu(MainWindow);
    \
    QMenu EditMenu(MainWindow);
    EditMenu.setTitle("Edit");

    QMenu StatusMenu(MainWindow);
    StatusMenu.setTitle("Status");

    QMenu RatingMenu(MainWindow);
    RatingMenu.setTitle("Rating");

    //Menu data
    QString Data_Information = "Information";
    QString Data_EditEpisode = "Edit_Episodes";

    //*************Actions************************

    //Information
    Menu.addAction("Information")->setData(Data_Information);

    //***** Edit Menu

    //Edit user episodes
    EditMenu.addAction(MainWindow->GetAction(Ouroboros::EditUserEpisodes)->icon(),MainWindow->GetAction(Ouroboros::EditUserEpisodes)->text())->setData(Data_EditEpisode);

    /*********** Status Menu ****************************/
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

    /********** Rating menu ******************/
    float UserRating = Entity->GetUserInfo()->GetRatingValue();

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

    /*********** Main menu ******************/

    //Add edit menu
    Menu.addMenu(&EditMenu);

    /******************************************** Show Menu and handle actions *****************************************************/
    QAction *Action = Menu.exec(QCursor::pos());

    if(!Action) return;

    //Information clicked
    if(Action->data().toString() == Data_Information)
    {
        ShowAnimeInformationDialog(*Entity);
    }

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

        QString Action = "Changed Status: %1";
        History_Manager.AddHistoryItem(Entity->GetAnimeTitle(),Action.arg(Entity->GetUserInfo()->GetStatus()),QDateTime::currentDateTime().toString(HISTORY_DATEFORMAT));
    }

    //Changed rating clicked
    if(RatingGroup->checkedAction()->text() != QString::number(UserRating))
    {
        //we have to check the "-" selection aswell
        float NewRating = RatingGroup->checkedAction()->text().toFloat();

        Entity->GetUserInfo()->SetRatingValue(NewRating);

        if(ModelContains(Entity->GetAnimeTitle()))
            UpdateAnime(Entity);

        QString Action = "Rating Changed: %1";
        History_Manager.AddHistoryItem(Entity->GetAnimeTitle(),Action.arg(QString::number(Entity->GetUserInfo()->GetRatingValue())),QDateTime::currentDateTime().toString(HISTORY_DATEFORMAT));
    }

    UpdateHummingbirdAnime(Slug);
}

/************************************************* View info edit functions ******************************************/

/**************************************
 * Shows the anime information dialog
 *************************************/
void GUIManager::ShowAnimeInformationDialog(Anime::AnimeEntity &Entity, bool ShowMyInfo)
{
    Dialog_AnimeInformation InfoDialog;

    connect(&InfoDialog,SIGNAL(UpdateAnime(Anime::AnimeEntity*)),this,SLOT(UpdateAnime(Anime::AnimeEntity*)));
    connect(&InfoDialog,&Dialog_AnimeInformation::UpdateAnime,[=](Anime::AnimeEntity *AnimeEntity)
    {
        UpdateHummingbirdAnime(AnimeEntity->GetAnimeSlug());
    });

    InfoDialog.ParseAnime(Entity,ShowMyInfo);
    InfoDialog.exec();
}

/***************************************************************
 * Shows the search dialog with given text as the search text
 ***************************************************************/
void GUIManager::ShowSearchDialog(QString Text)
{
    Dialog_Search SearchDialog;
    if(!Text.isEmpty()) SearchDialog.SetSearchString(Text);
    SearchDialog.exec();
}


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
    if(EpisodesWatched == Entity->GetUserInfo()->GetEpisodesWatched()) return false;

    Entity->GetUserInfo()->SetEpisodesWatched(EpisodesWatched,true);

    if(ModelContains(Entity->GetAnimeTitle()))
        UpdateAnime(Entity);

    QString Action = "Watched Episode: %1";
    History_Manager.AddHistoryItem(Entity->GetAnimeTitle(),Action.arg(QString::number(Entity->GetUserInfo()->GetEpisodesWatched())),QDateTime::currentDateTime().toString(HISTORY_DATEFORMAT));

    return true;
}

