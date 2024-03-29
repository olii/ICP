/** @file game.cpp
* @author Oliver Nemček
* @brief abstrahuje hraca so vsetkymi metodami nutnymi k
* hraniu hry a ku komunikacii po sieti
*/

#include "game.h"
#include "manager.h"
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>
#include <set>
#include <utility>
#include <boost/bind.hpp>
#include <random>


using std::set;

uint32_t Game::index = 0;

/**
 * @brief konstruktor, ktory vytvori hru podla zadanych parametrov
 * hra je po konstrukcii pripravena na prihlasenie hracov
 */
Game::Game(std::string name, int max, float tick, int timeout, Map map ):index_( Game::index++ ), timer(io_service)
{
    this->name = name;
    this->maxPlayers = max;
    this->tick = boost::posix_time::seconds(int(tick)) + boost::posix_time::millisec( (tick - int(tick))*1000 );
    this->timeout = timeout;
    this->map_original = map;
    this->tickF = tick;
    for ( int i = 0; i<playerModel::COUNT;i++ ) player_model.insert(i); // dostupne modely

    matrix = map.items;

    updatePacket.players.clear();
    updatePacket.guards.clear();
    updatePacket.keys.clear();
    updatePacket.gates.clear();
    updatePacket.treasure = MapItemsInfo(0,0);

    int guardIndexer = 0;

    for( uint i =0; i<matrix.size();++i )
    {
        for( uint j=0; j<matrix[i].size();++j)
        {
            switch (matrix[i][j])
            {
                case Map::GRASS:
                case Map::WALL:
                    break;
                case Map::GATE:
                    updatePacket.gates.push_back( MapItemsInfo( i,j, true ) );
                    break;
                case Map::PLAYER_SPAWN:
                {
                    Point tmp = std::make_pair(uint(i), uint(j));
                    playerSpawn.push_back( tmp );
                    matrix[i][j] = Map::GRASS;
                    break;
                }
                case Map::KEY:
                    matrix[i][j] = Map::GRASS;
                    updatePacket.keys.push_back( MapItemsInfo( i,j, true ) );
                    break;
                case Map::GUARD_SPAWN:
                {
                    matrix[i][j] = Map::GUARD_BASE;

                    playerDirection dir = static_cast<playerDirection>(Manager::instance().Random() % 4); // random direction
                    int model = (Manager::instance().Random() % GuardModel::GUARD_COUNT) + playerModel::COUNT;  // choose model
                    updatePacket.guards.push_back( MapItemsInfo( i, j,true,dir, model,guardIndexer++ ) ); // adding guards to update
                    break;
                }
                case Map::FINISH:
                    updatePacket.treasure = MapItemsInfo(i, j);
                    break;
                default:
                    break;
            }

        }
    }
    messageQue.clear();
    //std::cout << "Game [" << index_ << "  " << this->name << "] constructed" << std::endl;
}

Game::~Game()
{
    timer.cancel();
    players.clear();
    messageQue.clear();
    //std::cout << "Game [" << index_ <<"] destructed, message que="<< messageQue.size() << std::endl;
}

/**
 * @brief Metóda prida hraca do hry
 * @param[in] user zdielany pointer hraca
 * @return true ak sa pridanie podarilo inak false
 */
bool Game::Join( boost::shared_ptr<player> user )
{
    if (players.size() >= maxPlayers)
    {
        //std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] couldn't join to the game. Exceeding maxplayers." << std::endl;
        return false;
    }
    players.insert(user);

    user->SetModel(pick_model());

    user->SendStaticMap(map_original);

    /* find player spawn */
    Point clientSpawn = std::make_pair( 9999,9999 );
    for( auto spawnPos: playerSpawn)
    {
        if ( matrix[spawnPos.first][spawnPos.second] == Map::GRASS )
        {
            clientSpawn = spawnPos;
            break;
        }
    }
    if( clientSpawn.first != 9999 && clientSpawn.second != 9999 )
    {
        user->x = clientSpawn.first;
        user->y = clientSpawn.second;
        user->alive = true;
        user->keys = 0;
        user->steps = 0;
        user->dir = static_cast<playerDirection>(Manager::instance().Random() % 4); // nahodne smer
        user->encounteredKey = 0;
        user->startTime = boost::posix_time::second_clock::local_time();
        matrix[clientSpawn.first][clientSpawn.second] = static_cast<Map::StaticTypes>( Map::PLAYER_BASE );
    }
    else
    {
        //std::cout << "TODO!!!!!!!!!!!!!!!!!! Spawn not found" << std::endl;
        Leave(user);
        return false;
    }
    // std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] added to the game with model = " << user->GetModel()<< "." << std::endl;

    /* send initial map update to all */
    Dispatch();

    if ( players.size() == 1 ) // run the timer
    {
        Start();
    }
    std::string message;
    message = std::string("New Player with id ") + std::to_string(user->GetIndex()) + std::string(" joined game.");
    SendTextToAll(message);

    return true;
}

