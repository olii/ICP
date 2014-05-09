/**
 * @file game.cpp
 *
 * @brief vedlajsi modul pre implementaciu grafickeho rozhrania aplikacie
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "game.h"
#include "ui_game.h"
#include "../../shared/map.h"
#include <QDebug>
#include <QSize>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QHeaderView>


Game::Game(QWidget *parent, int StackedWidget) :
    QWidget(parent),
    scene(this),
    ui(new Ui::Game),
    gametimer(this),
    animacie(this)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    qDebug() << ui->stackedWidget->currentIndex();

    ui->graphicsView->setScene(&scene);
    ui->graphicsView_2->setScene(&scene);
    ui->graphicsView_3->setScene(&scene);

    QObject::connect(&gametimer, SIGNAL(timeout()), this, SLOT(gameLoop()));

    Informator * informator;
    informator = Informator::getInstance();

    ServerInfoList list = informator->GetConnection()->GetMapList();

    /* nastavi sa spravny Stacked Widget */
    if (StackedWidget == 0)
    {
        ui->stackedWidget->setCurrentIndex(0);
        for( auto x: list.list )
        {
            ui->comboBox->addItem(QString::fromStdString(x.map));
        }
        /* vykreslenie nahladu */
        ui->comboBox->setCurrentIndex(0);
        //LoadMap(ui->comboBox->itemText(0).toStdString());

        QStringList num_of_players; //rozne pocty hracov na vyber pri tvorbe hry
             num_of_players << "1" << "2" << "3" << "4";
        ui->comboBox_2->addItems(num_of_players);
    }
    else
    {
        ServerInfoList list = informator->GetConnection()->GetServerList();

        ui->tableWidget_2->setRowCount(list.list.size());
        ui->tableWidget_2->setColumnCount(6);
        QStringList m_TableHeader;
        m_TableHeader<<"ID"<<"Name"<<"Players" << "Tick" << "Timeout" <<  "Map";
        ui->tableWidget_2->setHorizontalHeaderLabels(m_TableHeader);
        ui->tableWidget_2->verticalHeader()->setVisible(false);
        ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        //ui->tableWidget_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidget_2->setAlternatingRowColors(true);
        ui->tableWidget_2->setCornerButtonEnabled(false);
        ui->tableWidget_2->setFrameStyle(QFrame::NoFrame);
        ui->tableWidget_2->setColumnWidth(0,70);


        int i = 0;
        for( auto x: list.list )
        {
            ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(QString::number(x.id)));
            ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(x.name)));
            ui->tableWidget_2->setItem(i, 2, new QTableWidgetItem(tr("%1/%2").arg(x.playing).arg(x.max)));
            ui->tableWidget_2->setItem(i, 3, new QTableWidgetItem(QString::number(x.timer)));
            ui->tableWidget_2->setItem(i, 4, new QTableWidgetItem(QString::number(x.timeout)));
            ui->tableWidget_2->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(x.map)));
            i++;
        }
        /*ui->tableWidget_2->setItem(0, 0, new QTableWidgetItem("1"));
        ui->tableWidget_2->setItem(0, 1, new QTableWidgetItem("Dummy name"));
        ui->tableWidget_2->setItem(0, 2, new QTableWidgetItem("2/4"));
        ui->tableWidget_2->setItem(0, 3, new QTableWidgetItem("Big map"));
        ui->tableWidget_2->setItem(1, 0, new QTableWidgetItem("2"));
        ui->tableWidget_2->setItem(1, 1, new QTableWidgetItem("Dummy name2"));
        ui->tableWidget_2->setItem(1, 2, new QTableWidgetItem("4/4"));
        ui->tableWidget_2->setItem(1, 3, new QTableWidgetItem("Big map2"));*/

        ui->stackedWidget->setCurrentIndex(1);
    }
}

Game::~Game()
{
    qDebug() << "destructor game";
    gametimer.stop();
    animacie.clear();

    Informator::getInstance()->GetConnection()->Leave();
    if( Informator::getInstance()->GetConnection()->Ready() )
    {
        Informator::getInstance()->GetConnection()->ReadPacket();
    }

    //Informator::getInstance()->Disconnect();
    delete ui;
}

/**
 * Slider pre oneskorenie
 * nastavuje sa hodnota oneskorenia dalsieho kroku hry
 */
void Game::on_horizontalSlider_valueChanged(int value)
{
    ui->doubleSpinBox->setValue(value / 10.0);
}

/**
 * Slider pre timeout
 * nastavuje sa hodnota cakania na zaciatok hry v sekundach
 */
