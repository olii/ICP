#ifndef NETWORK_H
#define NETWORK_H
#include <boost/asio.hpp>
#include <string>
#include "../shared/serverinfo.h"
#include "../shared/command.h"

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
    template<class T> T GetPacketContent();
    ServerInfoList GetServerList();
    ServerInfoList GetMapList();
    void CreateServer(std::string name, unsigned int max, std::string map, float timer, int timeout);

private:
    boost::asio::io_service io_service;
    std::string host;
    std::string port;
    tcp::socket socket_;
    bool failbit;

    uint32_t outheader[2];
    uint32_t inheader[2];
    std::string outdata;
    std::vector<char> indata;



    typedef std::vector<boost::asio::const_buffer> SerializedData;
    template<class T, typename C>
    SerializedData Serialize( const T&, C code );

    template <class T>
    void Deserialize( T& );


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
