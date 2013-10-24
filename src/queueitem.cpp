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

#include "queueitem.h"

using namespace Queue;

QueueItem::QueueItem(QObject *parent, ItemType Type) :
    QObject(parent),
    Type(Type)
{
    //We need to generate an id for the item
    Id = RandomValue(0,9999);
}

int QueueItem::Run()
{

}

/*************************************************
 * This operator overload compares the Id and the
 * type of the item to determine if it is the
 * same as another.
 *************************************************/
bool QueueItem::operator ==(const QueueItem &Other)
{
    if((this->GetId() == Other.GetId()) && (this->GetItemType() == Other.GetItemType()))
        return true;

    return false;
}


