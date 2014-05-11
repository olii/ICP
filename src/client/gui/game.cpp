/**
 * @file game.cpp
 *
 * @brief vedlajsi modul pre implementaciu grafickeho rozhrania aplikacie
 *        implementacia majoritnej casti logiky klienta
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "game.h"
#include "ui_game.h"
#include "modelmanager.h"

#include <QDebug>
#include <QSize>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QHeaderView>
#include <QScrollBar>
#include <QTime>
#include <QThread>

/**
 * Konstruktor noveho widgetu
 *
 * @param parent rodicovske okno
 * @param StackedWidget index pohladu, ktory sa ma zobrazit
 */
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

    counter = QTime::currentTime ();

    ui->graphicsView->setScene(&scene);
    ui->graphicsView_2->setScene(&scene);
    ui->graphicsView_3->setScene(&scene);

    ModelManager::GetInstance().RegisterModel(0, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(1, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(2, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(3, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(4, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(5, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(6, ModelManager::PlayerModel);
    ModelManager::GetInstance().RegisterModel(7, ModelManager::GuardModel);
    ModelManager::GetInstance().RegisterModel(8, ModelManager::GuardModel);
    ModelManager::GetInstance().RegisterModel(9, ModelManager::GuardModel);
    ModelManager::GetInstance().RegisterModel(10, ModelManager::GuardModel);
    ModelManager::GetInstance().RegisterModel(11, ModelManager::GuardModel);
    ModelManager::GetInstance().RegisterModel(12, ModelManager::GuardModel);
    ModelManager::GetInstance().RegisterModel(13, ModelManager::GuardModel);

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
        ui->comboBox_2->setCurrentIndex(3);
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

        ui->stackedWidget->setCurrentIndex(1);
    }
}

/**
 * Destruktor widgetu
 */
Game::~Game()
{
    qDebug() << "destructor game";
    gametimer.stop();
    animacie.clear();

    Informator::getInstance()->GetConnection()->Leave();
    for( int i = 0; i<20; ++i )
    {
        if( Informator::getInstance()->GetConnection()->Ready() )
        {
            Informator::getInstance()->GetConnection()->ReadPacket();
        }
        QThread::msleep(25);
    }

    //Informator::getInstance()->Disconnect();
    delete ui;
}

/**
 * @brief Game::GetTimer
 * @return hodnota delay pre danu hru
 */
int Game::GetTimer()
{
    return timer;
}
/**
 * @brief Game::SetTimer
 * @param miliseconds nastavi hodnotu delay danej hry v milisekundach
 */
void Game::SetTimer(int miliseconds)
{
    timer = miliseconds;
}

/**
 * @brief Game::resizeEvent
 *        vola spravnu obsluhu zmeny velkosti okna podla platna daneho okna, ktore je aktualne zobrazene
 * @param event
 */
void Game::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if (ui->stackedWidget->currentIndex() == 0)
        resize(ui->graphicsView);
    else if (ui->stackedWidget->currentIndex() == 1)
        resize(ui->graphicsView_2);
    else
        resize(ui->graphicsView_3);
}

/**
 * @brief Game::keyPressEvent
 *        ovladanie pomocou klavesovych skratiek
 * @param event
 */
void Game::keyPressEvent(QKeyEvent *event)
{
    if (ui->stackedWidget->currentIndex() == 2)
    {
        if( counter.addMSecs(50) >= QTime::currentTime() )
            return;
        counter = QTime::currentTime();

        Command c;
        Network *connection = Informator::getInstance()->GetConnection();

        switch(event->key())
        {
            case Qt::Key_W:
                c.SetType(Command::GO);
                connection->SendCommand(c);
                break;
            case Qt::Key_A:
                c.SetType(Command::LEFT);
                connection->SendCommand(c);
                break;
            case Qt::Key_S:
                c.SetType(Command::STOP);
                connection->SendCommand(c);
                break;
            case Qt::Key_D:
                c.SetType(Command::RIGHT);
                connection->SendCommand(c);
                break;
            case Qt::Key_Q:
                c.SetType(Command::OPEN);
                connection->SendCommand(c);
                break;
            case Qt::Key_E:
                c.SetType(Command::TAKE);
                connection->SendCommand(c);
                break;
            default:
                break;
        }
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

/**
 * @brief metoda pre prisposobenie zobrazenia obsahu sceny rozmerom grafickeho prvku pre jej vykreslenie
 * @param view pohlad pre ktory sa ma upravit scena
 */
void Game::resize(GView * view)
{
    qreal matrix_ratio;
    qreal vertical_scrolbar_ratio;
    qreal horizontal_scrolbar_ratio;

    matrix_ratio = view->matrix().m11() / view->getscale();
    /** vypocet konstant dolezitych pre zachovanie miesta zobrazenia po zmene rozmerov sceny */
    horizontal_scrolbar_ratio = view->horizontalScrollBar()->maximum() ? \
        static_cast<qreal> (view->horizontalScrollBar()->value()) /\
        static_cast<qreal> (view->horizontalScrollBar()->maximum()) : 0.0;

    vertical_scrolbar_ratio = view->verticalScrollBar()->maximum() ? \
        static_cast<qreal> (view->verticalScrollBar()->value()) /\
        static_cast<qreal> (view->verticalScrollBar()->maximum()) : 0.0;

    /** zakladne prisposobenie sceny rozmerom pohladu */
    view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
    view->savescale();

    /** obnovenie povodneho miesta na ktore bolo nastavene zobrazenie */
    view->scale(matrix_ratio, matrix_ratio);
    view->horizontalScrollBar()->setValue(\
        static_cast<int> (view->horizontalScrollBar()->maximum() * horizontal_scrolbar_ratio + 0.5));
    view->verticalScrollBar()->setValue(\
                static_cast<int> (view->verticalScrollBar()->maximum() * vertical_scrolbar_ratio + 0.5));
}

/**
 * @brief Game::setCenter
 *        metoda upravena pre volanie v animaciach, centruje pohlad na hraca
 * @param x
 */
void Game::setCenter(int x)
{
    fake_int = x;
    auto it = std::find_if( players.begin(),players.end(),
              [&]( GPixmapItem* ptr ){ return ptr->id == Informator::getInstance()->GetConnection()->GetId() ;} );
    //(if it.optinonflag = 1)
    if( it != players.end())
    {
        ui->graphicsView_3->centerOn((*it));
    }
}

/**
 * @brief Game::getCenter
 *        metoda nutna pre vytvorenie animacie centrovania pohladu na hraca
 */
int Game::getCenter()
{
    return fake_int;
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
        SetTimer(static_cast<int>(ui->tableWidget_2->item(row, 3)->text().toFloat() * 1000));
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
    //scene.clear();
    if (ui->lineEdit->text().isEmpty())
    {
        ShowError(QString("Create error"), QString("Unable to create game without name"), QMessageBox::Critical);
    }
    else
    {
        scene.clear();
        Informator::getInstance()->GetConnection()->CreateServer( ui->lineEdit->text().toStdString(),\
                                                 ui->comboBox_2->currentText().toInt(),\
                                                 ui->comboBox->currentText().toStdString(),\
                                                 static_cast<float> (ui->doubleSpinBox->value()),\
                                                 ui->spinBox->value());
        SetTimer(static_cast<int>(ui->doubleSpinBox->value() * 1000));
        inGame = true;
        ui->stackedWidget->setCurrentIndex(2);
        AfterJoinTask();
    }
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
    for( int i = 0; i<20; ++i )
    {
        if( Informator::getInstance()->GetConnection()->Ready() )
        {
            Informator::getInstance()->GetConnection()->ReadPacket();
        }
        QThread::msleep(25);
    }
    inGame = false;
    scene.clear();
    players.clear();
    close();
}

/**
  * @brief Game::LoadMap
  *        Funkcia pre nacitanie mapy a jej zobrazenie do sceny
  *
  * @param name nazov mapy
  * @param view ukazatel na pohlad
  */
void Game::LoadMap(std::string name, GView * view)
{
    Map map = Informator::getInstance()->GetConnection()->GetMapByName(name);
    Map::MapMatrix screenbuffer = map.items;


    scene.clear();

    qDebug() << "rows = "<< screenbuffer.size();
    qDebug() << "columns = " << screenbuffer[0].size();

    for (unsigned i = 0; i < screenbuffer.size(); i++)
    {
        for (unsigned j = 0; j < screenbuffer[i].size(); j++)
        {
            GPixmapItem* tmp;
            tmp = new GPixmapItem(ModelManager::GetInstance().GetFloor());
            tmp->setPos( TEXTURE_SIZE*j, TEXTURE_SIZE*i );
            view->scene()->addItem( tmp );

            switch (screenbuffer[i][j])
            {
                case Map::WALL:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetWall());
                    break;
                case Map::GATE:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetDoorClosed());
                    break;
                case Map::KEY:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetKey());
                    break;
                case Map::FINISH:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetFinish());
                    break;
                default:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetFloor());
                    break;
            }
            tmp->setPos( TEXTURE_SIZE*j, TEXTURE_SIZE*i );
            view->scene()->addItem( tmp );
        }
    }

    view->scene()->setSceneRect(view->scene()->itemsBoundingRect());

    /* prisposobenie sceny rozmerom okna */
    view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);
    view->savescale();
}

