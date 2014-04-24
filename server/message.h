#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>
#include <cstdint>


class Message
{
public:
    Message();
    Message(std::string &str );

    Message( std::string &&str );

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

    void resize( unsigned int size )
    {
        buffer.resize(size);
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
        return *( reinterpret_cast< int32_t* >( buffer.data() ) );
    }

    uint32_t getSize()
    {
        return *( reinterpret_cast< int32_t* >( buffer.data() + sizeof( uint32_t ) ) );
    }

    void setSize( uint32_t size )
    {
        *( reinterpret_cast< int32_t* >( buffer.data() + sizeof( uint32_t ) ) ) = size;
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

    int getHeaderSize()
    {
        return header_size;
    }


private:
    std::vector<char> buffer;
    enum { header_size = 2 * sizeof( uint32_t ) };
};

#endif // MESSAGE_H
