#include "message.h"
#include <iterator>     // std::back_inserter
#include <vector>       // std::vector
#include <algorithm>    // std::copy
#include <string>

Message::Message()
{
    buffer.resize(128);
}

Message::Message( std::string &str )
{
    buffer.resize(str.length() + header_size);
    setSize(str.length());
    std::copy( str.begin(), str.end(), back_inserter(buffer) );
    setType(65);
}

Message::Message(std::string &&str )
{
    buffer.resize(str.length() + header_size);
    setSize(str.length());
    std::copy( str.begin(), str.end(), back_inserter(buffer) );
    setType(65);
}
