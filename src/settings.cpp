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
#include <QDebug>

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
    SettingsName.ProgressBar.CurrentlyWatching = "BarColor_CurrentlyWatching";
    SettingsName.ProgressBar.Completed = "BarColor_Completed";
    SettingsName.ProgressBar.OnHold = "BarColor_OnHold";
    SettingsName.ProgressBar.Dropped = "BarColor_Dropped";

    //*********************** Set the default values ******************************************//

    //Progress bar
    Default.DefaultProgressDelegate.TextColor = Qt::black;
    Default.DefaultProgressDelegate.Outline = QColor(160,160,160);
    Default.DefaultProgressDelegate.Background = QColor(250,250,250);
    Default.DefaultProgressDelegate.CurrentlyWatching = QColor(92,213,0);
    Default.DefaultProgressDelegate.Completed = QColor(51,153,255);
    Default.DefaultProgressDelegate.OnHold = QColor(255,230,0);
    Default.DefaultProgressDelegate.Dropped = QColor(255,86,60);

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
    ProgressDelegate.TextColor.setNamedColor(Settings.value(ProgressBarName.TextColor,Default.DefaultProgressDelegate.TextColor.name()).toString());
    ProgressDelegate.Outline.setNamedColor(Settings.value(ProgressBarName.OutlineColor,Default.DefaultProgressDelegate.Outline.name()).toString());
    ProgressDelegate.Background.setNamedColor(Settings.value(ProgressBarName.BackgroundColor,Default.DefaultProgressDelegate.Background.name()).toString());
    ProgressDelegate.CurrentlyWatching.setNamedColor(Settings.value(ProgressBarName.CurrentlyWatching,Default.DefaultProgressDelegate.CurrentlyWatching.name()).toString());
    ProgressDelegate.Completed.setNamedColor(Settings.value(ProgressBarName.Completed,Default.DefaultProgressDelegate.Completed.name()).toString());
    ProgressDelegate.OnHold.setNamedColor(Settings.value(ProgressBarName.OnHold,Default.DefaultProgressDelegate.OnHold.name()).toString());
    ProgressDelegate.Dropped.setNamedColor(Settings.value(ProgressBarName.Dropped,Default.DefaultProgressDelegate.Dropped.name()).toString());
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
    Settings.setValue(ProgressBarName.OutlineColor,ProgressDelegate.Outline.name());
    Settings.setValue(ProgressBarName.BackgroundColor,ProgressDelegate.Background.name());
    Settings.setValue(ProgressBarName.CurrentlyWatching,ProgressDelegate.CurrentlyWatching.name());
    Settings.setValue(ProgressBarName.Completed,ProgressDelegate.Completed.name());
    Settings.setValue(ProgressBarName.OnHold,ProgressDelegate.OnHold.name());
    Settings.setValue(ProgressBarName.Dropped,ProgressDelegate.Dropped.name());
    Settings.endGroup();

    Settings.sync();
}

/**********************************
 * Resets all settings to default
 **********************************/
void OuroborosSettings::ResetSettings()
{
    //progressbar
    ProgressDelegate = Default.DefaultProgressDelegate;
}
