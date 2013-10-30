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
#define ROLE_USER_EPISODES (ROLE_DEFAULT + 2)
#define ROLE_ANIME_EPISODES (ROLE_DEFAULT + 3)
#define ROLE_USER_STATUS (ROLE_DEFAULT + 4)

//Filter text, used by filters to determine what to filter by
#define FILTERTEXT_COMPLETED "completed"
#define FILTERTEXT_CURRENTLY_WATCHING "currently-watching"
#define FILTERTEXT_ON_HOLD "on-hold"
#define FILTERTEXT_DROPPED "dropped"
#define FILTERTEXT_PLAN_TO_WATCH "plan-to-watch"

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

#include <QObject>
#include <QApplication>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QPainter>
#include <QRect>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QTreeView>

#include "ouroboros.h"

/********************************* Delegates *****************************************************/
// A Delegate is basically a control which we cqan embed into a view, E.g, in a torrent application
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
        //First we check if mouse is hovering above the item
        bool isHovering = ((Option.state & QStyle::State_MouseOver) == QStyle::State_MouseOver);

        //Draw the progress bar
        QStyleOptionProgressBar ProgressBar;
        ProgressBar.state = QStyle::State_Enabled;
        ProgressBar.direction = QApplication::layoutDirection();

        ProgressBar.rect = Option.rect;
        ProgressBar.rect.setY(ProgressBar.rect.y() + 2);
        ProgressBar.rect.setHeight(ProgressBar.rect.height() - 3);

        ProgressBar.fontMetrics = QApplication::fontMetrics();
        ProgressBar.minimum = 0;
        ProgressBar.maximum = 9999; //This will change depending on the anime episode count
        ProgressBar.textAlignment = Qt::AlignHCenter;
        ProgressBar.textVisible = true;

        //Get the user episode count and anime episode count from the index
        int UserEpisodeCount = Index.data(ROLE_USER_EPISODES).toInt();
        int AnimeEpisodeCount = Index.data(ROLE_ANIME_EPISODES).toInt();

        QString AnimeEpisodeText = QString::number(AnimeEpisodeCount);

        //Check if we have got the anime episode count
        if(AnimeEpisodeCount >= 0)
        {
            if(UserEpisodeCount > AnimeEpisodeCount)
                UserEpisodeCount = AnimeEpisodeCount;
        } else {
            AnimeEpisodeText = "-";
        }

        QString UserEpisodeText = QString::number(UserEpisodeCount);
        QString ProgressText = UserEpisodeText + "/" + AnimeEpisodeText;

        //set the progress bar values
        ProgressBar.progress = UserEpisodeCount;
        ProgressBar.text = ProgressText;

        //Finally draw the progress bar
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,&ProgressBar,Painter);

        //Draw the plus and minus boxes if mouse is hovering
        if(isHovering)
        {
            //Global values
            QColor BoxColor = QColor(96,96,96);
            //We draw the + and - boxes around the progress bar
            QStyleOptionGraphicsItem Minus;
            Minus.state = QStyle::State_Enabled;
            Minus.rect = ProgressBar.rect;
            Minus.rect.setWidth(ProgressBar.rect.height());

            Painter->fillRect(Minus.rect,QBrush(BoxColor));
            Painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine,Qt::RoundCap));
            Painter->drawLine(Minus.rect.x() + 3,Minus.rect.y() + (Minus.rect.height() / 2),Minus.rect.x() + Minus.rect.width() - 3,Minus.rect.y() + (Minus.rect.height() / 2));

            QStyleOptionGraphicsItem Plus;
            Plus.state = QStyle::State_Enabled;
            Plus.rect = ProgressBar.rect;
            Plus.rect.setX((ProgressBar.rect.x() + ProgressBar.rect.width() - ProgressBar.rect.height()));
            Plus.rect.setWidth(ProgressBar.rect.height());

            Painter->fillRect(Plus.rect,QBrush(BoxColor));
            Painter->setPen(QPen(QBrush(Qt::white),1,Qt::SolidLine,Qt::RoundCap));
            Painter->drawLine(Plus.rect.x() + 3,Plus.rect.y() + (Plus.rect.height() / 2),Plus.rect.x() + Plus.rect.width() - 3,Plus.rect.y() + (Plus.rect.height() / 2));
            Painter->drawLine(Plus.rect.x() + (Plus.rect.width() / 2),Plus.rect.y() + 3,Plus.rect.x() + (Plus.rect.width() / 2),Plus.rect.y() + Plus.rect.height() - 3);


            QApplication::style()->drawControl(QStyle::CE_PushButtonLabel,&Minus,Painter);
            QApplication::style()->drawControl(QStyle::CE_PushButtonLabel,&Plus,Painter);
        }
    }

    //Now to see if the user clicked a box, we have to check the editor events
    inline bool editorEvent(QEvent *Event, QAbstractItemModel *Model, const QStyleOptionViewItem &Option, const QModelIndex &Index)
    {
        //Make the hitboxes for the plus and minus buttons
        QRect Minus(Option.rect);
        Minus.setY(Minus.y() + 2);
        Minus.setHeight(Minus.height() - 3);
        Minus.setWidth(Minus.height());

        QRect Plus(Option.rect);
        Plus.setY(Plus.y() + 2);
        Plus.setHeight(Plus.height() - 3);
        Plus.setX((Plus.x() + Plus.width() - Plus.height()));
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
                emit ButtonClicked(Index,Button::Minus);
            else if(Plus.contains( MouseEvent->pos()))
                emit ButtonClicked(Index,Button::Plus);
        }
        return true;
    }
signals:
    void ButtonClicked(QModelIndex Index,ProgressDelegate::Button Type);
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

    QStandardItemModel* GetDataModel() const { return DataModel; }

    //Add entities to the model
    void PopulateModel();
    void AddAnime(Anime::AnimeEntity *Entity);
    void UpdateAnime(QStandardItem *Item,Anime::AnimeEntity *Entity);
    void UpdateAnime(QModelIndex Index,Anime::AnimeEntity *Entity);
    void UpdateAnime(Anime::AnimeEntity *Entity); //This function will get the top most found anime and update it, be careful

    //checks if model contains an item based on the name
    //Note: can return multiple results depending on the name
    bool ModelContains(QString Name) { return (DataModel->findItems(Name,Qt::MatchFixedString,HEADER_NAME).length() > 0); }

signals:

public slots:
    //Slots for the progressbar delegate
   void ProgressBarButtonClicked(QModelIndex Index,ProgressDelegate::Button Type);
   void SelectRow(QModelIndex Index);

   //Tab changed
   void TabChanged(int Tab);

private:
    Ouroboros *MainWindow;
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

    //Functions
    void SetViewHeaders();
    void SetUpFilters();
    void SetUpDelegates();

};

}

//Global
extern Manager::GUIManager GUI_Manager;
#endif // GUIMANAGER_H
