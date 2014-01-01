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

#include "globals.h"

OuroborosSettings Settings;

OuroborosSettings::OuroborosSettings()
{
    Filename = "Settings.ini";

    //**************Set the names of the setting values*****************************************

    //Application
    SettingsName.Application.GroupName = "Application";
    SettingsName.Application.CloseToTray = "CloseToTray";
    SettingsName.Application.MinimizeToTray = "MinimizeToTray";

    //Recognition
    SettingsName.Recognition.GroupName = "Recognition";
    SettingsName.Recognition.Enabled = "Enabled";
    SettingsName.Recognition.Delay = "Delay";
    SettingsName.Recognition.MPFocus = "MediaPlayerMustBeFocused";
    SettingsName.Recognition.NotifyEpisodeRecognised = "NotifyEpisodeRecognised";
    SettingsName.Recognition.NotifyEpisodeNotRecognised = "NotifyEpisodeNotRecognised";
    SettingsName.Recognition.WaitForMPClose = "WaitForMediaPlayerClose";

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

    //Application
    Default.Application.CloseToTray = true;
    Default.Application.MinimizeToTray = true;

    //Recognition
    Default.Recognition.Enabled = true;
    Default.Recognition.Delay = 120;
    Default.Recognition.WaitForMPClose = false;
    Default.Recognition.MPFocus = true;
    Default.Recognition.NotifyEpisodeRecognised = true;
    Default.Recognition.NotifyEpisodeNotRecognised = false;

    //Progress bar
    Default.ProgressDelegate.TextColor = Qt::black;
    Default.ProgressDelegate.Outline = QColor(160,160,160);
    Default.ProgressDelegate.Background = QColor(250,250,250);
    Default.ProgressDelegate.CurrentlyWatching = QColor(92,213,0);
    Default.ProgressDelegate.Completed = QColor(51,153,255);
    Default.ProgressDelegate.OnHold = QColor(255,230,0);
    Default.ProgressDelegate.Dropped = QColor(255,86,60);

}

/********************************
 * Loads settings from ini file
 *******************************/
void OuroborosSettings::Load()
{
    QString Filepath = QApplication::applicationDirPath() + "/" + Filename;
    QSettings Settings(Filepath,QSettings::IniFormat);

    SettingsNames::ApplicationNames ApplicationName = SettingsName.Application;
    SettingsNames::ProgressBarNames ProgressBarName = SettingsName.ProgressBar;
    SettingsNames::RecognitionNames RecognitionNames = SettingsName.Recognition;

    //Application settings
    Settings.beginGroup(ApplicationName.GroupName);
    Application.CloseToTray = Settings.value(ApplicationName.CloseToTray,Default.Application.CloseToTray).toBool();
    Application.MinimizeToTray = Settings.value(ApplicationName.MinimizeToTray,Default.Application.MinimizeToTray).toBool();
    Settings.endGroup();

    //Recognition
    Settings.beginGroup(RecognitionNames.GroupName);
    Recognition.Enabled = Settings.value(RecognitionNames.Enabled,Default.Recognition.Enabled).toBool();
    Recognition.Delay = Settings.value(RecognitionNames.Delay,Default.Recognition.Delay).toInt();
    Recognition.MPFocus = Settings.value(RecognitionNames.MPFocus,Default.Recognition.MPFocus).toBool();
    Recognition.WaitForMPClose = Settings.value(RecognitionNames.WaitForMPClose,Default.Recognition.WaitForMPClose).toBool();
    Recognition.NotifyEpisodeRecognised = Settings.value(RecognitionNames.NotifyEpisodeRecognised,Default.Recognition.NotifyEpisodeRecognised).toBool();
    Recognition.NotifyEpisodeNotRecognised = Settings.value(RecognitionNames.NotifyEpisodeNotRecognised,Default.Recognition.NotifyEpisodeNotRecognised).toBool();
    Settings.endGroup();

    //Progressbar colors
    Settings.beginGroup(ProgressBarName.GroupName);
    ProgressDelegate.TextColor.setNamedColor(Settings.value(ProgressBarName.TextColor,Default.ProgressDelegate.TextColor.name()).toString());
    ProgressDelegate.Outline.setNamedColor(Settings.value(ProgressBarName.OutlineColor,Default.ProgressDelegate.Outline.name()).toString());
    ProgressDelegate.Background.setNamedColor(Settings.value(ProgressBarName.BackgroundColor,Default.ProgressDelegate.Background.name()).toString());
    ProgressDelegate.CurrentlyWatching.setNamedColor(Settings.value(ProgressBarName.CurrentlyWatching,Default.ProgressDelegate.CurrentlyWatching.name()).toString());
    ProgressDelegate.Completed.setNamedColor(Settings.value(ProgressBarName.Completed,Default.ProgressDelegate.Completed.name()).toString());
    ProgressDelegate.OnHold.setNamedColor(Settings.value(ProgressBarName.OnHold,Default.ProgressDelegate.OnHold.name()).toString());
    ProgressDelegate.Dropped.setNamedColor(Settings.value(ProgressBarName.Dropped,Default.ProgressDelegate.Dropped.name()).toString());
    Settings.endGroup();
}

/*********************************
 * Saves settings to the ini file
 *********************************/
void OuroborosSettings::Save()
{
    QString Filepath = QApplication::applicationDirPath() + "/" + Filename;
    QSettings Settings(Filepath,QSettings::IniFormat);

    SettingsNames::ApplicationNames ApplicationName = SettingsName.Application;
    SettingsNames::ProgressBarNames ProgressBarName = SettingsName.ProgressBar;
    SettingsNames::RecognitionNames RecognitionNames = SettingsName.Recognition;

    //Application
    Settings.beginGroup(ApplicationName.GroupName);
    Settings.setValue(ApplicationName.CloseToTray,Application.CloseToTray);
    Settings.setValue(ApplicationName.MinimizeToTray,Application.MinimizeToTray);
    Settings.endGroup();

    //Recognition
    Settings.beginGroup(RecognitionNames.GroupName);
    Settings.setValue(RecognitionNames.Enabled,Recognition.Enabled);
    Settings.setValue(RecognitionNames.Delay,Recognition.Delay);
    Settings.setValue(RecognitionNames.MPFocus,Recognition.MPFocus);
    Settings.setValue(RecognitionNames.WaitForMPClose,Recognition.WaitForMPClose);
    Settings.setValue(RecognitionNames.NotifyEpisodeRecognised,Recognition.NotifyEpisodeRecognised);
    Settings.setValue(RecognitionNames.NotifyEpisodeNotRecognised,Recognition.NotifyEpisodeNotRecognised);
    Settings.endGroup();

    //Progress bar
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
    ProgressDelegate = Default.ProgressDelegate;
    Application = Default.Application;
    Recognition = Default.Recognition;
}