void Game::on_horizontalSlider_2_valueChanged(int value)
{
    ui->spinBox->setValue(value);
}

/**
 * Tlacidlo back z obrazovky NewGame
 * navrat na predosle menu
 */
void Game::on_pushButton_2_clicked()
{
    scene.clear();
    close();
}

/**
 * Combobox
 * zmena hodnoty
 */
/*void Game::on_comboBox_activated(const QString &arg1)
{
    qDebug() << arg1;

    LoadMap(arg1.toStdString());

}*/

/**
 * Combobox
 * zmena indexu
 */
/*void Game::on_comboBox_activated(int index)
{
    qDebug() << index;
}*/

/**
 * Tlacidlo back z obrazovky JoinGame
 * navrat na predosle menu
 */
void Game::on_pushButton_4_clicked()
{
    scene.clear();
    close();
}

/**
 * Tlacidlo Play z obrazovky JoinGame
 * vstup do hry
 */
void Game::on_pushButton_3_clicked()
{
    scene.clear();
    //QModelIndexList selectedList = ui->tableWidget_2->selectionModel()->selectedRows();
    if (ui->tableWidget_2->selectedItems().size() > 0)
    {
        int row = ui->tableWidget_2->currentRow();
        //qDebug() << row << ui->tableWidget_2->item(row, 0)->text().toInt() << ui->tableWidget_2->selectedItems().size();

        Informator::getInstance()->Join( ui->tableWidget_2->item(row, 0)->text().toInt() );
        inGame = true;
        ui->stackedWidget->setCurrentIndex(2);
        AfterJoinTask();
    }
}

/**
 * Tlacidlo Create Game z obrazovky NewGame
 * vstup do hry
 */
void Game::on_pushButton_clicked()
{
    scene.clear();
    ui->stackedWidget->setCurrentIndex(2);
}

/**
 * Tlacidlo Leave
 * ukoncenie a opustenie aktualnej hry
 */
void Game::on_pushButton_5_clicked()
{
    gametimer.stop();
    animacie.clear();
    Informator::getInstance()->GetConnection()->Leave();
    if( Informator::getInstance()->GetConnection()->Ready() )
    {
        Informator::getInstance()->GetConnection()->ReadPacket();
    }
    inGame = false;
    scene.clear();
    players.clear();
    close();
}

void Game::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    /* TODO nastavovat len potrebne */
        ui->graphicsView->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
        ui->graphicsView_2->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
        ui->graphicsView_3->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void Game::LoadMap(std::string name, GView * view)
{
    Map map = Informator::getInstance()->GetConnection()->GetMapByName(name);
    Map::MapMatrix screenbuffer = map.items;

    QPixmap floor("://resources/grass.jpg");
    QPixmap wall("://resources/brixbug.png");
    QPixmap gate("://resources/grass.jpg");
    QPixmap key("://resources/grass.jpg");
    QPixmap guard_spawn("://resources/grass.jpg");
    QPixmap player_spawn("://resources/grass.jpg");
    QPixmap finish("://resources/grass.jpg");

    scene.clear();

    qDebug() << "rows = "<< screenbuffer.size();
    qDebug() << "columns = " << screenbuffer[0].size();

    for (unsigned i = 0; i < screenbuffer.size(); i++)
    {
        for (unsigned j = 0; j < screenbuffer[i].size(); j++)
        {
            GPixmapItem* tmp;
            switch (screenbuffer[i][j])
            {
                case Map::GRASS:
                    tmp = new GPixmapItem(floor);
                    break;
                case Map::WALL:
                    tmp = new GPixmapItem(wall);
                    break;
                case Map::GATE:
                    tmp = new GPixmapItem(gate);
                    break;
                case Map::KEY:
                    tmp = new GPixmapItem(key);
                    break;
                case Map::GUARD_SPAWN:
                    tmp = new GPixmapItem(guard_spawn);
                    break;
                case Map::PLAYER_SPAWN:
                    tmp = new GPixmapItem(player_spawn);
                    break;
                case Map::FINISH:
                    tmp = new GPixmapItem(finish);
                    break;
                default:
                    tmp = new GPixmapItem(floor);
                    break;
            }
            tmp->setPos( TEXTURE_SIZE*j, TEXTURE_SIZE*i);
            view->scene()->addItem( tmp );
        }
    }

    view->scene()->setSceneRect(view->scene()->itemsBoundingRect());

    /* prisposobenie sceny rozmerom okna */
    view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void Game::on_comboBox_currentIndexChanged(const QString &arg1)
{
    LoadMap(arg1.toStdString(), ui->graphicsView);
}

/**
 * Zmena hodnoty spinBox polozky pre timeout
 * sucasne sa zmeni hodnota polozky Slider
 */
void Game::on_spinBox_valueChanged(int arg1)
{
    ui->horizontalSlider_2->setValue(arg1);
}

/**
 * Zmena hodnoty spinBox polozky pre delay
 * sucasne sa zmeni hodnota polozky Slider
 */
void Game::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->horizontalSlider->setValue(static_cast<int>(arg1*10 + 0.5));
}

