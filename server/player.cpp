#include "../shared/global.h"
#include "player.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <string>
#include <sstream>
#include "manager.h"
#include "../shared/serverinfo.h"
#include "../shared/command.h"



using boost::asio::ip::tcp;
using std::cout;
using std::cerr;
using std::endl;

uint32_t player::index = 0;


player::player(boost::asio::io_service& io_service): socket_(io_service)
{
    index_ = index++;
    cout << "Player [" << index_ << "] constructed." << endl;
}


player::~player()
{
    if (socket_.is_open())
    {
        socket_.cancel();
        socket_.close();
    }
    cout << "Player [" << index_ << "] destructed." << endl;
}


void player::start()
{
    cout << "Player [" << index_ << "] starting with IP = " << socket_.remote_endpoint().address().to_string() << endl;

    /* POSLE HELLO PACKET */
    Command version("ICP SERVER 2014 Bludisko\n");
    SerializedData buffers = Serialize(version, version.HeaderCode::CODE);

    boost::asio::async_write( socket_, buffers,
        boost::bind( &player::listen, shared_from_this(), boost::asio::placeholders::error ) );
}

void player::ErrorShutdown()
{
    cout << "Player [" << index_ << "] entered ErrorShutdown()." << endl;

    if (shutdown) return;
    shutdown = true;

    if (!game) return;
    if (game->Joined( shared_from_this() ) )
    {
        game->Leave( shared_from_this() );
    }
    game.reset();
    if (socket_.is_open())
    {
        socket_.cancel();
        socket_.close();
    }
    return;
}

void player::SendServerList()
{

    ServerInfoList list = Manager::instance().ServerList();

    SerializedData buffers = Serialize(list, list.HeaderCode::CODE);

    boost::asio::async_write( socket_, buffers,
                              boost::bind( &player::handle_write, shared_from_this(), boost::asio::placeholders::error ) );
}

void player::ErrorMessage(std::string str)
{
    Command text(str);
    text.SetType(Command::ERROR_MESSAGE);

    SerializedData buffers = Serialize(text, text.HeaderCode::CODE);
    boost::system::error_code ec;
    boost::asio::write( socket_, buffers , ec);

    if (ec)
    {
        cout << "Player [" << index_ << "] player::ErrorMessage error=" << ec << endl;
        ErrorShutdown();
        return;
    }
    else
    {
        cout << "Player [" << index_ << "] error message sent: " << str << endl;
    }
}

uint32_t player::GetIndex()
{
    return index_;
}

void player::LeaveServerRequest()
{
    game.reset();
    ErrorShutdown();
}

void player::SendString( std::string str )
{
    Command text(str);
    SerializedData buffers = Serialize(text, text.HeaderCode::CODE);

    boost::asio::async_write( socket_, buffers ,
        boost::bind( &player::handle_write, shared_from_this(), boost::asio::placeholders::error ) );
}


void player::listen( const boost::system::error_code& error )
{
    if ( error )
    {
        cout << "Player [" << index_ << "] player::listen error=" << error << endl;
        ErrorShutdown();
        return;
    }
    do_read();
}


