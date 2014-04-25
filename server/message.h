#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>


class Message
{
public:
    Message();
    Message( const Message& );
    Message(std::string &str, uint32_t type = 65 );

    Message( std::string &&str, uint32_t type = 65 );

    void setMessage(std::vector<char> &tmp)
    {
        buffer = tmp;
    }

    void setString( std::string &&str )
    {
        *this = Message( str );
    }

    void setString( std::string &str )
    {
        *this = Message( str );
    }
    std::vector<char>& getMessage(  )
    {
        return buffer;
    }

    void resizePayload( unsigned int size )
    {
        buffer.resize( size + header_size );
    }

    void * getHeaderPtr()
    {
        return buffer.data();
    }


    void * getPayLoadPtr()
    {
        return buffer.data() + header_size;
    }

    uint32_t getType()
    {
        return *( reinterpret_cast< uint32_t* >( buffer.data() ) );
    }

    uint32_t getSize()
    {
        return *( reinterpret_cast< int32_t* >( buffer.data() ) +1 );
    }

    void setSize( uint32_t size )
    {
        *( reinterpret_cast< int32_t* >( buffer.data() ) + 1 ) = size;
    }

    void setType( uint32_t type )
    {
        *( reinterpret_cast< int32_t* >( buffer.data() ) ) = type;
    }

    Message & operator=( const Message &tmp ) // copy
    {
        this->buffer = tmp.buffer;
        return *this;
    }
    Message & operator=( const Message &&tmp ) // move
    {
        this->buffer = tmp.buffer;
        return *this;
    }

    uint32_t getHeaderSize()
    {
        return header_size;
    }

    std::string to_str()
    {
        return std::string( reinterpret_cast<char*>(getPayLoadPtr()), getSize() );
    }

    void dump()
    {
        uint8_t *ptr = reinterpret_cast<uint8_t*>(buffer.data());
        for( int i = 0; i<8; ++i )
        {
            std::cout << std::hex << int(ptr[i])  << " ";
        }
        std::cout << std::dec << std::endl;
    }


private:
    std::vector<char> buffer;
    enum { header_size = 2 * sizeof( uint32_t ) };
};

#endif // MESSAGE_H
