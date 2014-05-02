#include "network.h"
#include <string>
#include <boost/asio.hpp>
#include <exception>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../shared/global.h"
#include "../shared/serverinfo.h"
#include "../shared/command.h"

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
    if (failbit)
    {
        throw NetworkException("Network error.");
    }
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
    t.id = id;
    t.SetType(Command::JOIN);

    SerializedData buffer = Serialize(t, t.HeaderCode::CODE);

    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

void Network::Leave()
{
    Command t;
    t.SetType(Command::LEAVE);

    SerializedData buffer = Serialize(t, t.HeaderCode::CODE);

    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

void Network::SendCommand(Command &c)
{
    SerializedData buffer = Serialize(c, c.HeaderCode::CODE);
    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
}

void Network::ReadPacket()
{
    boost::asio::read(socket_, boost::asio::buffer(inheader), boost::asio::transfer_exactly(packetHeader::header_size) );
    if ( (inheader[0] != packetHeader::SERVER_LIST) &&
         (inheader[0] != packetHeader::COMMAND) &&
         (inheader[0] != packetHeader::GAME_UPDATE) )
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
        if ( c.type == Command::ERROR_MESSAGE )
        {
            throw NetworkException(c.text);
        }
    }
}

packetHeader Network::GetHeaderType()
{
    return static_cast<packetHeader>(inheader[1]);
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
    Command c;
    c.SetType(Command::MAP_LIST);
    SerializedData buffer = Serialize(c, c.HeaderCode::CODE);

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

    SerializedData buffer = Serialize(t, t.HeaderCode::CODE);
    boost::asio::write(socket_, buffer, boost::asio::transfer_all());
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

    if (inheader[0] != packetHeader::COMMAND )
    {
        Shutdown();
        return false;
    }
    indata.resize(inheader[1]);

    boost::asio::read(socket_, boost::asio::buffer(indata), boost::asio::transfer_exactly(inheader[1]));

    Command t;
    Deserialize(t);

    if (t.type != Command::TEXT || t.text != std::string("ICP SERVER 2014 Bludisko\n") )
    {
        Shutdown();
        return false;
    }

    return true;
}

template<class T>
T Network::GetPacketContent()
{
    T t;
    Deserialize(t);
    return t;
}









template <class T, typename C>
std::vector<boost::asio::const_buffer> Network::Serialize(const T &t, C code )
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
void Network::Deserialize( T &t )
{
    std::string archive_data(&indata[0], indata.size());
    std::istringstream archive_stream(archive_data);
    boost::archive::text_iarchive archive(archive_stream);

    archive >> t;
}
