#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include "player.h"
#include "message.h"
#include <string>



using boost::asio::ip::tcp;
using std::cout;
using std::endl;


int player::index = 0;


player::player(boost::asio::io_service& io_service): socket_(io_service) ,buff (std::string( "ICP serverrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr 2014\r\n") )
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
    cout << "Player [" << index_ << "] starting routine" << endl;
    //SKLAD[index_] = shared_from_this();
    boost::system::error_code error;
    do_write( error );
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
      boost::bind( &player::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
}


void player::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred )
{
    (void)bytes_transferred;
    if ( error )
    {
        cout << "Error : [" << index_ << "]" << "player::handle_read " << error << endl;
        if (shutdown) return;
        //SKLAD.erase(index_);
        shutdown = true;
        return;
    }
    cout << "Player [" << index_ << "] received data" << endl;
}


void player::handle_write(const boost::system::error_code& error)
{
    if (error)
    {
        cout << "Error : [" << index_ << "]" << "player::handle_write " << error << endl;
        if (shutdown) return;
        //SKLAD.erase(index_);
        shutdown = true;
        return;

    }
    cout << "Player [" << index_ << "] message sent" << endl;

    timer->expires_from_now(boost::posix_time::seconds(1));
    timer->async_wait( boost::bind( &player::do_write, shared_from_this(), boost::asio::placeholders::error ) );
}


/*************************************************/
std::map<int, boost::shared_ptr<player>> SKLAD;
