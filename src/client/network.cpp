#include "network.h"
#include <string>
#include <boost/asio.hpp>
#include <exception>

#include "../shared/global.h"
#include "../shared/serverinfo.h"
#include "../shared/command.h"
#include "../shared/map.h"

Network::Network(std::string hostP, std::string portP): host(hostP), port(portP), socket_(io_service)
{
    failbit = false;
}
Network::~Network()
{
    Shutdown();
}

void Network::Shutdown()
{
    if (!failbit)
    {
        if ( socket_.is_open() )
        {
            boost::system::error_code ignored;
            socket_.shutdown( tcp::socket::shutdown_both, ignored );
            socket_.close(ignored);
            failbit = true;
        }
    }
}

bool Network::Ready()
{
    static int counter = 0;
    if (failbit || !socket_.is_open())
    {
        throw NetworkException("Network error.");
    }
    if ( counter ==  2048 ) // hack, heartbeat
    {
        counter = 0;
        SendHello();
    }
    ++counter;

    size_t ready = socket_.available();
    if (ready >= packetHeader::header_size)
    {
        return true;
    }
    return false;
}

void Network::Join( uint32_t id )
{
    Command t;
    t.SetId( id);
    t.SetType(Command::JOIN);

    SerializedData buffer = Serialize(t, Command::CODE);

    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

void Network::Leave()
{
    Command t;
    t.SetType(Command::LEAVE);

    SerializedData buffer = Serialize(t, Command::CODE);

    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

void Network::SendCommand(Command &c)
{
    SerializedData buffer = Serialize(c, Command::CODE);
    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

void Network::ReadPacket()
{
    boost::asio::read(socket_, boost::asio::buffer(inheader), boost::asio::transfer_exactly(packetHeader::header_size) );
    if ( (inheader[0] != packetHeader::SERVER_LIST) &&
         (inheader[0] != packetHeader::COMMAND) &&
         (inheader[0] != packetHeader::STATIC_MAP) &&
         (inheader[0] != packetHeader::GAME_UPDATE) &&
         (inheader[0] != packetHeader::HELLO) )
    {
        throw NetworkException("Network error: Unknown packet header.");
    }
    if (inheader[1] == 0)
    {
        return;
    }
    indata.resize(inheader[1]);
    boost::asio::read(socket_, boost::asio::buffer(indata), boost::asio::transfer_exactly(inheader[1]));

    /*command error ukonci spojenie, vyvola sa vynimk*/
    if (inheader[0] == packetHeader::COMMAND)
    {
        Command c;
        Deserialize(c);
        if ( c.GetType() == Command::ERROR_MESSAGE )
        {
            throw NetworkException(c.GetText());
        }
    }
}

packetHeader Network::GetHeaderType()
{
    return static_cast<packetHeader>(inheader[0]);
}

ServerInfoList Network::GetServerList()
{
    outheader[0] = packetHeader::SERVER_LIST;
    outheader[1] = 0;

    boost::asio::write(socket_, boost::asio::buffer(outheader), boost::asio::transfer_all());

    ReadPacket();
    if ( GetHeaderType() != packetHeader::SERVER_LIST )
    {
        throw NetworkException("Bad answer from server. Expected server list.");
    }
    ServerInfoList t;
    Deserialize(t);
    return t;

}

ServerInfoList Network::GetMapList()
{
    Command c(Command::MAP_LIST);

    SerializedData buffer = Serialize(c, Command::CODE);

    boost::asio::write(socket_, buffer, boost::asio::transfer_all());

    ReadPacket();

    if ( GetHeaderType() != packetHeader::SERVER_LIST )
    {
        throw NetworkException("Bad answer from server. Expected map list.");
    }
    ServerInfoList t;
    Deserialize(t);
    return t;
}

MapUpdate Network::GetMapUpdate()
{
    ReadPacket();
    MapUpdate t;
    if ( GetHeaderType() != packetHeader::GAME_UPDATE )
    {
        throw NetworkException("Bad answer from server. Expected update packet.");
    }
    Deserialize(t);
    return t;
}

Map Network::GetMapByName(std::string name)
{
    Command c(name);
    c.SetType(Command::GET_MAP);

    SerializedData buffer = Serialize(c, Command::CODE);

    boost::asio::write(socket_, buffer, boost::asio::transfer_all());

    ReadPacket();

    if ( GetHeaderType() != packetHeader::STATIC_MAP )
    {
        throw NetworkException("Bad answer from server. Expected map .");
    }
    Map t;
    Deserialize(t);
    return t;
}

void Network::CreateServer(std::string name, unsigned int max, std::string map, float timer, int timeout)
{
    ServerInfoCreate t;
    t.newServer.name = name;
    t.newServer.max = max;
    t.newServer.map= map;
    t.newServer.timer = timer;
    t.newServer.timeout = timeout;

    t.newServer.id = 0;
    t.newServer.playing = 0;

    SerializedData buffer = Serialize(t, ServerInfoCreate::CODE);
    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

uint32_t Network::GetId()
{
    return id;
}

void Network::SendHello()
{
    outheader[0] = packetHeader::HELLO;
    outheader[1] = packetHeader::HELLO;
    try
    {
        boost::asio::write(socket_, boost::asio::buffer(outheader), boost::asio::transfer_all());
    }
    catch (...)
    {
        throw NetworkException("Network disconnected.");
    }
}

bool Network::Connect()
{
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host, port);

    boost::asio::ip::tcp::resolver::iterator dest = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *dest;

    socket_.connect(endpoint);
    // vyhodi vynimku ak sa nepodari


    boost::asio::read(socket_, boost::asio::buffer(inheader), boost::asio::transfer_exactly(packetHeader::header_size) );

    if (inheader[0] != packetHeader::HELLO )
    {
        // vyhodi vynimku TODO
        Shutdown();
        return false;
    }
    indata.resize(inheader[1]);

    boost::asio::read(socket_, boost::asio::buffer(indata), boost::asio::transfer_exactly(inheader[1]));

    Command t;
    Deserialize(t);

    if (t.GetType() != Command::TEXT || t.GetText() != std::string("ICP SERVER 2014 Bludisko\n") )
    {
        Shutdown();
        return false;
    }
    id = t.GetId();

    return true;
}

