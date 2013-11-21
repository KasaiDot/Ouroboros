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
#include <QSettings>

class OuroborosSettings
{
public:
    OuroborosSettings();
    //Basic functions
    void Load();
    void Save();

    //Holds string eqiv values of the settings for saving and loading
    struct SettingsNames
    {
        //progressbar
        struct ProgressBarNames
        {
            QString GroupName;
            QString TextColor;
            QString OutlineColor;
            QString BackgroundColor;
            QString BarColor_CurrentlyWatching;
            QString BarColor_Completed;
            QString BarColor_OnHold;
            QString BarColor_Dropped;

        }ProgressBar;


    }SettingsName;

    //Since we can't apply a stylesheet to the progress delegate,
    //we get the colors the user sets
    struct ProgressDelegateSettings
    {
        QColor TextColor;
        QColor ProgressBarOutlineColor;
        QColor ProgressBarBackgroundColor;
        QColor ProgressBarColor_CurrentlyWatching;
        QColor ProgressBarColor_Completed;
        QColor ProgressBarColor_OnHold;
        QColor ProgressBarColor_Dropped;

    } ProgressDelegate;


private:
    QString Filename;
};




extern OuroborosSettings Settings;

#endif // SETTINGS_H
