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
#include <QHeaderView>
#include "map.h"
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), timer(this)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    ui->graphicsView->setScene(scene);
    connect(&timer, SIGNAL(timeout()), this, SLOT(gameloop()));
    timer.setInterval(0);
    ui->statusBar->showMessage("PLACEHOLDER");
    setWindowTitle( "ICP" );


    ui->tableWidget->setRowCount(2);
    ui->tableWidget->setColumnCount(4);

    QStringList m_TableHeader;
    m_TableHeader<<"#"<<"Name"<<"Players" << "Map";
    ui->tableWidget->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("1"));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem("Dummy name"));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem("2/4"));
    ui->tableWidget->setItem(0, 3, new QTableWidgetItem("Big map"));
    ui->tableWidget->setItem(1, 0, new QTableWidgetItem("2"));
    ui->tableWidget->setItem(1, 1, new QTableWidgetItem("Dummy name2"));
    ui->tableWidget->setItem(1, 2, new QTableWidgetItem("4/4"));
    ui->tableWidget->setItem(1, 3, new QTableWidgetItem("Big map2"));

    connect( ui->tableWidget, SIGNAL( cellDoubleClicked ( int, int ) ),
                 this, SLOT( cellSelected( int, int ) ) );
}

void MainWindow::cellSelected(int nRow, int nCol)
{
    qDebug() << "Cell at row " << QString::number(nRow) <<" column " << QString::number(nCol)
             << " was double clicked. ID:";

    QTableWidgetItem * tmp = ui->tableWidget->item(nRow, 0);
    qDebug() << tmp->text();
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete timer;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
             tr("Open Map"), "",
             tr("ICP Map (*.imp);;All Files (*)"));
    if (fileName.isEmpty())
             return;

    setWindowTitle( fileName );

    StaticMap t;

    std::ifstream ifs(fileName.toStdString());
    boost::archive::text_iarchive ia(ifs);
    ia >> t;
    ui->graphicsView->load(t);

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
    ui->graphicsView->init();

}

void MainWindow::gameloop()
{
    /*QList<QGraphicsItem *> list = scene->items();

    QList<QGraphicsItem *>::iterator i;
    for (i = list.begin(); i != list.end(); ++i)
    {
        (*i)->setPos((*i)->pos().x()+1, (*i)->pos().y()+1);
    }*/
    ui->graphicsView->translate(1.0, 2.0);
}

void MainWindow::on_pushButton_3_clicked()
{

    QString fileName = QFileDialog::getSaveFileName(this,
             tr("Save Map"), "",
             tr("ICP Map (*.imp);;All Files (*)"));
    if (fileName.isEmpty())
             return;

    StaticMap t(ui->graphicsView->dimension.height(), ui->graphicsView->dimension.width());

    QList<QGraphicsItem *> l = ui->graphicsView->scene()->items();
    for( auto x: l)
    {
        GPixmapItem *item =  qgraphicsitem_cast<GPixmapItem*>(x);
        if ( item == 0 )
            return;
        t.items[item->y()/512][item->x()/512] = static_cast<StaticMap::StaticTypes>(item->typ);
    }


    std::ofstream ofs(fileName.toStdString());
    boost::archive::text_oarchive oa(ofs);
    oa << t;

    /*if ( timer.isActive() )
    {
        timer.stop();
    }
    else
    {
        timer.start();
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

    qreal matrix_ratio;
    qreal vertical_scrolbar_ratio;
    qreal horizontal_scrolbar_ratio;

    matrix_ratio = ui->graphicsView->matrix().m11() / ui->graphicsView->getscale();

    horizontal_scrolbar_ratio = ui->graphicsView->horizontalScrollBar()->maximum() ? \
        static_cast<qreal> (ui->graphicsView->horizontalScrollBar()->value()) /\
        static_cast<qreal> (ui->graphicsView->horizontalScrollBar()->maximum()) : 0.0;

    vertical_scrolbar_ratio = ui->graphicsView->verticalScrollBar()->maximum() ? \
        static_cast<qreal> (ui->graphicsView->verticalScrollBar()->value()) /\
        static_cast<qreal> (ui->graphicsView->verticalScrollBar()->maximum()) : 0.0;

    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->savescale();

    ui->graphicsView->scale(matrix_ratio, matrix_ratio);
    ui->graphicsView->horizontalScrollBar()->setValue(\
        static_cast<int> (ui->graphicsView->horizontalScrollBar()->maximum() * horizontal_scrolbar_ratio + 0.5));
    ui->graphicsView->verticalScrollBar()->setValue(\
        static_cast<int> (ui->graphicsView->verticalScrollBar()->maximum() * vertical_scrolbar_ratio + 0.5));
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

void MainWindow::on_radioButton_2_clicked()
{
    ui->graphicsView->active = 3;
}

void MainWindow::on_radioButton_3_clicked()
{
    ui->graphicsView->active = 4;
}

void MainWindow::on_radioButton_4_clicked()
{
    ui->graphicsView->active = 5;
}

void MainWindow::on_radioButton_5_clicked()
{
    ui->graphicsView->active = 6;
}


void MainWindow::on_animator_clicked()
{
    QPixmap p("://resources/1_2.png");

    GPixmapItem *n = new GPixmapItem(p);
    ui->graphicsView->scene()->addItem(n);
    n->setToolTip("<h1>PLAYER 23</h1><p>step count: 1500</p>");

    n->setPos(+1024, +1024);


    //static QGraphicsItem *tmp = ui->graphicsView->scene()->itemAt(513, 1, QTransform());
    //GPixmapItem *ptr = qgraphicsitem_cast<GPixmapItem *>(tmp);
    GPixmapItem *ptr = n;

    QPropertyAnimation* anim1 = new QPropertyAnimation(ptr, "pos");
    anim1->setDuration(1000);
    anim1->setStartValue(ptr->pos());
    anim1->setEndValue(ptr->pos()+QPointF(-256, 0));
    anim1->setEasingCurve(QEasingCurve::Linear);

    QPropertyAnimation* anim2 = new QPropertyAnimation(ptr, "zindex");
    anim2->setDuration(1000);
    anim2->setStartValue(ptr->getSize());
    anim2->setEndValue(ptr->getSize()*(6.0/5.0));
    anim2->setEasingCurve(QEasingCurve::Linear);

    QParallelAnimationGroup *group1 = new QParallelAnimationGroup;
    group1->addAnimation(anim1);
    group1->addAnimation(anim2);


    anim1 = new QPropertyAnimation(ptr, "pos");
    anim1->setDuration(1000);
    anim1->setStartValue(ptr->pos()+QPointF(-256, 0));
    anim1->setEndValue(ptr->pos()+QPointF(-512, 0));
    anim1->setEasingCurve(QEasingCurve::Linear);

    anim2 = new QPropertyAnimation(ptr, "zindex");
    anim2->setDuration(1000);
    anim2->setStartValue(ptr->getSize()*(6.0/5.0));
    anim2->setEndValue(ptr->getSize());
    anim2->setEasingCurve(QEasingCurve::Linear);

    QParallelAnimationGroup *group2 = new QParallelAnimationGroup;
    group2->addAnimation(anim1);
    group2->addAnimation(anim2);


    QSequentialAnimationGroup *seq = new QSequentialAnimationGroup;
    seq->addAnimation(group1);
    seq->addAnimation(group2);
    seq->start(QAbstractAnimation::DeleteWhenStopped);

    //seq->setCurrentTime(seq->totalDuration());





}
