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



void GView::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        QGraphicsView::mousePressEvent(event);
    }
    mouseMoveEvent( event );
}

void GView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        QGraphicsView::mouseMoveEvent(event);
    }
    else
    {
        QPointF point = mapToScene(event->pos());
        QGraphicsItem *tmp = this->scene()->itemAt(point, QTransform());

        GPixmapItem *item =  qgraphicsitem_cast<GPixmapItem*>(tmp);
        if ( item == 0 )
        return;

        if ( event->buttons() == Qt::RightButton )
        {
            item->setPixmap(*podlaha);
            item->typ = 0;
        }
        else if (active == 0)
        {
            item->setPixmap(*podlaha);
            item->typ = active;
        }
        else if (active == 1)
        {
            item->setPixmap(*stena);
            item->typ = active;
        }
        else if (active == 2)
        {
            item->setPixmap(*kluc);
            item->typ = active;
        }
        else
        {
            ;
        }
    }
}


GView::GView(QWidget *parent) : QGraphicsView(parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    podlaha = new QPixmap(podlaha_base);
    stena = new QPixmap(stena_base);
    kluc = new QPixmap(kluc_base);

    //setDragMode(QGraphicsView::ScrollHandDrag);
    setInteractive(false);
}

GView::~GView()
{
    delete podlaha;
    delete stena;
    delete kluc;
    scene()->clear();
}

void GView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
}

void GView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
    else
    {
        QGraphicsView::keyPressEvent(event);
    }
}

void GView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control)
    {
        setDragMode(QGraphicsView::NoDrag);
    }
    else
    {
        QGraphicsView::keyPressEvent(event);
    }
}




void GView::init()
{
    vec.clear();
    scene()->clear();
    for( int i = 0; i< dimension.width(); ++i )
    {
        for ( int j = 0; j < dimension.height(); j++ )
        {
            GPixmapItem* tmp = new GPixmapItem(*podlaha);
            tmp->setPos( podlaha->width()*i, podlaha->height()*j);
            tmp->setToolTip(QString("TEST") + QString::number(i));
            this->scene()->addItem( tmp );
        }

    }
    scene()->setSceneRect(scene()->itemsBoundingRect());
}

void GView::wheelEvent(QWheelEvent* event) {

    const QPointF p0scene = mapToScene(event->pos());
    qreal factor = std::pow(1.001, event->delta());
    scale(factor, factor);
    const QPointF p1mouse = mapFromScene(p0scene);
    const QPointF move = p1mouse - event->pos(); // The move
    horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
    verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
}