/**
 * @brief metoda ktora odhlasi hraca z hry
 * @param[in] user zdielany pointer na hraca
 * @return Index vo vektore hlídačov @see Interface::guards
 */
void Game::Leave( boost::shared_ptr<player> user )
{
    //std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] leaving the game." << std::endl;
    players.erase( user );
    RemovePlayerMessage( user );
    player_model.insert(user->GetModel());


    /* odstranit kluce TODO */
    if( user->keys > 0 )
    {
        ReturnKeys(user);
    }

    matrix[user->x][user->y] = Map::GRASS;


    if ( players.empty() )
    {
        //std::cout << "Game [" << index_ <<"]: is empty." << std::endl;
        timer.cancel();
        Manager::instance().DestroyGame(shared_from_this());
    }
    else
    {
        Dispatch();
    }
    std::string message = std::string("New Player with id ") + std::to_string(user->GetIndex()) + std::string(" has left the game.");
    SendTextToAll(message);
}

/**
 * @brief testuje ci je dany hrac pripojeny v hre
 */
bool Game::Joined( boost::shared_ptr<player> user )
{
    return (players.find(user) != players.end());
}


/**
 * @brief metoda hraguje na herne spravy od hraca
 * @param[in] user hrac
 * @param[in] Herna sprava typu Command
 */
void Game::GameMessage(boost::shared_ptr<player> user, Command c)
{
    //std::cout << "Game [" << index_ <<"]: player[" << user->GetIndex() << "] got command" << std::endl;
    if( win == true )
    {
        user->SendString("Game has been won. Command not accepted.");
        return;
    }
    if( user->alive != true )
    {
        user->SendString("You are dead. Command not accepted.");
        return;
    }

    switch (c.GetType()) {
        case Command::LEFT:
            user->dir = static_cast<playerDirection>((user->dir + 3)%4);
            RemovePlayerMessage(user);
            Dispatch();
            break;

        case Command::RIGHT:
            user->dir = static_cast<playerDirection>((user->dir + 1)%4);
            RemovePlayerMessage(user);
            Dispatch();
            break;

        case Command::STOP:
            RemovePlayerMessage(user);
            break;

        case Command::GO:
            RemovePlayerMessage(user);
            messageQue.emplace_back( user, c );
            break;

        case Command::TAKE:
        {
            Point new_coord = GetNextPosition( user );
            auto is_key = IsItem( new_coord.first, new_coord.second, updatePacket.keys );
            if( is_key != updatePacket.keys.end() )
            {
                updatePacket.keys.erase(is_key);
                user->keys += 1;
                user->encounteredKey = false; // turn of stop on key encountered
                Dispatch();
            }
            else
            {
                user->SendString("There is no key you can take.");
            }
            break;
        }
        case Command::OPEN:
        {
            Point new_coord = GetNextPosition( user );
            try
            {
                if ( matrix.at(new_coord.first).at(new_coord.second) == Map::GATE )
                {
                    if ( user->keys <= 0 )
                    {
                        user->SendString("You have no keys left.");
                        break;
                    }
                    else
                    {
                        matrix[new_coord.first][new_coord.second] = Map::GRASS; // remove from map
                        auto gate_iter = IsItem( new_coord.first, new_coord.second, updatePacket.gates );
                        gate_iter->optionFlag = false; // set as open
                        user->keys -= 1;
                        Dispatch();
                    }
                }
                else
                {
                    user->SendString("There is no gate you can open.");
                }
            }
            catch (const std::out_of_range& )
            {
                user->SendString("There is no gate you can open.");
            }
            break;
        }

        default:
            break;
    }

}

bool Game::Full()
{
    return ( players.size() >= maxPlayers );
}

uint32_t Game::GetIndex()
{
    return index_;
}

std::string Game::GetName()
{
    return name;
}

int Game::GetPlayerCount()
{
    return players.size();
}

int Game::GetMaxPlayers()
{
    return maxPlayers;
}

std::string Game::GetMap()
{
    return map_original.name;
}

float Game::GetTick()
{
    return tickF;
}

