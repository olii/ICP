#include "GView.h"
#include <QDebug>
#include <QMouseEvent>
#include <QTransform>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QWhatsThis>

void GView::mousePressEvent(QMouseEvent *event)
{
    mouseMoveEvent( event );
}

void GView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsItem *tmp = this->scene()->itemAt(event->localPos(), QTransform());


    QGraphicsPixmapItem *item =  qgraphicsitem_cast<QGraphicsPixmapItem*>(tmp);
    if ( item == 0 )
        return;

    int index = (event->pos().y()/podlaha->size().height()) + (event->pos().x()/podlaha->size().width()) * dimension.height();

    if (index > (dimension.width()*dimension.height()-1) )
        return;

    if ( event->buttons() == Qt::RightButton )
    {
        vec[index].ptr->setPixmap(*podlaha);
        vec[index].item = 0;
    }
    else if (active == 0)
    {
        vec[index].ptr->setPixmap(*podlaha);
        vec[index].item = active;
    }
    else if (active == 1)
    {
        vec[index].ptr->setPixmap(*stena);
        vec[index].item = active;
    }
    else if (active == 2)
    {
        vec[index].ptr->setPixmap(*kluc);
        vec[index].item = active;
    }
    else
    {
        ;
    }
}


GView::GView(QWidget *parent) : QGraphicsView(parent)
{
    podlaha_orig = new QPixmap(podlaha_base);
    stena_orig = new QPixmap(stena_base);
    kluc_orig = new QPixmap(kluc_base);
    podlaha = new QPixmap();
    stena = new QPixmap();
    kluc = new QPixmap();
    resize_textures();
}

GView::~GView()
{
    delete podlaha;
    delete podlaha_orig;
    delete stena;
    delete stena_orig;
    delete kluc;
    delete kluc_orig;
    scene()->clear();
}

void GView::resizeEvent ( QResizeEvent * event )
{
    Q_UNUSED(event);
    resize_textures();

    int dx = podlaha->size().width();
    int dy = podlaha->size().height();

    int m = 0;
    int n = 0;

    QVector<vecStruct>::iterator i;
    for (i = vec.begin(); i != vec.end(); ++i)
    {
        (*i).ptr->setPos( m*dx, n*dy);
        if ((*i).item == 0)
            (*i).ptr->setPixmap(*podlaha);
        else if ((*i).item == 1)
            (*i).ptr->setPixmap(*stena);
        else if ((*i).item == 2)
            (*i).ptr->setPixmap(*kluc);

        n++;

        if ( n >= dimension.height() )
        {
            n = 0;
            m++;
        }
    }
}



void GView::resize_textures()
{
    QSize rect = size();
    QSize size (rect.width()/dimension.width(), rect.height()/dimension.height() );

    *podlaha = podlaha_orig->scaled(size,Qt::IgnoreAspectRatio,Qt::FastTransformation);
    *stena = stena_orig->scaled(size,Qt::IgnoreAspectRatio,Qt::FastTransformation);
    *kluc = kluc_orig->scaled(size,Qt::IgnoreAspectRatio,Qt::FastTransformation);
}




void GView::init()
{
    resize_textures();
    vec.clear();
    scene()->clear();
    for( int i = 0; i< dimension.width(); ++i )
    {
        for ( int j = 0; j < dimension.height(); j++ )
        {
            QGraphicsPixmapItem *tmp = this->scene()->addPixmap( *podlaha );
            tmp->setPos( podlaha->width()*i, podlaha->height()*j);
            tmp->setToolTip(QString("TEST") + QString::number(i));
            vecStruct v {tmp, 0};
            vec.append(v);
        }

    }
}




