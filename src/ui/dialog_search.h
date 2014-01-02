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

#ifndef DIALOG_SEARCH_H
#define DIALOG_SEARCH_H

//*********************************************************************************************************
#define SEARCH_HEADER_TITLE 0
#define SEARCH_HEADER_EPISODE 1
#define SEARCH_HEADER_TYPE 2
//*********************************************************************************************************

#include <QDialog>
#include "QList"

#include "api/apimanager.h"
#include "library/animedatabase.h"
#include "ui/customguielements.h"

namespace Ui {
class Dialog_Search;
}

class Dialog_Search : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Search(QWidget *parent = 0);
    ~Dialog_Search();

    void SetupSearchBox();
    void SetupTreeWidget();
    void SetSearchString(QString String);
    void ParseAnime(QByteArray &Data);
    void PopulateWidget();

    //used for stopping dialog closing when enter is pressed
    bool eventFilter(QObject *Object, QEvent *Event);

    //Menu actions
    enum SearchMenuActions
    {
        SearchMenuAction_CurrentlyWatching,
        SearchMenuAction_Completed,
        SearchMenuAction_Dropped,
        SearchMenuAction_PlanToWatch,
        SearchMenuAction_OnHold,
        SearchMenuAction_ViewInformation
    };

private:
    Ui::Dialog_Search *ui;

    //Search field
    CustomGui::EraseableLineEdit *SearchBox;
    bool Searching;

    //Temporary Hash to store anime
    QHash<QString,Anime::AnimeEntity> AnimeList;

public slots:
    void SearchAnime();
    void ShowCustomContextMenu(const QPoint &Pos);
    void HandleDoubleClick(QModelIndex Index);

};

#endif // DIALOG_SEARCH_H
