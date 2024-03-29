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
    void ResetSettings();

    /****************************************** Setting names *******************************************************************/
    //Holds string eqiv values of the settings for saving and loading
    struct SettingsNames
    {
        //Application
        struct ApplicationNames
        {
            QString GroupName;
            QString CloseToTray;
            QString MinimizeToTray;
            QString Stylesheet;
            QString ReplyTimeout;
        }Application;

        //Recognition
        struct RecognitionNames
        {
            QString GroupName;
            QString Enabled;
            QString Delay;
            QString WaitForMPClose;
            QString MPFocus;
            QString NotifyEpisodeRecognised;
            QString NotifyEpisodeNotRecognised;
        }Recognition;


    }SettingsName;

    /****************************************** Settings *******************************************************************/
    //Application settings
    struct ApplicationSettings
    {
        bool CloseToTray;
        bool MinimizeToTray;
        QString Stylesheet;
        int ReplyTimeout;
    }Application;

    //Recognition
    struct RecognitionSettings
    {
        bool Enabled;
        int Delay;
        bool WaitForMPClose;
        bool MPFocus;
        bool NotifyEpisodeRecognised;
        bool NotifyEpisodeNotRecognised;
    }Recognition;

    /****************************************** Defaults *******************************************************************/

    //Default settings
    struct DefaultSettings
    {
        ApplicationSettings Application;
        RecognitionSettings Recognition;
    }Default;

private:
    QString Filename;
};

extern OuroborosSettings Settings;

#endif // SETTINGS_H
