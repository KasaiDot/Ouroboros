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

#ifndef OUROBOROS_H
#define OUROBOROS_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTreeView>

#include "threadmanager.h"
#include "animedatabase.h"
#include "apimanager.h"

namespace Ui {
class Ouroboros;
}

class Ouroboros : public QMainWindow
{
    Q_OBJECT

public:
    explicit Ouroboros(QWidget *parent = 0);
    ~Ouroboros();

    //Views
    enum Views
    {
        CurrentlyWatching,
        OnHold,
        Dropped,
        Completed,
        PlanToWatch
    };

    enum Actions
    {
        EditUserEpisodes
    };

    //Fills the tabs with the view
    void SetViewLayouts();

    //Get Functions
    QTabWidget* GetMainTabWidget();
    QTreeView* GetView(Ouroboros::Views Type);
    QAction* GetAction(Ouroboros::Actions Type);

private:
    Ui::Ouroboros *ui;
};

#endif // OUROBOROS_H