/**
 * Zmena mapy pomocou Combo boxu - pohlad pre NewGame
 */
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

/**
 * Zmena mapy pomocou oznacenia riadku v tabulke - pohlad pre Join
 */
void Game::on_tableWidget_2_clicked(const QModelIndex &index)
{
    LoadMap(ui->tableWidget_2->item(index.row(), 5)->text().toStdString(), ui->graphicsView_2);
}

/**
 * Tlacidlo Refresh
 * znova sa nacitaju aktualne rozohrane hry a zobrazia sa informacie v tabulke
 */
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

/**
 * @brief Game::gameLoop
 *        Zakladny herny cyklus, ktory je volany pomocou casovaca a predstavuje zakladnu hernu logiku
 */
void Game::gameLoop()
{
    Network *connection = Informator::getInstance()->GetConnection();

    if( connection->Ready() )
    {
        connection->ReadPacket();
        if( connection->GetHeaderType() == packetHeader::COMMAND )
        {
            Command c = connection->GetPacketContent<Command>();

            if( c.GetType() == Command::TEXT )
            {
                ui->textBrowser->append(QString::fromStdString(c.GetText())); // niekde zobrazit text
            }
        }
        else if ( connection->GetHeaderType() == packetHeader::GAME_UPDATE  )
        {


            currentMap = connection->GetPacketContent<MapUpdate>();

            std::list<GPixmapItem*> pomoc;

            bool positions_changed = false;

            for ( auto &item: keys )
            {
                scene.removeItem(item);
                delete item;
            }

            for ( auto &item: doors )
            {
                scene.removeItem(item);
                delete item;
            }

            doors.clear();
            keys.clear();

            /* vlozim do sceny vsetky dvere a pridam ich do vektora dveri */
            for (auto item: currentMap.gates)
            {
                GPixmapItem* tmp;
                if (item.optionFlag == true)
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetDoorClosed());
                else
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetDoorOpened());

                tmp->x = item.x;
                tmp->y = item.y;

                tmp->setPos( item.y*512, item.x*512 );
                scene.addItem( tmp );

                doors.push_back(tmp); // vlozim do zoznamu strazcov
            }

            /* vlozim do sceny vsetkych strazcov a pridam ich do vektora strazcov */
            for (auto item: currentMap.keys)
            {
                GPixmapItem* tmp;
                tmp = new GPixmapItem(ModelManager::GetInstance().GetKey());

                tmp->x = item.x;
                tmp->y = item.y;

                tmp->setPos( item.y*512, item.x*512 );
                scene.addItem( tmp );

                keys.push_back(tmp); // vlozim do zoznamu strazcov
            }

            for ( auto &item: currentMap.players )
            {                
                auto it = std::find_if( players.begin(),players.end(),
                          [&]( GPixmapItem* ptr ){ return ptr->id == item.id ;} ); // najde id v zozname, aby som ho mohol naanimovat na novu poziciu

                if ( it ==  players.end() )
                {
                    //hraca som nenasiel v predoslom update == musel sa teraz pripojit, neanimujem
                    GPixmapItem* tmp;
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetModel(item.model, item.dir, ModelManager::PlayerModel));

                    tmp->x = item.x;
                    tmp->y = item.y;
                    tmp->id = item.id;

                    tmp->setPos( item.y*512, item.x*512 );
                    tmp->setToolTip(tr("<h2>Player %1</h2><p>Steps: <b>%2</b><br />Keys: <b>%3</b><br />In-Game Time: %4</p>")
                                    .arg(item.id).arg(item.steps).arg(item.keyCount).arg(QString::fromStdString(item.stringField)));
                    scene.addItem( tmp );
                    tmp->setVisible(item.optionFlag);

                    pomoc.push_back(tmp); // vlozim do zoznamu dokoncenych

                }
                else
                {
                    // hraca som nasiel, mozem ho naanimovat
                    // it = predosla pozicia
                    // item = nova pozicia
                    (*it)->setVisible(item.optionFlag);
                    (*it)->setPixmap(ModelManager::GetInstance().GetModel(item.model, item.dir, ModelManager::PlayerModel));
                    (*it)->setToolTip(tr("<h2>Player %1</h2><p>Steps: <b>%2</b><br />Keys: <b>%3</b><br />In-Game Time: %4</p>").
                                      arg(item.id).arg(item.steps).arg(item.keyCount).arg(QString::fromStdString(item.stringField)));
                    if ( (*it)->x == item.x && (*it)->y == item.y )
                    {
                        // pozicia hraca sa nezmeila - no job to do

                    }
                    else
                    {
                        if (!positions_changed)
                        {
                            animacie.setCurrentTime(animacie.totalDuration()); // ak to nejake nestihli tak ich ukoncim
                            animacie.clear(); // a vymazem
                            positions_changed = true;
                        }
                        animate((*it), item);
                        /* pri centrovani na hraca je potrebne animovat pohlad */
                        if ((*it)->id == connection->GetId() && ui->checkBox->isChecked())
                        {
                            QPropertyAnimation* anim1 = new QPropertyAnimation(this, "center");
                            anim1->setDuration(GetTimer());
                            anim1->setStartValue(0);
                            anim1->setEndValue(10);
                            anim1->setEasingCurve(QEasingCurve::Linear);
                            animacie.addAnimation(anim1);
                        }

                        (*it)->x = item.x; // nastavim nove pozicie do objektov
                        (*it)->y = item.y; // pouziju sa pri dalsom update

                    }

                    /* ak som zomrel tak vypnem prepinace */
                    if (!item.optionFlag && (*it)->id == connection->GetId())
                    {
                        ui->checkBox->setCheckState(Qt::Unchecked);
                        ui->checkBox->setCheckable(false);
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

            /* vykreslim strazcov */
            for ( auto &item: currentMap.guards )
            {
                auto it = std::find_if( guards.begin(),guards.end(),
                          [&]( GPixmapItem* ptr ){ return ptr->id == item.id ;} ); // najde id v zozname, aby som ho mohol naanimovat na novu poziciu

                (*it)->setPixmap(ModelManager::GetInstance().GetModel(item.model, item.dir, ModelManager::GuardModel));

                if ( (*it)->x == item.x && (*it)->y == item.y )
                {
                    // pozicia strazcu sa nezmeila - no job to do

                }
                else
                {
                    if (!positions_changed)
                    {
                        animacie.setCurrentTime(animacie.totalDuration()); // ak to nejake nestihli tak ich ukoncim
                        animacie.clear(); // a vymazem
                        positions_changed = true;
                    }
                    animate((*it), item);

                    (*it)->x = item.x; // nastavim nove pozicie do objektov
                    (*it)->y = item.y; // pouziju sa pri dalsom update

                }
            }
            ui->label_5->setText(QString::fromStdString(currentMap.treasure.stringField));

            // a spustit animacie
            /*center on*/

            if (positions_changed)
                animacie.start();

        }
    }

}

