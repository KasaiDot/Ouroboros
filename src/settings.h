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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>

class OuroborosSettings
{
public:
    OuroborosSettings();
    //Basic functions
    bool Load();
    bool Save();

    //Since we can't apply a stylesheet to the progress delegate,
    //we get the colors the user sets
    struct ProgressDelegateSettings
    {
        QColor TextColor;
        QColor ProgressBarOutlineColor;
        QColor ProgressBarBackgroundColor;
        QColor ProgressBarColor_CurrentlyAiring;
        QColor ProgressBarColor_FinishedAiring;

    } ProgressDelegate;

};

extern OuroborosSettings Settings;

#endif // SETTINGS_H
