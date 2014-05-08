#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../shared/global.h"
#include "../shared/command.h"
#include "../shared/serverinfo.h"
#include "../shared/map.h"
#include "network.h"


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

int main(int argc, char* argv[])
{
    atexit (Exit_fix_terminal);
    changemode(-1);
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: <host> <port>\n";
            return 1;
        }

        Network connection(argv[1], argv[2]);
        connection.Connect();
        //connection.Join(0);




        ServerInfoList list = connection.GetServerList();
        for( auto x: list.list )
        {
            cout << x << endl;
        }
        if( list.list.size() > 0 )
        {
            int gameid;
            cout << "Enter game id to join: ";
            cin >> gameid;
            connection.Join(gameid);
        }
        else
        {
            ServerInfoList list = connection.GetMapList();
            for( auto x: list.list )
            {
                cout << "#- " << x.map << " -#" <<endl;
            }
            std::string mapname ;
            cout << "Enter map name: ";
            cin >> mapname;
            connection.CreateServer("SERVER0", 2, mapname, 1, 5);
        }


        /* server posle staticku mapu */
        connection.ReadPacket();
        if (connection.GetHeaderType() != packetHeader::STATIC_MAP )
        {
            throw std::runtime_error( "Invalid packet received form server." );
        }
        cout << "packet received" << endl;

        Map map = connection.GetPacketContent<Map>();

        Map::MapMatrix screenbuffer = map.items;
        for (auto &row: screenbuffer)
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
                    case Map::GUARD_SPAWN:
                    case Map::PLAYER_SPAWN:
                        cout<<"░";
                        break;
                    case Map::FINISH:
                        cout << 'F';
                        break;
                    default:
                        break;
                }
            }
            cout << endl;
        }

        //cin.ignore(INT_MAX);
        changemode(1);
        /* MAIN GAME LOOP */
        while ( !cin.eof() )
        {
            if ( kbhit() )
            {
                char c;
                cin.get(c);
                if ( c == 27 ) // ESC
                {
                    cout << "ESCAPE";
                    connection.Leave();
                    break;
                }
                if( c == 'g' )
                {
                    Command c;
                    c.SetType(Command::GO);
                    connection.SendCommand(c);
                }
                if(c == 'x' )
                {
                    Command c;
                    c.SetType(Command::TEXT);
                    connection.SendCommand(c);
                }
                cout << "you hit " << c << endl;
            }
            if ( connection.Ready() )
            {
                connection.ReadPacket();
                cout << "packet header " << connection.GetHeaderType() << endl;
            }

            std::this_thread::sleep_for (std::chrono::milliseconds(20));
        }
        changemode(0);
    }
    catch (std::exception& e)
    {
        changemode(0);
        std::cerr << "Exception: " << e.what() << "\n";
    }
    changemode(0);
    return 0;
}





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
