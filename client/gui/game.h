#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QGraphicsScene>
#include <QTimer>
#include <QObject>
#include <QParallelAnimationGroup>

#include "informator.h"
#include "GView.h"

namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT

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

    virtual void resizeEvent(QResizeEvent * event);

    void LoadMap(std::string , GView *view);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_tableWidget_2_clicked(const QModelIndex &index);

    void on_pushButton_6_clicked();


    void gameLoop();
    virtual void closeEvent( QCloseEvent * event);

private:
    void AfterJoinTask();
    QGraphicsScene scene;
    Ui::Game *ui;
    QTimer gametimer;

    //MapUpdate previousMap;
    MapUpdate currentMap;
    std::list<GPixmapItem*> players; //tu uchivavam pointre na obrazky hracov
    bool inGame = false; // indikuje ci je prave hrac v hre
    QParallelAnimationGroup animacie;
};

#endif // GAME_H
