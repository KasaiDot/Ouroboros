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
