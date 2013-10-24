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

#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QObject>

namespace Queue
{

class QueueItem : public QObject
{
    Q_OBJECT
public:

    //Item types
    enum ItemType
    {
        Item_Auth,
        Item_GetLibrary
    };

    enum ItemReturn
    {
        ItemReturn_Success,
        ItemReturn_Fail
    };

    //Constructor
    explicit QueueItem(QObject *parent, ItemType Type);

    inline ItemType GetItemType() const { return Type; }
    inline int GetId() const { return Id; }

    // Ovveride the '==' operator to determine if one item is another
    // This is the only way to determine in a QList
    bool operator ==(QueueItem const &Other);

private:

    ItemType Type;
    int Id;

    inline int RandomValue(int Min, int Max) { return qrand() % ((Max + 1) - Min) + Min; }

signals:

public slots:

    int Run();

};

}

#endif // QUEUEITEM_H
