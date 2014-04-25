#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <string>
#include "player.h"
#include "message.h"
#include "manager.h"



using boost::asio::ip::tcp;
using std::cout;
using std::endl;


uint32_t player::index = 0;


player::player(boost::asio::io_service& io_service): socket_(io_service) ,buff (std::string( "ICP server v0.1\r\n") )
{
    timer = new boost::asio::deadline_timer(io_service, boost::posix_time::seconds(2));
    index_ = index++;
    cout << "Player [" << index_ << "] constructed" << endl;
}


player::~player()
{
    timer->cancel();
    delete timer;
    if (socket_.is_open())
    {
        socket_.cancel();
        socket_.close();
    }
    cout << "Player [" << index_ << "] destructed" << endl;
}


void player::start()
{
    game = Manager::instance().GetJoinableGame();
    if( !game )
    {
        cout << "Player [" << index_ << "] : no joinable game, must create one." << endl;
        return;
    }
    game->Join( shared_from_this() );
    cout << "Player [" << index_ << "] starting routine" << endl;
    //SKLAD[index_] = shared_from_this();
    boost::system::error_code error;


    /* posle sa hello sprava */
    boost::asio::async_write( socket_, boost::asio::buffer(buff.getMessage() ),
        boost::bind( &player::listen, shared_from_this(), boost::asio::placeholders::error ) );

    //do_write( error );
}

uint32_t player::GetIndex()
{
    return index_;
}


void player::listen( const boost::system::error_code& error )
{
    if ( error )
    {
        cout << "Error : [" << index_ << "]" << "player::listen " << error << endl;
        if (shutdown) return;
        shutdown = true;
        if (game->Joined( shared_from_this() ) )
        {
            game->Leave( shared_from_this() );
        }
        //SKLAD.erase(index_);
        return;
    }

    do_read();
}


void player::do_write( const boost::system::error_code& error )
{
    if ( error )
    {
        cout << "Error : [" << index_ << "]" << "player::do_write " << error << endl;
        if (shutdown) return;
        shutdown = true;
        //SKLAD.erase(index_);
        return;
    }

    boost::asio::async_write(socket_, boost::asio::buffer(buff.getMessage()),
        boost::bind( &player::handle_write, shared_from_this(), boost::asio::placeholders::error) );
}


void player::do_read()
{
    boost::asio::async_read(socket_, boost::asio::buffer(buff.getMessage()), boost::asio::transfer_exactly( buff.getHeaderSize() ),
      boost::bind( &player::handle_header, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
}


void player::handle_header(const boost::system::error_code& error, std::size_t bytes_transferred )
{
    if ( error || bytes_transferred != buff.getHeaderSize() )
    {
        cout << "Error : [" << index_ << "]" << "player::handle_header " << error << endl;
        if (shutdown) return;
        //SKLAD.erase(index_);
        shutdown = true;
        if (game->Joined( shared_from_this() ) )
        {
            game->Leave( shared_from_this() );
        }
        return;
    }

    cout << "Player [" << index_ << "] received header with payload size "<< buff.getSize() << endl;
    cout << "read " << bytes_transferred << "bytes" <<endl;
    buff.dump();

    if ( buff.getSize() > 0 )
    {
        buff.resizePayload( buff.getSize() );
        boost::asio::async_read(socket_, boost::asio::buffer(buff.getPayLoadPtr(),buff.getSize() ), boost::asio::transfer_exactly( buff.getSize() ),
          boost::bind( &player::handle_payload, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
    }

}

void player::handle_payload( const boost::system::error_code& error, std::size_t bytes_transferred )
{
    if ( error || bytes_transferred != buff.getSize() )
    {
        cout << "Error : [" << index_ << "]" << "player::handle_payload " << error << endl;
        if (shutdown) return;
        //SKLAD.erase(index_);
        shutdown = true;
        if (game->Joined( shared_from_this() ) )
        {
            game->Leave( shared_from_this() );
        }
        return;
    }
    cout << "Player [" << index_ << "] received payload with size "<< buff.getSize() << endl;

    cout << ">>>>>>>" << buff.to_str() ;
    cout << "<<<<<<<" << endl;
    do_read();
}


void player::handle_write(const boost::system::error_code& error)
{
    if (error)
    {
        cout << "Error : [" << index_ << "]" << "player::handle_write " << error << endl;
        if (shutdown) return;
        //SKLAD.erase(index_);
        shutdown = true;
        if (game->Joined( shared_from_this() ) )
        {
            game->Leave( shared_from_this() );
        }
        return;

    }
    cout << "Player [" << index_ << "] message sent" << endl;

    timer->expires_from_now(boost::posix_time::seconds(1));
    timer->async_wait( boost::bind( &player::do_write, shared_from_this(), boost::asio::placeholders::error ) );
}


/*************************************************/
std::map<int, boost::shared_ptr<player>> SKLAD;