/**
 * @brief Game::animate
 * @param old_item
 * @param new_item
 */
void Game::animate(GPixmapItem * & old_item, MapItemsInfo & new_item)
{
    QPointF start( old_item->y*512, old_item->x*512 );// zaciatok
    QPointF end (new_item.y*512, new_item.x*512);       // koniec
    QPointF midpoint = (start + end)/2;         // stred

    QPropertyAnimation* anim1 = new QPropertyAnimation(old_item, "pos");
    anim1->setDuration(GetTimer()/2);
    anim1->setStartValue( start ); // povodne suradnice
    anim1->setEndValue( midpoint );
    anim1->setEasingCurve(QEasingCurve::Linear);


    QPropertyAnimation* anim2 = new QPropertyAnimation(old_item, "zindex");
    anim2->setDuration(GetTimer()/2);
    anim2->setStartValue(512);
    anim2->setEndValue(512*(6.0/5.0));
    anim2->setEasingCurve(QEasingCurve::Linear);

    QParallelAnimationGroup *hore = new QParallelAnimationGroup;
    hore->addAnimation(anim1);
    hore->addAnimation(anim2);

    anim1 = new QPropertyAnimation(old_item, "pos");
    anim1->setDuration(GetTimer()/2);
    anim1->setStartValue(midpoint);
    anim1->setEndValue(end);
    anim1->setEasingCurve(QEasingCurve::Linear);

    anim2 = new QPropertyAnimation(old_item, "zindex");
    anim2->setDuration(GetTimer()/2);
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
}