int Game::GetTimeout()
{
    return timeout;
}

void Game::Shutdown()
{
    timer.cancel();
    for( auto &x: players )
    {
        x->LeaveServerRequest();
    }
    players.clear();
    messageQue.clear();
}

/**
 * @brief metoda spusta interny casovac hry
 */
void Game::Start()
{
    win = false;
    if( timeout <= 0 )
    {
        SendTextToAll(std::string("Good luck!") );
        boost::system::error_code ignored;
        timer.expires_from_now(boost::posix_time::seconds(0));
        startTime = boost::posix_time::second_clock::local_time();
        GameLoop(ignored);
    }
    else
    {
        SendTextToAll(std::string("Game starts in: ") +  std::to_string(timeout) + std::string(" seconds.") );
        timer.expires_from_now(boost::posix_time::seconds(1));
        timer.async_wait( boost::bind( &Game::WaitLoop, shared_from_this(), boost::asio::placeholders::error ) );
        timeout--;
    }
}

/**
 * @brief casovac timeoutu hry
 */
void Game::WaitLoop(const boost::system::error_code &error)
{
    if( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            //std::cout << "Game [" << index_ <<"]: timer operation aborted" << std::endl;
            return;
        }
        //std::cout << "Game [" << index_ <<"]: timer error " << error << std::endl;
    }
    if( timeout <= 0 )
    {
        SendTextToAll(std::string("Good luck!") );
        startTime = boost::posix_time::second_clock::local_time();
        boost::system::error_code ignored;
        GameLoop(ignored);
    }
    else
    {
        SendTextToAll(std::string("Game starts in: ") +  std::to_string(timeout) + std::string(" seconds.") );
        timer.expires_from_now(boost::posix_time::seconds(1));
        timer.async_wait( boost::bind( &Game::WaitLoop, shared_from_this(), boost::asio::placeholders::error ) );
        timeout--;
    }
}


/**
 * @brief zakladny herny cyklus, volany periodicky
 */
void Game::GameLoop(const boost::system::error_code &error)
{
    if( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            //std::cout << "Game [" << index_ <<"]: timer operation aborted" << std::endl;
            return;
        }
        //std::cout << "Game [" << index_ <<"]: timer error " << error << std::endl;
        Shutdown();
        Manager::instance().DestroyGame(shared_from_this());
    }

    /* prechod hracskymi prikazmi a vykonavanie*/
    std::list< std::pair< boost::shared_ptr<player>, Command>  >::iterator it;
    it = messageQue.begin();
    while (it != messageQue.end())
    {
        if( win )
        {
            messageQue.erase(it++);
            continue;
        }
        Point new_coord = GetNextPosition( it->first );

        try
        {
            if ( IsItem( new_coord.first, new_coord.second, updatePacket.keys ) != updatePacket.keys.end() ) // new position has key
            {
                // player was already stopped on key, he wandted to move, so move him
                if ( it->first->encounteredKey == true )
                {
                    matrix[it->first->x][it->first->y] = Map::GRASS;
                    matrix[new_coord.first][new_coord.second] = Map::PLAYER_BASE;
                    it->first->x = new_coord.first;
                    it->first->y = new_coord.second;
                    it->first->steps += 1;
                    it->first->encounteredKey = false;
                    it++;
                }
                else
                {
                    // he just encountered key on the map, stop him
                    it->first->encounteredKey = true;
                    messageQue.erase(it++);
                }

            }
            else if ( matrix.at(new_coord.first).at(new_coord.second) == Map::GRASS )
            {
                matrix[it->first->x][it->first->y] = Map::GRASS;
                matrix[new_coord.first][new_coord.second] = Map::PLAYER_BASE;
                it->first->x = new_coord.first;
                it->first->y = new_coord.second;
                it->first->steps += 1;
                it++;
            }
            else if ( matrix.at(new_coord.first).at(new_coord.second) == Map::FINISH )    // somebody reached end
            {
                matrix[it->first->x][it->first->y] = Map::GRASS;
                matrix[new_coord.first][new_coord.second] = Map::PLAYER_BASE;
                it->first->x = new_coord.first;
                it->first->y = new_coord.second;
                it->first->steps += 1;
                it++;
                /*now we stop the game, because somebody win*/
                win = true;
                SendTextToAll("Congratulations. You Win.");
                boost::posix_time::time_duration diff;
                diff = boost::posix_time::second_clock::local_time() - startTime;
                SendTextToAll("Game Time: " + boost::posix_time::to_simple_string(diff));
                //TODO SEND STATS
                updatePacket.treasure.optionFlag = true;
            }
            else
            {
                messageQue.erase(it++);
            }
        }
        catch (const std::out_of_range& )
        {
            messageQue.erase(it++);
        }
    }

    /* do AI*/
    UpdateAI();


    //SendTextToAll( "THIS IS LONG STRING" );
    Dispatch();

    timer.expires_at(timer.expires_at() + tick);
    timer.async_wait( boost::bind( &Game::GameLoop, shared_from_this(), boost::asio::placeholders::error ) );
}

