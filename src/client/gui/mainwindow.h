/**
 * @file mainwindow.h
 *
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "informator.h"

namespace Ui {
class MainWindow;
}
/**
 * @brief The MainWindow class
 *        trieda predstavujuca hlavne okno, ktore sa zobrazi po spusteni aplikacie
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Informator * informator;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