/**
 * @brief Game::AfterJoinTask
 *        metoda, ktora pripoji hraca do hry, a vytvori zakladne zobrazenie mapy
 */
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
    for (unsigned i = 0; i < background.size(); i++)
    {
        for (unsigned j = 0; j < background[i].size(); j++)
        {
            GPixmapItem* tmp;
            switch (background[i][j])
            {
                case Map::GRASS:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetFloor());
                    break;
                case Map::WALL:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetWall());
                    break;
                default:
                    tmp = new GPixmapItem(ModelManager::GetInstance().GetFloor());
            }
            tmp->setPos( TEXTURE_SIZE*j, TEXTURE_SIZE*i);
            scene.addItem( tmp );
        }
    }
    scene.setSceneRect(scene.itemsBoundingRect());

    /*Hned nato pride prvy mapupdate*/
    currentMap = connection->GetMapUpdate();

    /* na zaciatok vykreslim ciel */
    GPixmapItem* tmp;
    tmp = new GPixmapItem(ModelManager::GetInstance().GetFinish());

    tmp->x = currentMap.treasure.x;
    tmp->y = currentMap.treasure.y;

    tmp->setPos( currentMap.treasure.y*512, currentMap.treasure.x*512 );
    scene.addItem( tmp );

    doors.clear();
    keys.clear();
    guards.clear();

    /* vlozim do sceny vsetky dvere a pridam ich do vektora dveri */
    for (auto item: currentMap.gates)
    {
        GPixmapItem* tmp;
        if (item.optionFlag == true)
            tmp = new GPixmapItem(ModelManager::GetInstance().GetDoorClosed());
        else
            tmp = new GPixmapItem(ModelManager::GetInstance().GetDoorOpened());

        tmp->x = item.x;
        tmp->y = item.y;

        tmp->setPos( item.y*512, item.x*512 );
        scene.addItem( tmp );

        doors.push_back(tmp); // vlozim do zoznamu strazcov
    }

    /* vlozim do sceny vsetkych strazcov a pridam ich do vektora strazcov */
    for (auto item: currentMap.keys)
    {
        GPixmapItem* tmp;
        tmp = new GPixmapItem(ModelManager::GetInstance().GetKey());

        tmp->x = item.x;
        tmp->y = item.y;

        tmp->setPos( item.y*512, item.x*512 );
        scene.addItem( tmp );

        keys.push_back(tmp); // vlozim do zoznamu strazcov
    }

    /* vlozim do sceny vsetkych strazcov a pridam ich do vektora strazcov */
    for (auto &item: currentMap.guards)
    {
        GPixmapItem* tmp;
        tmp = new GPixmapItem(ModelManager::GetInstance().GetModel(item.model, item.dir, ModelManager::GuardModel));

        tmp->x = item.x;
        tmp->y = item.y;
        tmp->id = item.id;

        tmp->setPos( item.y*512, item.x*512 );
        scene.addItem( tmp );

        guards.push_back(tmp); // vlozim do zoznamu strazcov
    }

    ui->graphicsView_3->fitInView(ui->graphicsView_3->scene()->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView_3->savescale();

    gametimer.start(10); // kazdych 10 ms
}

