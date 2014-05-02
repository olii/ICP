#ifndef COMMAND_H
#define COMMAND_H


#include "global.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>

class Command
{
public:
    enum HeaderCode { CODE = packetHeader::COMMAND }; // mandatory
    enum Type { JOIN, LEAVE, TEXT, LEFT, RIGHT, GO, STOP, TAKE, OPEN, MAP_LIST, ERROR_MESSAGE  };

    Command();
    Command( std::string txt);
    Command( Type typ );
    void SetType( Type typ );

    Type type;
    uint32_t id;
    std::string text;


private:
    /* serializacia */
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & type ;
        ar & id;
        ar & text;
    }
};

#endif // COMMAND_H
