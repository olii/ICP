#ifndef NETWORK_H
#define NETWORK_H
#include <boost/asio.hpp>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../shared/serverinfo.h"
#include "../shared/command.h"
#include "../shared/map.h"

using boost::asio::ip::tcp;


class Network
{
public:
    Network(std::string hostP, std::string portP);
    ~Network();
    bool Connect();
    void Shutdown();
    bool Ready();
    void Join(uint32_t id);
    void Leave();
    void SendCommand(Command &c);
    void ReadPacket();
    packetHeader GetHeaderType();
    ServerInfoList GetServerList();
    ServerInfoList GetMapList();
    void CreateServer(std::string name, unsigned int max, std::string map, float timer, int timeout);
    Map GetMapByName(std::string name);

    uint32_t GetId();

    template<class T>
    T GetPacketContent()
    {
        T t;
        Deserialize(t);
        return t;
    }

private:
    boost::asio::io_service io_service;
    std::string host;
    std::string port;
    tcp::socket socket_;
    bool failbit;

    uint32_t id;

    uint32_t outheader[2];
    uint32_t inheader[2];
    std::string outdata;
    std::vector<char> indata;

    void SendHello();



    typedef std::vector<boost::asio::const_buffer> SerializedData;

    template <class T, typename C>
    std::vector<boost::asio::const_buffer> Serialize(const T &t, C code )
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
    void Deserialize( T &t )
    {
        std::string archive_data(&indata[0], indata.size());
        std::istringstream archive_stream(archive_data);
        boost::archive::text_iarchive archive(archive_stream);

        archive >> t;
    }


};

class NetworkException: public std::exception {
private:
    std::string message_;
public:
    NetworkException(const std::string& message) : message_(message)
    {    }

    virtual const char* what() const throw()
    {
        return message_.c_str();
    }

    virtual ~NetworkException() throw() {}

};

#endif // NETWORK_H