void Game::on_tableWidget_2_clicked(const QModelIndex &index)
{
    LoadMap(ui->tableWidget_2->item(index.row(), 5)->text().toStdString(), ui->graphicsView_2);
}

void Game::on_pushButton_6_clicked()
{
    ServerInfoList list = Informator::getInstance()->GetConnection()->GetServerList();
    ui->tableWidget_2->clear();

    ui->tableWidget_2->setRowCount(list.list.size());
    QStringList m_TableHeader;
    m_TableHeader<<"ID"<<"Name"<<"Players" << "Tick" << "Timeout" <<  "Map";
    ui->tableWidget_2->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidget_2->verticalHeader()->setVisible(false);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    int i = 0;
    for( auto x: list.list )
    {
        ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(QString::number(x.id)));
        ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(x.name)));
        ui->tableWidget_2->setItem(i, 2, new QTableWidgetItem(tr("%1/%2").arg(x.playing).arg(x.max)));
        ui->tableWidget_2->setItem(i, 3, new QTableWidgetItem(QString::number(x.timer)));
        ui->tableWidget_2->setItem(i, 4, new QTableWidgetItem(QString::number(x.timeout)));
        ui->tableWidget_2->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(x.map)));
        i++;
    }
}

void Game::gameLoop()
{
    qDebug() << "timed event";
    Network *connection = Informator::getInstance()->GetConnection();

    if( connection->Ready() )
    {
        connection->ReadPacket();
        if( connection->GetHeaderType() == packetHeader::COMMAND )
        {
            Command c = connection->GetPacketContent<Command>();

            if( c.GetType() == Command::TEXT )
            {
                qDebug() << QString::fromStdString(c.GetText()); // niekde zobrazit text
            }
            else
            {
                qDebug() << "Received packet is not command::text";
            }
        }
        else if ( connection->GetHeaderType() == packetHeader::GAME_UPDATE  )
        {
            currentMap = connection->GetPacketContent<MapUpdate>();

            std::list<GPixmapItem*> pomoc;

            animacie.setCurrentTime(animacie.totalDuration()); // ak to nejake nestihli tak ich ukoncim
            animacie.clear(); // a vymazem

            QPixmap hrac("://resources/076814-antique-glowing-copper-orb-icon-business-key2.png");


            for ( auto &item: currentMap.players )
            {                
                auto it = std::find_if( players.begin(),players.end(),
                          [&]( GPixmapItem* ptr ){ return ptr->id == item.id ;} ); // najde id v zozname, aby som ho mohol naanimovat na novu poziciu

                if ( it ==  players.end() )
                {
                    //hraca som nenasiel v predoslom update == musel sa teraz pripojit, neanimujem
                    GPixmapItem* tmp;
                    tmp = new GPixmapItem(hrac);

                    tmp->x = item.x;
                    tmp->y = item.y;
                    tmp->id = item.id;

                    tmp->setPos( item.y*512, item.x*512 );
                    scene.addItem( tmp );

                    pomoc.push_back(tmp); // vlozim do zoznamu dokoncenych
                }
                else
                {
                    // hraca som nasiel, mozem ho naanimovat
                    // it = predosla pozicia
                    // item = nova pozicia

                    if ( (*it)->x == item.x && (*it)->y == item.y )
                    {
                        // pozicia hraca sa nezmeila - no job to do
                    }
                    else
                    {
                        //(*it)->x => item.x;
                        //(*it)->y => item.y;
                        //(*it)->setPos( item.y*512, item.x*512 );

                        // tu zacina animacia
                        /* najhorsia pasaz*/
                        QPointF start( (*it)->y*512, (*it)->x*512 );// zaciatok
                        QPointF end (item.y*512, item.x*512);       // koniec
                        QPointF midpoint = (start + end)/2;         // stred

                        QPropertyAnimation* anim1 = new QPropertyAnimation((*it), "pos");
                        anim1->setDuration(100);
                        anim1->setStartValue( start ); // povodne suradnice
                        anim1->setEndValue( midpoint );
                        anim1->setEasingCurve(QEasingCurve::Linear);


                        QPropertyAnimation* anim2 = new QPropertyAnimation((*it), "zindex");
                        anim2->setDuration(100);
                        anim2->setStartValue(512);
                        anim2->setEndValue(512*(6.0/5.0));
                        anim2->setEasingCurve(QEasingCurve::Linear);

                        QParallelAnimationGroup *hore = new QParallelAnimationGroup;
                        hore->addAnimation(anim1);
                        hore->addAnimation(anim2);

                        anim1 = new QPropertyAnimation((*it), "pos");
                        anim1->setDuration(100);
                        anim1->setStartValue(midpoint);
                        anim1->setEndValue(end);
                        anim1->setEasingCurve(QEasingCurve::Linear);

                        anim2 = new QPropertyAnimation((*it), "zindex");
                        anim2->setDuration(100);
                        anim2->setStartValue(512*(6.0/5.0));
                        anim2->setEndValue(512);
                        anim2->setEasingCurve(QEasingCurve::Linear);

                        QParallelAnimationGroup *dole = new QParallelAnimationGroup;
                        dole->addAnimation(anim1);
                        dole->addAnimation(anim2);

                        QSequentialAnimationGroup *hore_dole = new QSequentialAnimationGroup;
                        hore_dole->addAnimation(hore);
                        hore_dole->addAnimation(dole);

                        animacie.addAnimation(hore_dole);

                        (*it)->x = item.x; // nastavim nove pozicie do objektov
                        (*it)->y = item.y; // pouziju sa pri dalsom update

                    }
                    pomoc.push_back(*it); // vlozim do zoznamu dokoncenych
                    players.erase(it);
                }


            }
            /* postupne som si odoberal polozky z players, ak tam nejaka ostala tak je neplatna a treba ju vymazat*/
            for ( auto &item: players )
            {
                qDebug() << "odstranujem z pamate";
                scene.removeItem(item);
                delete item;
            }
            /* hotovo, teraz mozem vymenit pomoc za players */
            players.swap(pomoc);

             // a spustit animacie
             animacie.start();
        }
    }

}

