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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QStringList>

namespace Manager
{

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    ThemeManager();

public slots:
    void LoadThemeList();
    void LoadTheme(QString Filename);
    inline void ClearTheme() { CurrentTheme = ""; emit ThemeChanged(CurrentTheme);}

    QString GetTheme() const { return CurrentTheme; }
    void SetTheme(QByteArray Style) { CurrentTheme = Style; }

    QStringList GetFileList() const { return FileList; }

private:
    QString CurrentTheme;
    QString CurrentFileName;

    //List of the styles
    QStringList FileList;

signals:
    void ThemeChanged(QString NewTheme);
    void ThemeListLoaded();

};

}

extern Manager::ThemeManager Theme_Manager;

#endif // THEMEMANAGER_H
