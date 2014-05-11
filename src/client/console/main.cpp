/**
 * @file main.cpp
 *
 * @brief implementacia konzoloveho klienta
 *
 * @author Oliver Nemcek
 */

#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../../shared/global.h"
#include "../../shared/command.h"
#include "../../shared/serverinfo.h"
#include "../../shared/map.h"
#include "../network.h"


#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <cstdlib>

#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

using std::cout;
using std::endl;
using std::cin;

void changemode(int);
int  kbhit(void);
void Exit_fix_terminal(void)
{
    changemode(0);
}


/**
 * @brief main
 *        je tu obsiahnuta kompletna implementacia vzhladom na demonstracne ucely, na ktore mala tato cast zadania sluzit
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    atexit (Exit_fix_terminal); ///< na konci sa opat na stavi cin na blokujuci
    changemode(-1); ///< ulozi sa aktualny stav terminalu
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: <host> <port>\n";
            return 1;
        }

        Network connection(argv[1], argv[2]);   ///< vytvorenie pripojenia na server
        connection.Connect();



        cout << "Join game: 0\nCreate game: 1\nYour Choice:" << endl;

        int b;
        cin >> b;
        if( !b )
        {
            ServerInfoList list = connection.GetServerList();
            for( auto x: list.list )
            {
                cout << x << endl;
            }
            int gameid;
            cout << "Enter game id to join: ";
            cin >> gameid;
            connection.Join(gameid);    ///< pripojenie sa k uz existujucej hre
        }
        else
        {
            ServerInfoList list = connection.GetMapList();
            for( auto x: list.list )    ///< zobrazia sa nazvy vsetkych dostupnych map
            {
                cout << "#- " << x.map << " -#" <<endl;
            }
            std::string mapname ;
            std::string gamename ;
            int players_max;
            float delay;
            int timeout;
            cout << "Enter game name: ";
            cin >> gamename;
            cout << "Enter maximum number of players: ";
            cin >> players_max;
            cout << "Enter map name: ";
            cin >> mapname;
            cout << "Enter delay (0.5-5.0 s): ";
            cin >> delay;
            cout << "Enter amount of time to wait for start of the game in seconds: ";
            cin >> timeout;
            connection.CreateServer(gamename, players_max, mapname, delay, timeout); ///< vytvorenie novej hry
        }


        /** server posle staticku mapu */
        connection.ReadPacket();
        if (connection.GetHeaderType() != packetHeader::STATIC_MAP )
        {
            throw std::runtime_error( "Invalid packet received form server." );
        }
        cout << "packet received" << endl;

        Map map = connection.GetPacketContent<Map>();   ///< obsah danej mapy sa upravy tak aby sa vykreslilo iba pozadie - inicializacia pohladu
        Map::MapMatrix background = map.items;
        for (auto &row: background)
        {
            for (auto &column: row)
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

        MapUpdate updatepacket = connection.GetMapUpdate(); ///< server zaslal prvy update
        Map::MapMatrix screenbuffer = background;
        for ( auto item: updatepacket.players ) ///< v terminalovej verzii je kazdy hrac rovnaky
        {
            screenbuffer[item.x][item.y] = static_cast<Map::StaticTypes>( Map::PLAYER_BASE );
        }



        for (auto &row: screenbuffer)   ///< vykreslenie prveho update
        {
            for (auto &column: row)
            {
                switch(column)
                {
                    case Map::GRASS:
                        cout<<"░";
                        break;
                    case Map::WALL:
                        cout << "█";
                        break;
                    case Map::GATE:
                        cout << 'G';
                        break;
                    case Map::KEY:
                        cout << 'K';
                        break;
                    case Map::FINISH:
                        cout << 'F';
                        break;
                    case Map::PLAYER_BASE:
                        cout << 'P';
                    default:
                        break;
                }
            }
            cout << endl;
        }

        //cin.ignore(INT_MAX);
        changemode(1);  ///< nastavenie cin na neblokujuci
        /* MAIN GAME LOOP */
        while ( !cin.eof() )
        {
            if ( kbhit() )  ///< reakcia na stisk klavesy
            {
                char c;
                cin.get(c);
                if ( c == 27 ) // ESC
                {
                    cout << "ESCAPE";
                    connection.Leave();
                    break;
                }
                if( c == 'w' )
                {
                    Command c;
                    c.SetType(Command::GO);
                    connection.SendCommand(c);
                }
                if( c == 's' )
                {
                    Command c;
                    c.SetType(Command::STOP);
                    connection.SendCommand(c);
                }
                if( c == 'a' )
                {
                    Command c;
                    c.SetType(Command::LEFT);
                    connection.SendCommand(c);
                }
                if( c == 'd' )
                {
                    Command c;
                    c.SetType(Command::RIGHT);
                    connection.SendCommand(c);
                }
                if( c == 'e' )
                {
                    Command c;
                    c.SetType(Command::TAKE);
                    connection.SendCommand(c);
                }
                if( c == 'q' )
                {
                    Command c;
                    c.SetType(Command::OPEN);
                    connection.SendCommand(c);
                }
            }
            if ( connection.Ready() )   ///< reakcia na packet, ktory prave dorazil
            {
                connection.ReadPacket();
                if( connection.GetHeaderType() == packetHeader::COMMAND )
                {
                    Command c = connection.GetPacketContent<Command>();

                    if( c.GetType() == Command::TEXT )  ///< ak mi sietou neprisiel novy obsah mapy tak prisla textova sprava, ktoru vypisem
                    {
                        cout << c.GetText() << endl;
                    }
                    else
                    {
                        cout << "Received packet different than text or gameupdate";
                    }
                }
                else if ( connection.GetHeaderType() == packetHeader::GAME_UPDATE  ) ///< server zaslal game update
                {
                    MapUpdate updatepacket = connection.GetPacketContent<MapUpdate>();
                    Map::MapMatrix screenbuffer = background;
                    screenbuffer[updatepacket.treasure.x][updatepacket.treasure.y] = Map::FINISH;   ///< vlozenie pokladu
                    for ( auto item: updatepacket.gates )   ///< vlozenie bran
                    {
                        if ( item.optionFlag == true )
                        {
                            screenbuffer[item.x][item.y] = Map::GATE;
                        }
                        else
                        {
                            screenbuffer[item.x][item.y] = Map::GATE_OPEN;
                        }
                    }
                    for ( auto item: updatepacket.keys )    ///< vlozenie klucov
                    {
                        if ( item.optionFlag == true )
                        {
                            screenbuffer[item.x][item.y] = Map::KEY;
                        }
                    }
                    for ( auto item: updatepacket.guards ) ///< vlozenie strazcov
                    {
                        screenbuffer[item.x][item.y] = static_cast<Map::StaticTypes>( Map::GUARD_BASE);
                    }
                    for ( auto item: updatepacket.players ) ///< vlozenie hracov
                    {
                        if(item.optionFlag == true )    ///< vykresluju sa len hraci, ktori su zivi
                        {
                            screenbuffer[item.x][item.y] = static_cast<Map::StaticTypes>( Map::PLAYER_BASE + item.dir);
                        }
                    }



                    for (auto &row: screenbuffer)   ///< vykreslenie celej hry
                    {
                        for (auto &column: row)
                        {
                            switch(column)
                            {
                                case Map::GRASS:
                                    cout<<" ";
                                    break;
                                case Map::WALL:
                                    cout << "█";
                                    break;
                                case Map::GATE:
                                    cout << "G";
                                    break;
                                case Map::GATE_OPEN:
                                    cout << "░";
                                    break;
                                case Map::GUARD_BASE:
                                    cout << "X";
                                    break;
                                case Map::KEY:
                                    cout << "K";
                                    break;
                                case Map::FINISH:
                                    cout << "F";
                                    break;
                                case static_cast<Map::StaticTypes>( Map::PLAYER_BASE + 0):
                                    cout << "↑";
                                    break;
                                case static_cast<Map::StaticTypes>( Map::PLAYER_BASE + 1):
                                    cout << "→";
                                    break;
                                case static_cast<Map::StaticTypes>( Map::PLAYER_BASE + 2):
                                    cout << "↓";
                                    break;
                                case static_cast<Map::StaticTypes>( Map::PLAYER_BASE + 3):
                                    cout << "←";
                                    break;
                                default:
                                    break;
                            }
                        }
                        cout << endl;
                    }
                    cout << endl;
                }
                else
                {
                    //cout << "Unknown packet header " << connection.GetHeaderType() << endl;
                }
            }

            std::this_thread::sleep_for (std::chrono::milliseconds(10));    ///< klient sa uspi na 10 misec. a znova kontroluje obsah socketu
        }
        changemode(0);  ///< nastavi sa cin na povodny blokujuci
    }
    catch (std::exception& e)
    {
        changemode(0); ///< nastavi sa cin na povodny blokujuci
        std::cerr << "Exception: " << e.what() << "\n";
    }
    changemode(0); ///< nastavi sa cin na povodny blokujuci
    return 0;
}


/**
 * @brief changemode
 * @param dir logicky parameter, ktory riedi ci sa ma cin nastavenie ulozit, ci sa nastavi / nenastavi ako blokujuci
 */
void changemode(int dir)
{
  static struct termios oldt, newt, tmp;

  if ( dir == -1 )
  {
      tcgetattr( STDIN_FILENO, &oldt);
  }
  else if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &tmp);
    newt = tmp;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}

/**
 * @brief kbhit
 *        riedi obsluhu udalosti stisk klavesy
 * @return vrati nenulovu hodotu pri stisku klavesy
 */
int kbhit (void)
{
  struct timeval tv;
  fd_set rdfs;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);

  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);

}
