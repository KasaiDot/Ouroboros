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
    void ParseAnime(Anime::AnimeEntity &Entity);

private:
    Ui::Dialog_AnimeInformation *ui;
};

#endif // DIALOG_ANIMEINFORMATION_H