void Game::AfterJoinTask()
{
    Network *connection = Informator::getInstance()->GetConnection();
    /* nacita sa staticka mapa */
    connection->ReadPacket();
    if (connection->GetHeaderType() != packetHeader::STATIC_MAP )
    {
        throw std::runtime_error( "Invalid packet received form server." );
    }
    Map map = connection->GetPacketContent<Map>(); // vlastne nacitanie packetu MAP, vsimni si template <Map>
    Map::MapMatrix background = map.items;

    /*odmazem nepotrebne veci, steny necham, ostatne je travicka*/
    for (auto &row: background)
    {
        for (auto &column: row)   // dolezite je ze to preberam ako referenciu auto&, ak len auto tak by som upravoval kopiu
        {
            switch(column)
            {
                case Map::GRASS:
                case Map::WALL:
                    break;
                default:
                    column = Map::GRASS;
                    break;
            }
        }
    }
    /* mozem vykreslit mapu*/
    QPixmap floor("://resources/ToyFactory_Multi-texturing_tile.png");
    QPixmap wall("://resources/krabica.jpg");
    for (unsigned i = 0; i < background.size(); i++)
    {
        for (unsigned j = 0; j < background[i].size(); j++)
        {
            GPixmapItem* tmp;
            switch (background[i][j])
            {
                case Map::GRASS:
                    tmp = new GPixmapItem(floor);
                    break;
                case Map::WALL:
                    tmp = new GPixmapItem(wall);
                    break;
                default:
                    tmp = new GPixmapItem(floor);
            }
            tmp->setPos( TEXTURE_SIZE*j, TEXTURE_SIZE*i);
            scene.addItem( tmp );
        }
    }
    scene.setSceneRect(scene.itemsBoundingRect());

    /*Hned nato pride prvy mapupdate*/

    currentMap = connection->GetMapUpdate();
    /* necham vykreslit hracov DEMO */
    players.clear();
    QPixmap hrac("://resources/076814-antique-glowing-copper-orb-icon-business-key2.png");
    for ( auto item: currentMap.players )
    {
        GPixmapItem* tmp;
        tmp = new GPixmapItem(hrac);

        tmp->x = item.x;
        tmp->y = item.y;
        tmp->id = item.id;

        players.push_back(tmp);

        tmp->setPos( TEXTURE_SIZE*item.y, TEXTURE_SIZE*item.x);
        scene.addItem( tmp );
    }

    gametimer.start(30); // kazdych 30 ms
}

void Game::closeEvent(QCloseEvent *event)
{
    gametimer.stop();
}