/**
 * @brief odosle text vsetkym hracom v hre
 * @param[in] text retazec
 */
void Game::SendTextToAll(  std::string text)
{
    for( auto x: players )
    {
        x->SendString( text );
    }
}


/**
 * @brief odosle vsetkym hracom v hre novy paket s aktualizaciami
 */
void Game::Dispatch()
{
    updatePacket.players.clear();
    for ( auto &x: players )
    {
        boost::posix_time::time_duration diff;
        diff = boost::posix_time::second_clock::local_time() - startTime;
        MapItemsInfo p(x->x, x->y,x->alive, x->dir, x->GetModel(),x->GetIndex(),boost::posix_time::to_simple_string(diff), x->keys, x->steps );
        updatePacket.players.push_back(p);
    }

    /*update timer of whole game*/
    if( !win )
    {
        boost::posix_time::time_duration diff;
        diff = boost::posix_time::second_clock::local_time() - startTime;
        updatePacket.treasure.stringField = boost::posix_time::to_simple_string(diff);
    }

    for( auto &x: players )
    {
        x->SendMapUpdate( updatePacket );
    }
}

void Game::RemovePlayerMessage(boost::shared_ptr<player> user)
{
    messageQue.remove_if([&user](std::pair< boost::shared_ptr<player>, Command> &x)
    {
        return x.first == user;
    });
}

int Game::pick_model()
{
    int offset = (Manager::instance().Random())%player_model.size();
    auto it = player_model.begin();
    for (; offset != 0; offset--) it++;

    int retval = *it;
    player_model.erase(retval);
    return retval;
}


/**
 * @brief navrati do mapy vsetky kluce ktore mal hrac
 * kluce sa spawnuju na poziciu okolo hraca do kruhu
 */
void Game::ReturnKeys( boost::shared_ptr<player> user )
{
    /* returning keys to map based on iterative list algorithm*/

    std::list<Point> positions;
    std::set<Point> closed;

    positions.push_back( std::make_pair(user->x, user->y) ); // insert initial position

    while( user->keys !=0 && !positions.empty() )
    {
        Point point = positions.front();
        positions.pop_front();

        if( closed.count(point) > 0 )
        {
            /* already in closed list */
            continue;
        }
        else
        {
            closed.insert(point);
        }

        try
        {
            if ( matrix.at(point.first).at(point.second) != Map::WALL )
            {
                auto it = IsItem( point.first, point.second, updatePacket.gates );
                /* position is not CLOSED gate nor key nor wall*/
                if ( IsItem( point.first, point.second, updatePacket.keys ) == updatePacket.keys.end() &&
                     ( IsItem( point.first, point.second, updatePacket.gates ) == updatePacket.gates.end() ||
                       it->optionFlag == false )
                    )
                {
                    updatePacket.keys.push_back( MapItemsInfo( point.first, point.second, true ) );
                    user->keys -= 1;

                    positions.push_back( std::make_pair(point.first+1, point.second) );
                    positions.push_back( std::make_pair(point.first-1, point.second) );
                    positions.push_back( std::make_pair(point.first, point.second+1) );
                    positions.push_back( std::make_pair(point.first, point.second-1) );
                }
            }
        }
        catch (const std::out_of_range& )
        { }
    }

    /* all keys are back in game*/
}

/**
 * @brief aktualizuje umelu inteligenciu vsetkych hlidacov
 * zalozena na rand()
 */
