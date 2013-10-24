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


