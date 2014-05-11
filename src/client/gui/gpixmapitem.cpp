/**
 * @file gpixmapitem.cpp
 *
 * @brief vedlajsi modul pre implementaciu grafickeho rozhrania aplikacie
 *        pomocny modul pre obsluhu animacii
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "gpixmapitem.h"
#include "informator.h"
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
    if ( m_size == TEXTURE_SIZE ) // HARDCODED konstanta, zmen na tu tvoju
    {
        QGraphicsPixmapItem::paint(painter, option, widget);
    }
    else
    {
        //cenrovanie vykreslenia na stred - pouzite pri animaciach
        painter->drawPixmap(QRectF(QPointF((TEXTURE_SIZE/2)-(m_size/2),(TEXTURE_SIZE/2)-(m_size/2)),QSizeF(m_size,m_size)).toRect(), pixmap());
    }
}
