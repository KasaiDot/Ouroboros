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

#include "dialog_history.h"
#include "ui_dialog_history.h"

#include <QMenu>

Dialog_History::Dialog_History(QStack<Manager::HistoryItem> &HistoryStack) :
    ui(new Ui::Dialog_History),
    HistoryStack(HistoryStack)
{
    ui->setupUi(this);
    SetupTreeWidget();
    PopulateWidget();

    //connect signals and slots
    connect(ui->HistoryTreeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ShowCustomContextMenu(QPoint)));
}

Dialog_History::~Dialog_History()
{
    delete ui;
}

/*************************
 * Configures tree widget
 *************************/
void Dialog_History::SetupTreeWidget()
{
    QFontMetrics Font = ui->HistoryTreeWidget->fontMetrics();

    ui->HistoryTreeWidget->header()->setSectionsMovable(false);

    ui->HistoryTreeWidget->header()->resizeSection(HISTORY_HEADER_TIME, Font.width("**** 99:99:99 99/99/99 ****"));
    ui->HistoryTreeWidget->header()->resizeSection(HISTORY_HEADER_TITLE, Font.width("**** abcdefghijklmnopqrstuvwxyz123456789 ****"));
    ui->HistoryTreeWidget->header()->setSectionResizeMode(HISTORY_HEADER_ACTION,QHeaderView::Stretch);
    ui->HistoryTreeWidget->header()->setSectionResizeMode(HISTORY_HEADER_TIME,QHeaderView::Interactive);
}

/*************************************
 * Adds history items to the widget
 ************************************/
void Dialog_History::PopulateWidget()
{
    //clear current items
    ui->HistoryTreeWidget->clear();

    QList<Manager::HistoryItem> ItemList = HistoryStack.toList();
    QList<Manager::HistoryItem>::const_iterator ItemIterator;

    //iterate through items
    for(ItemIterator = ItemList.begin(); ItemIterator != ItemList.end(); ++ItemIterator)
    {
        //construct the string list
        Manager::HistoryItem Item = (*ItemIterator);
        QStringList DataList;
        DataList << Item.AnimeTitle << Item.Action << Item.DateAndTime;

        //add the item to the end of the list
        QTreeWidgetItem *TreeItem = new QTreeWidgetItem(DataList);
        ui->HistoryTreeWidget->insertTopLevelItem(ui->HistoryTreeWidget->topLevelItemCount(),TreeItem);
    }

}

/************************************************
 * Shows the context menu when item is clicked
 ************************************************/
void Dialog_History::ShowCustomContextMenu(const QPoint &Pos)
{
    QModelIndex Index = ui->HistoryTreeWidget->indexAt(Pos);
    if(Index.row() < 0) return;

    QMenu Menu;

    //Add the items
    Menu.addAction("Remove item")->setData(REMOVE);
    Menu.addSeparator();
    Menu.addAction("Clear history")->setData(CLEAR);

    /**************************** Show menu ****************************/
    QAction *Action = Menu.exec(QCursor::pos());

    if(!Action) return;

    int ActionValue = Action->data().toInt();

    switch (ActionValue)
    {
        case REMOVE:
            History_Manager.RemoveHistoryItem(Index.row());
            delete ui->HistoryTreeWidget->takeTopLevelItem(Index.row());
        break;

        case CLEAR:
            History_Manager.ClearHistory();
            ui->HistoryTreeWidget->clear();
        break;
    default:
        break;
    }

}
