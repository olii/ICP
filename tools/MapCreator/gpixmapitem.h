#ifndef GPIXMAPITEM_H
#define GPIXMAPITEM_H

#include <QGraphicsPixmapItem>

class GPixmapItem : public QGraphicsPixmapItem
{
public:
    int typ = 0;



    explicit GPixmapItem();
    GPixmapItem(QPixmap&);

    enum { Type = UserType + 1 };

    int type() const
    {
        return Type;
    }

};

#endif // GPIXMAPITEM_H