void player::do_read()
{
    boost::asio::async_read(socket_, boost::asio::buffer(inheader), boost::asio::transfer_exactly( packetHeader::header_size ),
      boost::bind( &player::handle_header, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
}


void player::handle_header(const boost::system::error_code& error, std::size_t bytes_transferred )
/* TODO */
{
    if ( error || bytes_transferred != packetHeader::header_size )
    {
        cout << "Player [" << index_ << "] player::listen error=" << error << endl;
        ErrorShutdown();
        return;
    }

    void (player::*f_ptr)(const boost::system::error_code& error, std::size_t bytes_transferred) = NULL;


    switch ( inheader[0] )  // typ paketu
    {
        case packetHeader::SERVER_LIST:
        {
            if ( inheader[1] != 0 )
            {
                cerr << "Player [" << index_ << "] received packetHeader::SERVER_LIST with not null payload size" << endl;
                ErrorShutdown();
                return;
            }
            if ( IsInGame() )
            {
                cerr << "Player [" << index_ << "] is already in game but asks for server list" << endl;
                ErrorShutdown();
                return;
            }
            cout << "Player [" << index_ << "] requested server list, proceeding." << endl;
            SendServerList();
            break;
        }
        case packetHeader::COMMAND:
        {
            if ( inheader[1] == 0 ) // sprava nema pokracovanie
            {
                cerr << "Player [" << index_ << "] received packetHeader::COMMAND with null payload size" << endl;
                ErrorShutdown();
                return;
            }
            f_ptr = &player::HandleCommand;
            break;
        }
        case packetHeader::CREATE_SERVER:
        {
            if ( inheader[1] == 0 ) // sprava nema pokracovanie
            {
                cerr << "Player [" << index_ << "] received packetHeader::CREATE_SERVER with null payload size" << endl;
                ErrorShutdown();
                return;
            }
            if( IsInGame() )
            {
                ErrorMessage("Cannot create server while joined in game.");
                ErrorShutdown();
                return;
            }
            f_ptr = &player::HandleCreateServer;
            break;
        }
        default:
            cerr << "Player [" << index_ << "] unknown header '" << inheader[0] << "', shutting down" << endl;
            ErrorShutdown();
            break;
    }

    if ( inheader[1] > 0 )
    {
        indata.resize(inheader[1]);
        boost::asio::async_read(socket_, boost::asio::buffer(indata), boost::asio::transfer_exactly( inheader[1] ),
          boost::bind( f_ptr, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
    }
    else
    {
        do_read();
    }
}
void player::HandleCommand(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if ( error || bytes_transferred != inheader[1] )
    {
        cout << "Player [" << index_ << "] player::HandleCommand error=" << error << endl;
        ErrorShutdown();
        return;
    }

    Command packet;
    Deserialize( packet );

    //enum Type { JOIN, LEAVE, TEXT, LEFT, RIGHT, GO, STOP, TAKE, OPEN, MAP_LIST, ERROR_MESSAGE  };
    switch (packet.type)
    {
        case Command::JOIN:
        {
            if ( IsInGame() )
            {
                ErrorMessage("Player is already in game. Cannot join another one. Leave first.");
                ErrorShutdown();
                return;
            }
            boost::shared_ptr< Game > new_game = Manager::instance().GetGameById(packet.id);

            if(!new_game)
            {
                ErrorMessage(std::string("No game with id ") + std::to_string(packet.id) );
                ErrorShutdown();
                return;
            }
            if ( ! new_game->Join( shared_from_this() ) )
            {
                ErrorMessage(std::string("Unable to join game. Game is full or error ocured.") );
                ErrorShutdown();
                return;
            }
            game = new_game;

            /* TODO SEND MAP */

            break;
        }
        case Command::LEAVE:
        {
            if ( !IsInGame() )
            {
                ErrorMessage("Player is not in game. Cannot leave.");
                ErrorShutdown();
                return;
            }
            game->Leave( shared_from_this() );
            ErrorShutdown(); // nie je skutocne chyba, ale funkcia zastavi socket a hrac sa validne zdestruuje
            return;
        }
        case Command::TEXT:
        {
            ErrorMessage("Player is not allowed to sent text message.");
            ErrorShutdown();
            return;
            break;
        }
        case Command::LEFT:
        case Command::RIGHT:
        case Command::GO:
        case Command::STOP:
        case Command::TAKE:
        case Command::OPEN:
        {
            if ( !IsInGame() )
            {
                ErrorMessage("Player is not in game. Cannot use game commands.");
                ErrorShutdown();
                return;
            }
            game->GameMessage( shared_from_this(), packet);
            break;
        }
        case Command::MAP_LIST:
            cout << "MAP_LIST is not implemented yet" << endl;
            break;
        case Command::ERROR_MESSAGE:
            ErrorMessage("Player is not allowed to sent error message.");
            ErrorShutdown();
            return;
            break;
        default:
        {
            ErrorMessage("Unknown command. Disconnecting...");
            ErrorShutdown();
            return;
        }
    }
    do_read();
}

void player::HandleCreateServer(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if ( error || bytes_transferred != inheader[1] )
    {
        cout << "Player [" << index_ << "] player::HandleCreateServer error=" << error << endl;
        ErrorShutdown();
        return;
    }

    ServerInfoCreate packet;
    Deserialize( packet );

    boost::shared_ptr<Game> new_game = Manager::instance().CreateGame(packet.newServer.name, packet.newServer.max, packet.newServer.timer, packet.newServer.timeout, packet.newServer.map);
    if ( !new_game )
    {
        ErrorMessage("Couldn't create game with specified parameters.");
        ErrorShutdown();
        return;
    }
    if ( ! new_game->Join( shared_from_this() ) )
    {
        ErrorMessage(std::string("Unable to join game. Unknown error.") );
        ErrorShutdown();
        return;
    }
    game = new_game;
    cout << "Player [" << index_ << "] created game "<< game->GetIndex() << endl;


    do_read();
}




bool player::IsInGame()
{
    return (game);
}


void player::handle_write(const boost::system::error_code& error)
{
    if (error)
    {
        cout << "Player [" << index_ << "] player::listen error=" << error << endl;
        ErrorShutdown();
        return;

    }
    cout << "Player [" << index_ << "] message sent" << endl;

    //timer->expires_from_now(boost::posix_time::seconds(1));
    //timer->async_wait( boost::bind( &player::do_write, shared_from_this(), boost::asio::placeholders::error ) );
}


template <class T, typename C>
std::vector<boost::asio::const_buffer> player::Serialize(const T &t, C code )
{
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << t;

    outheader[0] = code;
    outheader[1] = static_cast<uint32_t>(archive_stream.str().size()) ;
    outdata = archive_stream.str();

    SerializedData buffers;
    buffers.push_back(boost::asio::buffer(outheader));
    buffers.push_back(boost::asio::buffer(outdata));
    return buffers;
}

template <class T>
void player::Deserialize( T &t )
{
    std::string archive_data(&indata[0], indata.size());
    std::istringstream archive_stream(archive_data);
    boost::archive::text_iarchive archive(archive_stream);

    archive >> t;
}
