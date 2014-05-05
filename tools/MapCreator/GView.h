#ifndef GVIEW_H
#define GVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QVector>
#include <QKeyEvent>
#include "gpixmapitem.h"
#include "map.h"


const QString stena_base = "://resources/wall.png";
const QString podlaha_base = "://resources/grass.jpg";
const QString kluc_base = "://resources/key.png";
const QString playerSpawn = "://resources/PlayerSpawn.png";
const QString guardSpawn = "://resources/guard.png";
const QString door = "://resources/door.png";

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

    void load(StaticMap &t);

    QPixmap *podlaha = 0;
    QPixmap *stena = 0;
    QPixmap *kluc = 0;
    QPixmap *hrac = 0;
    QPixmap *guard = 0;
    QPixmap *brana = 0;
    QSize dimension {1,1};
    int active = 0;
    QVector< vecStruct > vec;

    float floatx = 0.0;
    float floaty = 0.0;

    void init();


};

#endif // GVIEW_H
