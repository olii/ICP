#include "gpixmapitem.h"
#include <QObject>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>




GPixmapItem::GPixmapItem(): QObject()
{
    m_size = 0;
}

GPixmapItem::GPixmapItem(QPixmap &t): QGraphicsPixmapItem(t)
{
    m_size = t.size().width();
}

void GPixmapItem::setSize(int size)
{
    m_size = size;
}

int GPixmapItem::getSize()
{
    return m_size;
}

void GPixmapItem::resetSize()
{
     m_size = pixmap().size().width();
}

QRectF GPixmapItem::boundingRect() const
{
   return QRectF(QPointF(0,0),pixmap().size());
}

void GPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget)
{
    if ( m_size == 512 )
    {
        QGraphicsPixmapItem::paint(painter, option, widget);
    }
    else
    {
        painter->drawPixmap(QRectF(QPointF((512/2)-(m_size/2),(512/2)-(m_size/2)),QSizeF(m_size,m_size)).toRect(), pixmap());
    }
}
