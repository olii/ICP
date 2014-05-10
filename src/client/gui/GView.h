#ifndef GVIEW_H
/**
 * @file GView.h
 *
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#define GVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QVector>
#include <QKeyEvent>
#include "gpixmapitem.h"


class GView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GView(QWidget *parent = 0);
    ~GView();
    virtual void wheelEvent(QWheelEvent* event);
    //virtual void resizeEvent(QResizeEvent * event);

    virtual void mouseDoubleClickEvent(QMouseEvent * event);

    //void load(StaticMap &t);
    void savescale();
    qreal getscale();

    QSize dimension {1,1};
    int active = 0;

    qreal scale11 = 1.0;

    void init();


private:

};

#endif // GVIEW_H
