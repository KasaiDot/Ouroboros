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

//Roles
#define ROLE_DEFAULT 0x0100 //Since we can't use Qt::UserRole in the macro, we have to manually assign it
#define ROLE_ANIME_SLUG (ROLE_DEFAULT + 1)
#define ROLE_ANIME_EPISODES (ROLE_DEFAULT + 2)
#define ROLE_USER_STATUS (ROLE_DEFAULT + 3)
#define ROLE_ANIME_STATUS (ROLE_DEFAULT + 4)
#define ROLE_USER_EPISODES 0//Make this equal to the display role so we can sort the progress

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

#include <QObject>
#include <QApplication>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QPainter>
#include <QRect>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QProgressBar>
#include <QStaticText>

#include "ouroboros.h"
#include "settings.h"
#include "globals.h"

/********************************* Delegates *****************************************************/
// A Delegate is basically a control which we can embed into a view, E.g, in a torrent application
// we would use a delegate to add a progress bar to the view

//Progress Bar
class ProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    inline ProgressDelegate(QObject *parent): QStyledItemDelegate(parent) {}

    //Button types
    enum Button
    {
        Plus,
        Minus
    };

    //The paint method is where we do most of the work
    inline void paint(QPainter *Painter, const QStyleOptionViewItem &Option, const QModelIndex &Index) const
    {
        QStyledItemDelegate::paint(Painter,Option,Index);

        //First we check if mouse is hovering above the item
        bool isHovering = ((Option.state & QStyle::State_MouseOver) == QStyle::State_MouseOver);

        //progress bar valies
        int MinValue = 0;
        int CurrentValue = 0;
        int MaxValue = 500;

        //Get the user episode count and anime episode count from the index
        int UserEpisodeCount = Index.data(ROLE_USER_EPISODES).toInt();
        int AnimeEpisodeCount = Index.data(ROLE_ANIME_EPISODES).toInt();

        QString AnimeEpisodeText = QString::number(AnimeEpisodeCount);

        //Check if we have got the anime episode count
        if(AnimeEpisodeCount > ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
        {
            if(UserEpisodeCount > AnimeEpisodeCount)
                UserEpisodeCount = AnimeEpisodeCount;

            MaxValue = AnimeEpisodeCount;
        } else {
            AnimeEpisodeText = "-";
            MaxValue = UserEpisodeCount + 50;
        }

        //Set Current value
        if(UserEpisodeCount < MinValue)
        {
            UserEpisodeCount = MinValue;
        }

        //Do this so we don't divide by zero
        CurrentValue = UserEpisodeCount;
        if(CurrentValue == 0 && AnimeEpisodeCount <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
            CurrentValue = 1;

        //Fill in whole bar if unknown anime episode and completed
        if(Index.data(ROLE_USER_STATUS).toString() == STATUS_COMPLETED)
        {
            if(AnimeEpisodeCount <= ANIMEENTITY_UNKNOWN_ANIME_EPISODE)
                MaxValue = CurrentValue;
        }


        QString UserEpisodeText = QString::number(UserEpisodeCount);
        QString ProgressText = UserEpisodeText + "/" + AnimeEpisodeText;

        //Colors
        QColor TextColor = Settings.ProgressDelegate.TextColor;
        QColor ProgressBarOutlineColor = Settings.ProgressDelegate.ProgressBarOutlineColor;
        QColor ProgressBarBackgroundColor = Settings.ProgressDelegate.ProgressBarBackgroundColor ;
        QColor ProgressBarColor = (Index.data(ROLE_ANIME_STATUS) == ANIME_STATUS_CURRENTLY_AIRING) ? Settings.ProgressDelegate.ProgressBarColor_CurrentlyAiring : Settings.ProgressDelegate.ProgressBarColor_FinishedAiring;

        //create the text rect
        int MaxTextPixelSize = Option.fontMetrics.size(Qt::TextSingleLine,"999/999").width() + 8;
        QRect TextRect(Option.rect.left() + Option.rect.width() - MaxTextPixelSize + 3,Option.rect.top() + 2,MaxTextPixelSize,Option.rect.height());

        //Paint the string
        Painter->save();
        Painter->setRenderHint(QPainter::Antialiasing);
        Painter->setBrush(Qt::transparent);
        Painter->setPen(TextColor);
        Painter->drawText(TextRect,ProgressText);
        Painter->restore();

        //Paint the progressbar outline
        //note if editing progressbaroutline then you have to update the one in editor events
        QRect ProgressBarOutlineRect(Option.rect.left() + 3,Option.rect.top() + 3,Option.rect.width() - MaxTextPixelSize - 6,Option.rect.height() - 6);
        Painter->save();
        Painter->setRenderHint(QPainter::Antialiasing);
        Painter->setBrush(ProgressBarOutlineColor);
        Painter->setPen(QColor(Qt::transparent));
        Painter->drawRect(ProgressBarOutlineRect);
        Painter->restore();

        //Paint the progressbar background
        QRect ProgressBarBackgroundRect(ProgressBarOutlineRect.left() + 1, ProgressBarOutlineRect.top() + 1,ProgressBarOutlineRect.width() - 2, ProgressBarOutlineRect.height() - 2);
        Painter->save();
        Painter->setRenderHint(QPainter::Antialiasing);
        Painter->setBrush(ProgressBarBackgroundColor);
        Painter->setPen(QColor(Qt::transparent));
        Painter->drawRect(ProgressBarBackgroundRect);
        Painter->restore();

        //Paint the progressbar
        int ProgressBarWidth = float((ProgressBarOutlineRect.width() * CurrentValue)/ MaxValue);
        if(ProgressBarWidth > 1 && CurrentValue >= MinValue)
        {
            QRect ProgressBarRect(ProgressBarBackgroundRect.left(),ProgressBarBackgroundRect.top(),ProgressBarWidth - 2,ProgressBarBackgroundRect.height());
            Painter->save();
            Painter->setRenderHint(QPainter::Antialiasing);
            Painter->setBrush(ProgressBarColor);
            Painter->setPen(QColor(Qt::transparent));
            Painter->drawRect(ProgressBarRect);
            Painter->restore();

        } else {
            ProgressBarWidth = 1;
        }


        //Draw the plus and minus boxes if mouse is hovering
        if(isHovering)
        {
            //Global values
           QColor BoxColor = QColor(96,96,96);
            //We draw the + and - boxes around the progress bar
            QStyleOptionGraphicsItem Minus;
            Minus.state = QStyle::State_Enabled;
            Minus.rect = ProgressBarOutlineRect;
            Minus.rect.setWidth(ProgressBarOutlineRect.height());

            Painter->fillRect(Minus.rect,QBrush(BoxColor));
            Painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine,Qt::RoundCap));
            Painter->drawLine(Minus.rect.x() + 3,Minus.rect.y() + (Minus.rect.height() / 2),Minus.rect.x() + Minus.rect.width() - 3,Minus.rect.y() + (Minus.rect.height() / 2));

            QStyleOptionGraphicsItem Plus;
            Plus.state = QStyle::State_Enabled;
            Plus.rect = ProgressBarOutlineRect;
            Plus.rect.setX((ProgressBarOutlineRect.x() + ProgressBarOutlineRect.width() - ProgressBarOutlineRect.height()));
            Plus.rect.setWidth(ProgressBarOutlineRect.height());

            Painter->fillRect(Plus.rect,QBrush(BoxColor));
            Painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine,Qt::RoundCap));
            Painter->drawLine(Plus.rect.x() + 3,Plus.rect.y() + (Plus.rect.height() / 2),Plus.rect.x() + Plus.rect.width() - 3,Plus.rect.y() + (Plus.rect.height() / 2)); //- line
            Painter->drawLine(Plus.rect.x() + (Plus.rect.width() / 2),Plus.rect.y() + 3,Plus.rect.x() + (Plus.rect.width() / 2),Plus.rect.y() + Plus.rect.height() - 3); // | line


            QApplication::style()->drawControl(QStyle::CE_PushButtonLabel,&Minus,Painter);
            QApplication::style()->drawControl(QStyle::CE_PushButtonLabel,&Plus,Painter);

        }
    }

    //Now to see if the user clicked a box, we have to check the editor events
    inline bool editorEvent(QEvent *Event, QAbstractItemModel *Model, const QStyleOptionViewItem &Option, const QModelIndex &Index)
    {
        //Make the the progressbar rect
        int MaxTextPixelSize = QApplication::fontMetrics().size(Qt::TextSingleLine,"999/999").width() + 8;
        QRect ProgressBarOutlineRect(Option.rect.left() + 3,Option.rect.top() + 3,Option.rect.width() - MaxTextPixelSize - 6,Option.rect.height() - 6);

        //Make the hitboxes for the plus and minus buttons
        QRect Minus(ProgressBarOutlineRect);
        Minus.setWidth(Minus.height());

        QRect Plus(ProgressBarOutlineRect);
        Plus.setX(ProgressBarOutlineRect.x() + ProgressBarOutlineRect.width() - ProgressBarOutlineRect.height());
        Plus.setWidth(Plus.height());

        QMouseEvent* MouseEvent = static_cast<QMouseEvent*>(Event);

        //Check for clicks
        if( Event->type() == QEvent::MouseButtonPress)
        {
            //we want to select the row
            emit SelectRow(Index);
        } else if( Event->type() == QEvent::MouseButtonRelease) {

            //User Clicked the button, so we send the signal for GUI_Manager to handle
            if(Minus.contains( MouseEvent->pos()))
                emit ButtonClicked(Index.data(ROLE_ANIME_SLUG).toString(),Button::Minus);
            else if(Plus.contains( MouseEvent->pos()))
                emit ButtonClicked(Index.data(ROLE_ANIME_SLUG).toString(),Button::Plus);
        }
        return true;
    }
