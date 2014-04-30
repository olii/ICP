#include <cstdlib>
#include <deque>
#include <vector>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../shared/global.h"
#include "../shared/command.h"
#include "../shared/serverinfo.h"

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
        boost::asio::async_read(socket_, boost::asio::buffer( inheader ), boost::asio::transfer_exactly( packetHeader::header_size ),
          boost::bind( &Network::HandleHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
    }

    void HandleHeader( const boost::system::error_code& error, std::size_t /*bytes_transferred*/ )
    {
        if ( error )
        {
            cout << "Network: Error " << error << endl;
            shutdown = true;
            return;
        }
        cout << "Received header with id = " << inheader[0] << ", length = " << inheader[1] << "." << endl;
        switch (inheader[0])
        {
            case packetHeader::COMMAND:
                cout << "This packet is COMMAND" << endl;
                recvCommand();
                break;

            case packetHeader::SERVER_LIST:
                cout << "This packet is SERVER LIST" << endl;
                recvServerList();
                break;

            default:
                break;
        }
    }
    void recvCommand()
    {
        indata.resize( inheader[1] );
        boost::asio::async_read(socket_, boost::asio::buffer( indata ), boost::asio::transfer_exactly( inheader[1] ),
          boost::bind( &Network::HandleCommand, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
    }
    void HandleCommand (const boost::system::error_code& error, std::size_t /*bytes_transferred*/ )
    {
        if ( error )
        {
            cout << "Network: Error " << error << endl;
            shutdown = true;
            return;
        }
        std::string archive_data(&indata[0], indata.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);

        Command t;
        archive >> t;
        cout << "RECEIVED DATA!!!" << endl;
        switch (t.type)
        {
            case Command::Type::TEXT:
                cout << ">>>>" <<t.text << "<<<<" << endl;
                break;
            default:
                cout << "Unknown data" << endl;
                break;
        }
        AsyncRead();
    }
    void recvServerList()
    {
        indata.resize( inheader[1] );
        boost::asio::async_read(socket_, boost::asio::buffer( indata ), boost::asio::transfer_exactly( inheader[1] ),
          boost::bind( &Network::HandleServerList, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
    }

    void HandleServerList(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
    {
        if ( error )
        {
            cout << "Network: Error " << error << endl;
            shutdown = true;
            return;
        }
        std::string archive_data(&indata[0], indata.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);

        ServerInfoList t;
        archive >> t;
        cout << "RECEIVED DATA!!!" << endl;
        for( auto x : t.list )
        {
            cout << x.id << " " << x.name << " " << x.playing << " " << x.max << " " << x.map << endl;
        }

        AsyncRead();
    }

    void AsyncRead()
    {
        AsyncReadHeader();
    }

    bool ValidServer()
    {
        boost::system::error_code ec;
        boost::asio::read(socket_, boost::asio::buffer(inheader), boost::asio::transfer_exactly(packetHeader::header_size), ec );

        if (ec || inheader[0] != packetHeader::COMMAND )
        {
            Shutdown();
            return false;
        }
        indata.resize(inheader[1]);

        boost::asio::read(socket_, boost::asio::buffer(indata), boost::asio::transfer_exactly(inheader[1]), ec );

        if (ec )
        {
            Shutdown();
            return false;
        }
        std::string archive_data(&indata[0], indata.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);

        Command t;
        archive >> t;
        if (t.type != Command::TEXT || t.text != std::string("ICP SERVER 2014 Bludisko\n") )
        {
            Shutdown();
            return false;
        }

        return true;
    }
    void ServerListRequest()
    {
        boost::system::error_code ignored_error;

        inheader[0] = packetHeader::SERVER_LIST;
        inheader[1] = 0;

        boost::asio::write(socket_, boost::asio::buffer(inheader),
            boost::asio::transfer_all(), ignored_error);
    }


private:
    boost::asio::io_service &io_;
    tcp::socket socket_;
    tcp::resolver::iterator endpoint;
    bool connected;
    bool shutdown;
    uint32_t inheader[2];
    std::vector<char> indata;
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


    try
    {
        net.Connect();
    }
    catch( std::exception& e )
    {
        cout << "Unable to connect to server. Reason: " << e.what() << endl ;
        return -1;
    }

    if ( ! net.ValidServer() )
    {

    }
    net.ServerListRequest();
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