void Game::UpdateAI()
{
    for( auto &guard : updatePacket.guards )
    {
        Point new_coord = std::make_pair( guard.x, guard.y );
        switch (guard.dir)
        {
            case playerDirection::UP:
                new_coord.first -= 1;
                break;
            case playerDirection::RIGHT:
                new_coord.second += 1;
                break;
            case playerDirection::DOWN:
                new_coord.first += 1;
                break;
            case playerDirection::LEFT:
                new_coord.second -= 1;
                break;
            default:
                break;
        }

        /*test this new coord*/
        try
        {
            if ( matrix.at(new_coord.first).at(new_coord.second) == Map::PLAYER_BASE && !win)
            {
                // kill player
                /* find out who */
                for( auto &user: players )
                {
                    if( user->x == int(new_coord.first) && user->y == int(new_coord.second) )
                    {
                        /* kill this player */
                        std::string message = std::string("Player with id ") + std::to_string(user->GetIndex()) + std::string(" was killed.");
                        SendTextToAll(message);

                        boost::posix_time::time_duration diff;
                        diff = boost::posix_time::second_clock::local_time() - user->startTime;
                        SendTextToAll("It took him only: " + boost::posix_time::to_simple_string(diff) + " to get killed.");


                        user->alive = false;
                        matrix[new_coord.first][new_coord.second] = Map::GUARD_BASE;
                        matrix[guard.x][guard.y] = Map::GRASS;
                        guard.x = new_coord.first;
                        guard.y = new_coord.second;
                        ReturnKeys(user);
                        RemovePlayerMessage(user);


                        /* find player spawn */
                        Point clientSpawn = std::make_pair( 9999,9999 );
                        for( auto spawnPos: playerSpawn)
                        {
                            if ( matrix[spawnPos.first][spawnPos.second] == Map::GRASS )
                            {
                                clientSpawn = spawnPos;
                                break;
                            }
                        }
                        if( clientSpawn.first != 9999 && clientSpawn.second != 9999 )
                        {
                            user->x = clientSpawn.first;
                            user->y = clientSpawn.second;
                            user->alive = true;
                            user->keys = 0;
                            user->dir = static_cast<playerDirection>(Manager::instance().Random() % 4); // nahodne smer
                            user->encounteredKey = 0;
                            matrix[clientSpawn.first][clientSpawn.second] = static_cast<Map::StaticTypes>( Map::PLAYER_BASE );
                        }


                        /* test if all players are dead */
                        auto it = find_if(players.begin(), players.end(), [] ( boost::shared_ptr<player> ptr ) { return ptr->alive == true; } );
                        if( it == players.end() )
                        {
                            SendTextToAll("All players are dead. Game over.");
                            //won = true; // make game stopped, HACK
                        }

                    }
                    else
                    {
                        continue;
                    }
                }

            }
            else if ( matrix.at(new_coord.first).at(new_coord.second) != Map::WALL &&
                      matrix.at(new_coord.first).at(new_coord.second) != Map::GATE &&
                      matrix.at(new_coord.first).at(new_coord.second) != Map::GUARD_BASE &&
                      matrix.at(new_coord.first).at(new_coord.second) != Map::FINISH &&
                      guard.steps > 0 )
            {
                //do move
                guard.steps--;
                matrix[new_coord.first][new_coord.second] = Map::GUARD_BASE;
                matrix[guard.x][guard.y] = Map::GRASS;
                guard.x = new_coord.first;
                guard.y = new_coord.second;
            }
            else
            {
                guard.steps = Manager::instance().Random()%10;
                if( Manager::instance().Random()%2 )
                {
                    guard.dir = static_cast<playerDirection>((guard.dir + 3)%4);
                }
                else
                {
                    guard.dir = static_cast<playerDirection>((guard.dir + 1)%4);
                }
                continue;
            }

        }
        catch (const std::out_of_range& )
        {
            if( Manager::instance().Random()%2 )
            {
                guard.dir = static_cast<playerDirection>((guard.dir + 3)%4);
            }
            else
            {
                guard.dir = static_cast<playerDirection>((guard.dir + 1)%4);
            }
        }
    }
}



std::vector<MapItemsInfo>::iterator Game::IsItem(int x, int y, std::vector<MapItemsInfo> &where )
{
    return find_if(where.begin(), where.end(), [x, y] (const MapItemsInfo &s) { return (s.x  == x && s.y == y); } );
}

/**
 * @brief vypocita novu poziciu hraca podla smeru v ktorom je otoceny
 * @param[in] user hrac
 */
Point Game::GetNextPosition( boost::shared_ptr<player> user )
{
    Point new_coord = std::make_pair( user->x, user->y );
    switch (user->dir)
    {
        case playerDirection::UP:
            new_coord.first -= 1;
            break;
        case playerDirection::RIGHT:
            new_coord.second += 1;
            break;
        case playerDirection::DOWN:
            new_coord.first += 1;
            break;
        case playerDirection::LEFT:
            new_coord.second -= 1;
            break;
        default:
            break;
    }
    return new_coord;
}
