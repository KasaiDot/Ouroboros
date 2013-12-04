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

#ifndef DIALOG_ANIMEINFORMATION_H
#define DIALOG_ANIMEINFORMATION_H

#include <QDialog>
#include "animeentity.h"

namespace Ui {
class Dialog_AnimeInformation;
}

class Dialog_AnimeInformation : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_AnimeInformation(QWidget *parent = 0);
    ~Dialog_AnimeInformation();
    void ParseAnime(Anime::AnimeEntity &Entity, bool ShowMyInfo = true);
    inline QString GetStatusName(int Index)
    {
        switch (Index)
        {
            case 0:
                return QString(STATUS_CURRENTLY_WATCHING);
            break;

            case 1:
                return QString(STATUS_COMPLETED);
            break;

            case 2:
                return QString(STATUS_ON_HOLD);
            break;

            case 3:
                return QString(STATUS_PLAN_TO_WATCH);
            break;

            case 4:
                return QString(STATUS_DROPPED);
            break;

            default:
                return "";
            break;
        }
    }
    inline int GetStatusIndex(QString Status)
    {
        if(Status == STATUS_CURRENTLY_WATCHING)
            return 0;
        else if(Status == STATUS_COMPLETED)
            return 1;
        else if(Status == STATUS_ON_HOLD)
            return 2;
        else if(Status == STATUS_PLAN_TO_WATCH)
            return 3;
        else if(Status == STATUS_DROPPED)
            return 4;

        return 0;
    }

private slots:
    void on_RewatchingCheckBox_toggled(bool Checked);

    void on_MainButtonBox_accepted();

signals:
    void UpdateAnime(Anime::AnimeEntity *Entity);

private:
    Ui::Dialog_AnimeInformation *ui;
    Anime::AnimeEntity *Entity;
    bool ShowMyInfo;
};

#endif // DIALOG_ANIMEINFORMATION_H
