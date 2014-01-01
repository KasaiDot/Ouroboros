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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

//*********************************************************************************************************
//Header column positions
#define HEADER_NAME 0
#define HEADER_PROGRESS 1
#define HEADER_RATING 2
#define HEADER_SHOW_TYPE 3

//Tabs
#define TAB_CURRENTLY_WATCHING 0
#define TAB_COMPLETED 1
#define TAB_ON_HOLD 2
#define TAB_PLAN_TO_WATCH 3
#define TAB_DROPPED 4
#define TAB_SEARCH 5

//Tab names used when adding item count to the name
#define TABNAME_CURRENTLY_WATCHING "Currently watching"
#define TABNAME_COMPLETED "Completed"
#define TABNAME_ON_HOLD "On hold"
#define TABNAME_PLAN_TO_WATCH "Plan to watch"
#define TABNAME_DROPPED "Dropped"
//*********************************************************************************************************


#include <QObject>
#include <QApplication>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QProgressBar>
#include <QStaticText>

#include "ouroboros.h"
#include "settings.h"
#include "globals.h"
#include "animeentity.h"
#include "animedatabase.h"
#include "customguielements.h"

namespace Manager
{

class GUIManager : public QObject
{
    Q_OBJECT
public:
    explicit GUIManager(QObject *parent = 0);

    //MUST SET MAIN WINDOW
    void SetMainWindow(Ouroboros *Main);
    void SetUpSearchBox(CustomGui::EraseableLineEdit *SearchBox);

    QStandardItemModel* GetDataModel() const { return DataModel; }

    //checks if model contains an item based on the Title
    //Note: can return multiple results depending on the Title
    bool ModelContains(QString Title) { return (DataModel->findItems(Title,Qt::MatchFixedString,HEADER_NAME).length() > 0); }

    //Note: can return multiple results depending on the Title
    //Should be used in conjunction with model contains
    QStandardItem *GetItem(QString Title)
    {
        if(ModelContains(Title))
            return (DataModel->findItems(Title,Qt::MatchFixedString,HEADER_NAME).at(0));
        return nullptr;
    }

signals:

public slots:
    //Add entities to the model
    void PopulateModel();
    void ClearModel();
    void AddAnime(Anime::AnimeEntity *Entity);
    void UpdateAnime(QStandardItem *Item,Anime::AnimeEntity *Entity);
    void UpdateAnime(QModelIndex Index,Anime::AnimeEntity *Entity);
    void UpdateAnime(Anime::AnimeEntity *Entity); //This function will get the top most found anime and update it, be careful

    //Sets the data required for the item
    void SetAnimeItemData(QStandardItem *Item_Name,
                          QStandardItem *Item_Progress,
                          QStandardItem *Item_Rating,
                          QStandardItem *Item_Type,
                          Anime::AnimeEntity *Entity);

    //Slots for the progressbar delegate
    void ProgressBarButtonClicked(QString Slug,CustomGui::ProgressDelegate::Button Type);
    void SelectRow(QModelIndex Index);

    //Tab changed
    void TabChanged(int Tab);
    //Changes tab
    void ChangeTab(int Tab);

    //Adds item count to the tabs
    void RenameTabs();
    void RenameTab(int TabIndex, QString TabName, int Count);

    //Context menus
    void ShowViewItemComtextMenu(const QPoint &Pos);

    //Menu functions
    void ShowAnimeInformationDialog(Anime::AnimeEntity &Entity, bool ShowMyInfo = true);
    void ShowSearchDialog(QString Text = "");
    bool EditUserEpisodes(Anime::AnimeEntity *Entity);

    //push anime to the queue to update in the api
    void UpdateHummingbirdAnime(QString AnimeSlug);

    //connect double click signals
    void ConnectDoubleClickSignal(QTreeView *View);
    void HandleDoubleClickSignal(QModelIndex Index);

    //Anime detection functions
    void StartWatching(Anime::AnimeEpisode &Episode,Anime::AnimeEntity *Entity);
    void FinishWatching(Anime::AnimeEpisode &Episode,Anime::AnimeEntity *Entity);

private:
    Ouroboros *MainWindow;
    bool isMainWindowSet;
    QTreeView *CurrentView;

    //Data Model - Holds all data for the view
    QStandardItemModel *DataModel;

    //View Filters
    QSortFilterProxyModel *Filter_CurrentlyWatching;
    QSortFilterProxyModel *Filter_OnHold;
    QSortFilterProxyModel *Filter_Dropped;
    QSortFilterProxyModel *Filter_Completed;
    QSortFilterProxyModel *Filter_PlanToWatch;
    QSortFilterProxyModel *Filter_Search;

    //Search box
    CustomGui::EraseableLineEdit *SearchBox;

    //Functions
    void SetModelHeaders();
    void SetUpFilters();
    void SetUpDelegates();

signals:
    void ShowTrayMessage(QString Title, QString Message, int msecs = 10000);

};

}

//Global
extern Manager::GUIManager GUI_Manager;
#endif // GUIMANAGER_H
