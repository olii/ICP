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

void GView::resizeEvent(QResizeEvent *event)
{
    //fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    //QGraphicsView::resizeEvent(event);
}

/*void GView::load(StaticMap &t)
{
    scene()->clear();
    dimension.setHeight(t.items.size());
    dimension.setWidth(t.items[0].size());

    for ( uint i = 0; i<t.items.size(); i++ )
        for (uint j = 0; j<t.items[0].size(); ++j)
        {
            GPixmapItem* tmp = new GPixmapItem(*podlaha);
            tmp->setPos( podlaha->width()*j, podlaha->height()*i);
            tmp->setToolTip(QString("TEST") + QString::number(i));
            switch(t.items[i][j])
            {
                case 0:
                    break;
                case 1:
                    tmp->setPixmap(*stena);
                    break;
                case 2:
                    tmp->setPixmap(*kluc);
                    break;
                case 3:
                    tmp->setPixmap(*hrac);
                    break;
                case 4:
                    tmp->setPixmap(*guard);
                    break;
                case 5:
                    tmp->setPixmap(*brana);
                    break;
                case 6:
                    tmp->setPixmap(*finish);
                    break;
            }
            tmp->typ = t.items[i][j];
            scene()->addItem( tmp );
        }

    scene()->setSceneRect(scene()->itemsBoundingRect());
       fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
       savescale();

}*/

void GView::savescale()
{
    scale11 = matrix().m11();
}


qreal GView::getscale()
{
    return scale11;
}



void GView::init()
{
    //scene()->clear();
    /*for( int i = 0; i< dimension.height(); ++i )
    {
        for ( int j = 0; j < dimension.width(); j++ )
        {
            GPixmapItem* tmp = new GPixmapItem(*podlaha);
            tmp->setPos( podlaha->width()*j, podlaha->height()*i);
            tmp->setToolTip(QString("TEST") + QString::number(i));
            this->scene()->addItem( tmp );
        }

    }
        scene()->setSceneRect(scene()->itemsBoundingRect());

       fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
       savescale();

       qreal factor = std::pow(1.001, 120);
       scale(factor, factor);
       factor = std::pow(1.001, -120);
       scale(factor, factor);
*/
}

void GView::wheelEvent(QWheelEvent* event) {

    //qDebug() << "zoom event";
    const QPointF p0scene = mapToScene(event->pos());
    qreal factor = std::pow(1.0005, event->delta());
    /* zvacsovanie a zmensovanie je mozne len v urcitom rozsahu */
    /*if (factor * matrix().m11() < scale11 - 0.00001 || (matrix().m11() > 1.0 && event->delta() > 0))
        return;*/
    scale(factor, factor);
    const QPointF p1mouse = mapFromScene(p0scene);
    const QPointF move = p1mouse - event->pos(); // The move
    horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
    verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
}




