
#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "../../shared/global.h"

#include <map>
#include <utility>
#include <QPixmap>

class ModelManager
{
public:
        enum ModelType { PlayerModel, GuardModel};

        static ModelManager& GetInstance()
        {
            static ModelManager Instance;

            return Instance;
        }

        void RegisterModel(int id, ModelType type);
        QPixmap &GetModel( int id, playerDirection dir, ModelType type );

        QPixmap &GetDoorOpened();
        QPixmap &GetDoorClosed();
        QPixmap &GetFloor();
        QPixmap &GetWall();
        QPixmap &GetKey();
        QPixmap &GetFinish();
private:
        ModelManager();

        std::map < std::pair<int, playerDirection>, QPixmap > modelDatabasePlayer;
        std::map < std::pair<int, playerDirection>, QPixmap > modelDatabaseGuard;

        QPixmap floor;
        QPixmap wall;
        QPixmap door_closed;
        QPixmap door_opened;
        QPixmap key;
        QPixmap finish;
        //static ModelManager Instance; // singleton
};

#endif // MODELMANAGER_H
