/*
**  Ouroboros, Hummingbird.me Desktop App
**  Copyright (C) 2014, Mikunj Varsani
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
    SettingsName.Application.Stylesheet = "Stylesheet";

    //Recognition
    SettingsName.Recognition.GroupName = "Recognition";
    SettingsName.Recognition.Enabled = "Enabled";
    SettingsName.Recognition.Delay = "Delay";
    SettingsName.Recognition.MPFocus = "MediaPlayerMustBeFocused";
    SettingsName.Recognition.NotifyEpisodeRecognised = "NotifyEpisodeRecognised";
    SettingsName.Recognition.NotifyEpisodeNotRecognised = "NotifyEpisodeNotRecognised";
    SettingsName.Recognition.WaitForMPClose = "WaitForMediaPlayerClose";

    //*********************** Set the default values ******************************************//

    //Application
    Default.Application.CloseToTray = true;
    Default.Application.MinimizeToTray = true;
    Default.Application.Stylesheet = "Default.css";

    //Recognition
    Default.Recognition.Enabled = true;
    Default.Recognition.Delay = 120;
    Default.Recognition.WaitForMPClose = false;
    Default.Recognition.MPFocus = true;
    Default.Recognition.NotifyEpisodeRecognised = true;
    Default.Recognition.NotifyEpisodeNotRecognised = false;

}

/********************************
 * Loads settings from ini file
 *******************************/
void OuroborosSettings::Load()
{
    QString Filepath = QApplication::applicationDirPath() + "/" + Filename;
    QSettings Settings(Filepath,QSettings::IniFormat);

    SettingsNames::ApplicationNames ApplicationName = SettingsName.Application;
    SettingsNames::RecognitionNames RecognitionNames = SettingsName.Recognition;

    //Application settings
    Settings.beginGroup(ApplicationName.GroupName);
    Application.CloseToTray = Settings.value(ApplicationName.CloseToTray,Default.Application.CloseToTray).toBool();
    Application.MinimizeToTray = Settings.value(ApplicationName.MinimizeToTray,Default.Application.MinimizeToTray).toBool();
    Application.Stylesheet = Settings.value(ApplicationName.Stylesheet,Default.Application.Stylesheet).toString();
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
}

/*********************************
 * Saves settings to the ini file
 *********************************/
void OuroborosSettings::Save()
{
    QString Filepath = QApplication::applicationDirPath() + "/" + Filename;
    QSettings Settings(Filepath,QSettings::IniFormat);

    SettingsNames::ApplicationNames ApplicationName = SettingsName.Application;
    SettingsNames::RecognitionNames RecognitionNames = SettingsName.Recognition;

    //Application
    Settings.beginGroup(ApplicationName.GroupName);
    Settings.setValue(ApplicationName.CloseToTray,Application.CloseToTray);
    Settings.setValue(ApplicationName.MinimizeToTray,Application.MinimizeToTray);
    Settings.setValue(ApplicationName.Stylesheet,Application.Stylesheet);
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

    Settings.sync();
}

/**********************************
 * Resets all settings to default
 **********************************/
void OuroborosSettings::ResetSettings()
{
    Application = Default.Application;
    Recognition = Default.Recognition;
}