void Game::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    gametimer.stop();
}

/**
 * @brief Game::on_lineEdit_2_editingFinished
 *        zadavanie textovych prikazov pre ovladanie hracov
 */
void Game::on_lineEdit_2_editingFinished()
{
    Command c;
    Network *connection = Informator::getInstance()->GetConnection();

    if (ui->lineEdit_2->text() == "go") {
        ui->textBrowser->append(ui->lineEdit_2->text());
        c.SetType(Command::GO);
        connection->SendCommand(c);
    } else if (ui->lineEdit_2->text() == "stop") {
        ui->textBrowser->append(ui->lineEdit_2->text());
        c.SetType(Command::STOP);
        connection->SendCommand(c);
    } else if (ui->lineEdit_2->text() == "left") {
        ui->textBrowser->append(ui->lineEdit_2->text());
        c.SetType(Command::LEFT);
        connection->SendCommand(c);
    } else if (ui->lineEdit_2->text() == "right") {
        ui->textBrowser->append(ui->lineEdit_2->text());
        c.SetType(Command::RIGHT);
        connection->SendCommand(c);
    } else if (ui->lineEdit_2->text() == "take") {
        ui->textBrowser->append(ui->lineEdit_2->text());
        c.SetType(Command::TAKE);
        connection->SendCommand(c);
    } else if (ui->lineEdit_2->text() == "open") {
        ui->textBrowser->append(ui->lineEdit_2->text());
        c.SetType(Command::OPEN);
        connection->SendCommand(c);
    } else {
        ui->textBrowser->append("Unknown command");
    }
    ui->lineEdit_2->setText("");
}

/**
 * @brief Game::on_checkBox_toggled
 *        povolenie / zakaz posuvania pohladu pomocou mysi na zaklade toho ci je zapnute centrovanie pohladu na hraca
 * @param checked
 */
void Game::on_checkBox_toggled(bool checked)
{
    if (checked)
    {
        setCenter(0);
        ui->graphicsView_3->setDragMode(QGraphicsView::NoDrag);
    }
    else
    {
        ui->graphicsView_3->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

/**
 * @brief Game::on_pushButton_7_clicked
 *        nahodny vyber hry do ktorej sa hrac pripoji
 */
void Game::on_pushButton_7_clicked()
{
    ServerInfoList list = Informator::getInstance()->GetConnection()->GetServerList();

    scene.clear();

    if (list.list.size() > 0)
    {
        int index = qrand() % list.list.size();
        Informator::getInstance()->Join( list.list[index].id );
        SetTimer(static_cast<int>(list.list[index].timer * 1000));
        inGame = true;
        ui->stackedWidget->setCurrentIndex(2);
        AfterJoinTask();
    }
    else
    {
        ShowError(QString("Unable to connect"), QString("There is no game to connect"), QMessageBox::Information);
    }
}