signals:
    void ButtonClicked(QString Slug,ProgressDelegate::Button Type);
    void SelectRow(QModelIndex Index);

private:

};


//*************************************************************************************************
namespace Manager
{

class GUIManager : public QObject
{
    Q_OBJECT
public:
    explicit GUIManager(QObject *parent = 0);

    //MUST SET MAIN WINDOW
    void SetMainWindow(Ouroboros *Main);
    void SetUpSearchBox(QLineEdit *SearchBox);

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

    //Slots for the progressbar delegate
    void ProgressBarButtonClicked(QString Slug,ProgressDelegate::Button Type);
   void SelectRow(QModelIndex Index);

   //Tab changed
   void TabChanged(int Tab);
    //Changes tab
   void ChangeTab(int Tab);

   //Context menus
   void ShowViewItemComtextMenu(const QPoint &Pos);

    //view info edit functions
    bool EditUserEpisodes(Anime::AnimeEntity *Entity);

    //push anime to the queue to update in the api
    void UpdateHummingbirdAnime(QString AnimeSlug);

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
    QLineEdit *SearchBox;

    //Functions
    void SetModelHeaders();
    void SetUpFilters();
    void SetUpDelegates();

signals:

};

}

//Global
extern Manager::GUIManager GUI_Manager;
#endif // GUIMANAGER_H
