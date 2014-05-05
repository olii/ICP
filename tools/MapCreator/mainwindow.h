#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QVector>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void resizeEvent(QResizeEvent* event);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void gameloop();
    void on_pushButton_3_clicked();
    void on_radioButton_podlaha_clicked();

    void on_radioButton_stena_clicked();

    void on_radioButton_clicked();

    void cellSelected(int nRow, int nCol);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QTimer timer;


};

#endif // MAINWINDOW_H
