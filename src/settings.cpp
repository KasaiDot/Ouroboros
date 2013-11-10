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

#include "settings.h"

#include <QApplication>

OuroborosSettings Settings;


OuroborosSettings::OuroborosSettings()
{
    Filename = "Settings.ini";

    //**************Set the names of the setting values*****************************************

    //Progress bar
    SettingsName.ProgressBar.GroupName = "Progress_Bar";
    SettingsName.ProgressBar.TextColor = "TextColor";
    SettingsName.ProgressBar.OutlineColor = "OutlineColor";
    SettingsName.ProgressBar.BackgroundColor = "BackgroundColor";
    SettingsName.ProgressBar.BarColor_CurrentlyAiring = "BarColor_CurrentlyAiring";
    SettingsName.ProgressBar.BarColor_FinishedAiring = "BarColor_FinishedAiring";

    //Progress bar settings
   /* ProgressDelegate.TextColor = Qt::black;
    ProgressDelegate.ProgressBarOutlineColor = QColor(160,160,160);
    ProgressDelegate.ProgressBarBackgroundColor = QColor(250,250,250);
    ProgressDelegate.ProgressBarColor_CurrentlyAiring = QColor(98,226,0);
    ProgressDelegate.ProgressBarColor_FinishedAiring = QColor(51,153,255); */
}

/********************************
 * Loads settings from ini file
 *******************************/
void OuroborosSettings::Load()
{
    QString Filepath = QApplication::applicationDirPath() + "/" + Filename;
    QSettings Settings(Filepath,QSettings::IniFormat);
    SettingsNames::ProgressBarNames ProgressBarName = SettingsName.ProgressBar;

    //Progressbar colors
    Settings.beginGroup(ProgressBarName.GroupName);
    ProgressDelegate.TextColor.setNamedColor(Settings.value(ProgressBarName.TextColor,QColor(Qt::black).name()).toString());
    ProgressDelegate.ProgressBarOutlineColor.setNamedColor(Settings.value(ProgressBarName.OutlineColor,QColor(160,160,160).name()).toString());
    ProgressDelegate.ProgressBarBackgroundColor.setNamedColor(Settings.value(ProgressBarName.BackgroundColor,QColor(250,250,250).name()).toString());
    ProgressDelegate.ProgressBarColor_CurrentlyAiring.setNamedColor(Settings.value(ProgressBarName.BarColor_CurrentlyAiring,QColor(98,226,0).name()).toString());
    ProgressDelegate.ProgressBarColor_FinishedAiring.setNamedColor(Settings.value(ProgressBarName.BarColor_FinishedAiring,QColor(51,153,255).name()).toString());
    Settings.endGroup();
}

/*********************************
 * Saves settings to the ini file
 *********************************/
void OuroborosSettings::Save()
{
    QString Filepath = QApplication::applicationDirPath() + "/" + Filename;
    QSettings Settings(Filepath,QSettings::IniFormat);
    SettingsNames::ProgressBarNames ProgressBarName = SettingsName.ProgressBar;

    Settings.beginGroup(ProgressBarName.GroupName);
    Settings.setValue(ProgressBarName.TextColor,ProgressDelegate.TextColor.name());
    Settings.setValue(ProgressBarName.OutlineColor,ProgressDelegate.ProgressBarOutlineColor.name());
    Settings.setValue(ProgressBarName.BackgroundColor,ProgressDelegate.ProgressBarBackgroundColor.name());
    Settings.setValue(ProgressBarName.BarColor_CurrentlyAiring,ProgressDelegate.ProgressBarColor_CurrentlyAiring.name());
    Settings.setValue(ProgressBarName.BarColor_FinishedAiring,ProgressDelegate.ProgressBarColor_FinishedAiring.name());
    Settings.endGroup();

    Settings.sync();
}
