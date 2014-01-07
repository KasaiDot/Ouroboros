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

#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QObject>
#include <QStringList>

namespace Manager
{

class StyleManager : public QObject
{
    Q_OBJECT
public:
    StyleManager();

public slots:
    void LoadStyleList();
    void LoadStyle(QString Filename);
    inline void ClearStyle() { CurrentStyle = ""; emit StyleChanged(CurrentStyle);}

    QByteArray GetStyle() const { return CurrentStyle; }
    void SetStyle(QByteArray Style) { CurrentStyle = Style; }

    QStringList GetFileList() const { return FileList; }

private:
    QByteArray CurrentStyle;
    QString CurrentFileName;

    //List of the styles
    QStringList FileList;

signals:
    void StyleChanged(QByteArray NewStyle);
    void StyleListLoaded();

};

}

extern Manager::StyleManager Style_Manager;

#endif // STYLEMANAGER_H
