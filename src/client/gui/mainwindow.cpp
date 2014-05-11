/**
 * @file mainwindow.cpp
 *
 * @brief vedlajsi modul pre implementaciu grafickeho rozhrania aplikacie
 *        implementacia klikatelnej casti pripajania sa na server
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game.h"
#include "../network.h"
#include "../../shared/command.h"
#include "../../shared/global.h"
#include "../../shared/map.h"
#include "../../shared/serverinfo.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    //qDebug() << ui->stackedWidget->currentIndex();
}

MainWindow::~MainWindow()
{
    qDebug() << "destructor";
    delete ui;
}

/**
 * Tlacidlo connect
 * pokus o pripojenie na server
 */
void MainWindow::on_pushButton_clicked()
{
    try
    {   //objekt uchovava vsetky potrebne informacie o hre aj pre dalsie okna a moduly
        informator = Informator::getInstance();
        informator->Connect(ui->lineEdit->text().toStdString() , ui->lineEdit_2->text().toStdString());
        //informator->Join(5);

        qDebug() << informator->getClientId() << "\n";

        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() + 1);
    }
    catch (std::exception& e)
    {
        ShowError(QString("Connection error"), QString(e.what()), QMessageBox::Critical);
    }
}

/**
 * Tlacidlo exit
 * koniec hry
 */
void MainWindow::on_pushButton_2_clicked()
{
    close();
    return;
}

/**
 * Tlacidlo back
 * vratenie sa do rezimu "Pripojenie na server"
 */
void MainWindow::on_pushButton_5_clicked()
{
    //informator->connection->Shutdown();
    informator->Disconnect();
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
}

/**
 * Tlacidlo New Game
 * vytvorenie noveho okna, ktore bude obsahovat obrazovku pre vytvorenie novej hry
 */
void MainWindow::on_pushButton_3_clicked()
{
    Game * new_game = new Game();
    //hide();
    new_game->setWindowModality(Qt::ApplicationModal);
    new_game->show();
}

/**
 * Tlacidlo Join Game
 * vytvorenie noveho okna, ktore bude obsahovat obrazovku pre pripojenie sa do existujucej hry
 */
void MainWindow::on_pushButton_4_clicked()
{
    Game * new_game = new Game(nullptr, 1);
    //hide();
    //LoadMap(, ui->graphicsView);  //nacita sa prvy nahlad mapy
    new_game->setWindowModality(Qt::ApplicationModal);
    new_game->show();
}

/**
 * @brief MainWindow::on_pushButton_6_clicked
 *        po kliknuti na dane tlacidlo sa zobrazia strucne informacie o hre
 */
void MainWindow::on_pushButton_6_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Informacie o projekte");
    msgBox.setWindowTitle("Projekt ICP 2013/2014");
    msgBox.setInformativeText("Autori: Oliver Nemcek\n\tStefan Marticek\n");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
