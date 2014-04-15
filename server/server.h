#ifndef SERVER_H
#define SERVER_H
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using std::cout;
using std::endl;

class player;
extern std::vector<boost::shared_ptr<player>> SKLAD;


class player: public boost::enable_shared_from_this<player>
{
public:
    player(boost::asio::io_service& io_service): socket_(io_service)
    {
        index_ = index++;
        cout << "Player [" << index_ << "] constructed" << endl;
    }
    ~player()
    {
        cout << "Player [" << index_ << "] destructed" << endl;
    }
    void start()
    {
        SKLAD.push_back( shared_from_this() );
        cout << "Player [" << index_ << "] starting routine" << endl;
    }
    
    
    static int index;
    tcp::socket socket_;
    
    
private:
    int index_;
};


class server
{
public:
    server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint);

private:
    void start_accept();
    void handle_accept(boost::shared_ptr<player> sharedPtr, const boost::system::error_code& error);

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
};






#endif // SERVER_H
