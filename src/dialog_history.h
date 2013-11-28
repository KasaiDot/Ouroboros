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

#ifndef DIALOG_HISTORY_H
#define DIALOG_HISTORY_H

#define HISTORY_HEADER_TITLE 0
#define HISTORY_HEADER_ACTION 1
#define HISTORY_HEADER_TIME 2

#include <QDialog>

#include "historymanager.h"


namespace Ui {
class Dialog_History;
}

class Dialog_History : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_History(QStack<Manager::HistoryItem> &HistoryStack);
    ~Dialog_History();

private:
    Ui::Dialog_History *ui;

    //TreeWidget fucntions
    void SetupTreeWidget();
    void PopulateWidget();

    //local copy of stack
    QStack<Manager::HistoryItem> HistoryStack;

    //menu click actions
    enum HistoryMenuAction
    {
        HistoryMenu_None,
        HistoryMenu_Remove,
        HistoryMenu_Clear
    };

public slots:
    void ShowCustomContextMenu(const QPoint &Pos);

};

#endif // DIALOG_HISTORY_H
