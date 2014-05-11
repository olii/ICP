/**
 * @file modelmanager.cpp
 *
 * @brief ulozenie textur do pamate a ich sprava, ktoru riadi objekt - navrhovy vzor singleton 
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "modelmanager.h"
/**
 * @brief ModelManager::ModelManager
 *        pri vytvarani objektu sa automaticky skonstruuju zakladne textury, ktorych vzhlad nie je nutne na zaklade logiky hry menit
 */
ModelManager::ModelManager() :
    floor(":/models/resources/podlaha.png"),
    wall(":/models/resources/stena.png"),
    door_closed(":/models/resources/gate.png"),
    door_opened(":/models/resources/door_opened.png"),
    key(":/models/resources/kluc.png"),
    finish(":/models/resources/treasure.png")
{

}

/**
 * @brief ModelManager::RegisterModel
 * @param id id registrovaneho modelu
 * @param type urcuje ci sa jedna o hraca alebo strazcu
 */
void ModelManager::RegisterModel(int id, ModelType type)
{
        std::map < std::pair<int, playerDirection>, QPixmap > *data;
        if( type == PlayerModel )
        {
                data = &modelDatabasePlayer;
        }
        else
        {
                data = &modelDatabaseGuard;
        }
        for( int i =0; i < 4; i++ )
        {
                QString str = QString( ":/models/resources/hraci/" ) + QString::number(id) + QString( "_" ) + QString::number(i) + QString(".png");
                data->insert( std::make_pair( std::make_pair( id, static_cast<playerDirection>(i) ), QPixmap(str) ) );
        }
}

/**
 * @brief ModelManager::GetModel
 * @param id id registrovaneho modelu
 * @param dir smer, ktorym je hrac otoceny
 * @param type urcuje ci sa jedna o hraca alebo strazcu
 * @return referencia na objekt QPixmap, ktory predstavuje danu texturu v pamati
 */
QPixmap &ModelManager::GetModel( int id, playerDirection dir, ModelType type )
{
        if( type == PlayerModel )
        {
                return modelDatabasePlayer[ std::make_pair( id, dir ) ];
        }
        else
        {
                return modelDatabaseGuard[ std::make_pair( id, dir ) ];
        }
}

QPixmap & ModelManager::GetDoorOpened()
{
    return door_opened;
}

QPixmap & ModelManager::GetDoorClosed()
{
    return door_closed;
}

QPixmap & ModelManager::GetFloor()
{
    return floor;
}

QPixmap & ModelManager::GetWall()
{
    return wall;
}

QPixmap & ModelManager::GetKey()
{
    return key;
}

QPixmap & ModelManager::GetFinish()
{
    return finish;
}
