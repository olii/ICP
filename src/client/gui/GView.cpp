#include "GView.h"
#include <QDebug>
#include <QMouseEvent>
#include <QTransform>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QWhatsThis>
#include <QScrollBar>
#include "gpixmapitem.h"
#include "boost/serialization/serialization.hpp"
/**
 * @file GView.cpp
 *
 * @brief vedlajsi modul pre implementaciu grafickeho rozhrania aplikacie
 *        implementacia metod vlastnej grafickej entity odvodenej z QGraphicsView
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "mainwindow.h"
#include <QApplication>



GView::GView(QWidget *parent) : QGraphicsView(parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    setDragMode(QGraphicsView::ScrollHandDrag);
    //setInteractive(false);
}

GView::~GView()
{
    //scene()->clear();
}


void GView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(QApplication::activeWindow()->isFullScreen())
    {
        QApplication::activeWindow()->showNormal();
    }
    else
    {
        QApplication::activeWindow()->showFullScreen();
    }
}

void GView::savescale()
{
    scale11 = matrix().m11();
}


qreal GView::getscale()
{
    return scale11;
}

void GView::wheelEvent(QWheelEvent* event) {

    //qDebug() << "zoom event";
    const QPointF p0scene = mapToScene(event->pos());
    qreal factor = std::pow(1.0005, event->delta());
    /* zvacsovanie a zmensovanie je mozne len v urcitom rozsahu */
    if (factor * matrix().m11() < scale11 - 0.00001 || (matrix().m11() > 1.0 && event->delta() > 0))
        return;
    scale(factor, factor);
    const QPointF p1mouse = mapFromScene(p0scene);
    const QPointF move = p1mouse - event->pos(); // The move
    horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
    verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
}




