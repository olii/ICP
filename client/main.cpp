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
#include "network.h"


#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <thread>
#include <chrono>

using boost::asio::ip::tcp;

using std::cout;
using std::endl;
using std::cin;

void changemode(int);
int  kbhit(void);

int main(int argc, char* argv[])
{
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
    connection.CreateServer("SERVER0", 1, "MAPNAME", 1.3, 5);

    changemode(1);
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
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}





void changemode(int dir)
{
  static struct termios oldt, newt;

  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
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
