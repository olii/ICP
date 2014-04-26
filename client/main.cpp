#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

using std::cout;
using std::endl;




class Network
{
public:
    Network( boost::asio::io_service& io, tcp::resolver::iterator endpoint_iterator ):
        io_(io), socket_(io), endpoint( endpoint_iterator ), connected(false)
    {
    }
    ~Network(  )
    {
        Shutdown();
    }

    void Connect()
    {
        socket_.connect(*endpoint);

        // ak nenastane vynimka tak sa pokracuje
        connected = true;
    }

    void Shutdown()
    {
        if (connected){
            socket_.shutdown( tcp::socket::shutdown_both );
            socket_.close();
            connected = false;
        }
        shutdown = true;
    }

    void AsyncReadHeader()
    {
        boost::asio::async_read(socket_, boost::asio::buffer( header ), boost::asio::transfer_exactly( 8 ),
          boost::bind( &Network::HandleHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
    }

    void HandleHeader( const boost::system::error_code& error, std::size_t bytes_transferred )
    {
        if ( error )
        {
            cout << "Network: Error " << error << endl;
            shutdown = true;
            return;
        }
        cout << "Received header with id = " << header[0] << ", length = " << header[1] << "." << endl;
    }

    void AsyncRead()
    {
        AsyncReadHeader();
    }



private:
    boost::asio::io_service &io_;
    tcp::socket socket_;
    tcp::resolver::iterator endpoint;
    bool connected;
    bool shutdown;
    uint32_t header[2];
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);

    Network net(io_service, iterator);

    net.Connect();
    net.AsyncRead();

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    char line[100 + 1];
    while (std::cin.getline(line, 100 + 1))
    {
      using namespace std; // For strlen and memcpy.
      /*chat_message msg;
      msg.body_length(strlen(line));
      memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      c.write(msg);*/
    }

    net.Shutdown();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
