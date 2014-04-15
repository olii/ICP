#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GView.h"
#include <QString>
#include <QGraphicsEllipseItem>
#include <QtDebug>
#include <QSize>
#include <QTimer>
#include <QResizeEvent>
#include <QTimeLine>
#include <QGraphicsItemAnimation>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    ui->graphicsView->setScene(scene);
    timer = new QTimer( this );
    connect(timer, SIGNAL(timeout()), this, SLOT(gameloop()));
    timer->setInterval(0);
    ui->statusBar->showMessage("PLACEHOLDER");
    setWindowTitle( "ICP" );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}

void MainWindow::on_pushButton_clicked()
{

    setWindowTitle( ui->lineEdit->text() );
}

void MainWindow::on_pushButton_2_clicked()
{
    //elipse->setToolTip(QString("<em>sasdasdasdasdas</em><br /><h1>Nadpis</h1>"));
    /*QSize rect = ui->graphicsView->size();
    QSize size (rect.width()/ui->lineEdit->text().toInt(), rect.height()/ui->lineEdit_2->text().toInt() );
    QPixmap pixmap = orig.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    scene->clear();
    for( int i = 0; i< ui->lineEdit->text().toInt(); ++i )
    {
        for ( int j = 0; j < ui->lineEdit_2->text().toInt(); j++ )
        {
            QGraphicsPixmapItem *tmp = scene->addPixmap(pixmap);
            tmp->setPos(size.width()*i, size.height()*j);
        }
    }*/
    ui->graphicsView->scene()->clear();
    ui->graphicsView->dimension = QSize( ui->lineEdit->text().toInt(), ui->lineEdit_2->text().toInt() );
    ui->graphicsView->resize_textures();
    ui->graphicsView->init();

}

void MainWindow::gameloop()
{
    QList<QGraphicsItem *> list = scene->items();

    QList<QGraphicsItem *>::iterator i;
    for (i = list.begin(); i != list.end(); ++i)
    {
        (*i)->setPos((*i)->pos().x()+1, (*i)->pos().y()+1);
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    /*if ( timer->isActive() )
    {
        timer->stop();
    }
    else
    {
        timer->start();
    }*/
}


void MainWindow::resizeEvent(QResizeEvent* event)
{
   Q_UNUSED(event);
   QString text ;
   text = "Size of draw area ";
   text += QString::number(ui->graphicsView->size().width());
   text += "x";
   text += QString::number(ui->graphicsView->size().height());
   ui->statusBar->showMessage(text);
}

void MainWindow::on_radioButton_podlaha_clicked()
{
    ui->graphicsView->active = 0;
}

void MainWindow::on_radioButton_stena_clicked()
{
    ui->graphicsView->active = 1;
}

void MainWindow::on_radioButton_clicked()
{
    ui->graphicsView->active = 2;
}
