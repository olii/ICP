#ifndef GVIEW_H
#define GVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QVector>
#include <QKeyEvent>
#include "gpixmapitem.h"


const QString stena_base = "://resources/cervena.png";
const QString podlaha_base = "://resources/33usewk.png";
const QString kluc_base = "://resources/kluc2.png";

struct vecStruct{
    QGraphicsPixmapItem* ptr;
    int item;
};

class GView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GView(QWidget *parent = 0);
    ~GView();
    virtual void mousePressEvent( QMouseEvent * event );
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent ( QResizeEvent * event );
    virtual void keyPressEvent ( QKeyEvent * event );
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    QPixmap *podlaha = 0;
    QPixmap *stena = 0;
    QPixmap *kluc = 0;
    QSize dimension {1,1};
    int active = 0;
    QVector< vecStruct > vec;

    float floatx = 0.0;
    float floaty = 0.0;

    void init();


};

#endif // GVIEW_H
