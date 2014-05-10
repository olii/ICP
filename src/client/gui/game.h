#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QGraphicsScene>
#include <QTimer>
#include <QResizeEvent>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QTime>

#include "informator.h"
#include "GView.h"
#include "../../shared/map.h"

namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int center READ getCenter WRITE setCenter)

public:
    explicit Game(QWidget *parent = 0, int StackedWidget = 0);
    ~Game();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_pushButton_2_clicked();

    //void on_comboBox_activated(const QString &arg1);

    //void on_comboBox_activated(int index);

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

    void LoadMap(std::string , GView *view);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_tableWidget_2_clicked(const QModelIndex &index);

    void on_pushButton_6_clicked();


    void gameLoop();
    void animate(GPixmapItem *&old_item, MapItemsInfo &new_item);
    virtual void closeEvent( QCloseEvent * event);
    virtual void resizeEvent(QResizeEvent * event);
    virtual void keyPressEvent(QKeyEvent * event);

    void on_lineEdit_2_editingFinished();
    void setCenter(int x);
    int getCenter();

    void on_checkBox_toggled(bool checked);

    void on_pushButton_7_clicked();

private:
    void AfterJoinTask();
    void resize(GView *view);
    QGraphicsScene scene;
    Ui::Game *ui;
    QTimer gametimer;
    QTime counter;

    //MapUpdate previousMap;
    MapUpdate currentMap;
    std::list<GPixmapItem*> players; //tu uchivavam pointre na obrazky hracov
    std::list<GPixmapItem*> guards; //tu uchovavam pointre na obrazky strazcov
    std::list<GPixmapItem*> keys; //tu uchovavam pointre na obrazky strazcov
    std::list<GPixmapItem*> doors; //tu uchovavam pointre na obrazky strazcov
    bool inGame = false; // indikuje ci je prave hrac v hre
    QParallelAnimationGroup animacie;

    int timer;
    int fake_int;
    int GetTimer();
    void SetTimer(int miliseconds);

};

#endif // GAME_H
