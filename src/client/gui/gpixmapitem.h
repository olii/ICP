#ifndef GPIXMAPITEM_H
#define GPIXMAPITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

class GPixmapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(int zindex READ getSize WRITE setSize)
public:
    explicit GPixmapItem();
    GPixmapItem(QPixmap&);

    enum { Type = UserType + 1 };

    int type() const
    {
        return Type;
    }
    int typ = 0;

    void setSize(int size);
    int getSize();
    void resetSize();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);

    int x;
    int y;
    uint32_t id;

private:
    int m_size;  // toto je pomer pre animacie, robi zmensovanie resp zvacsovanie
};

#endif // GPIXMAPITEM_H
