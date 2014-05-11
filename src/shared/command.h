/** @file command.h
* @author Oliver Nemček
* @brief Definicia typu paketu Command
*
* Serializovany typ spravy Command
* pouziva sa prevazne pre rychle prenesenie sprav zo servera
* ktore nevyzaduju zlozite parametre
*/

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
    enum Type { JOIN, LEAVE, TEXT, LEFT, RIGHT, GO, STOP, TAKE, OPEN, MAP_LIST, ERROR_MESSAGE, GET_MAP  };

    Command();
    Command( std::string txt);
    Command( Type typ );
    void SetType( Type typ );

    uint32_t GetId();
    void SetId( uint32_t );
    Type GetType();
    std::string GetText();


private:
    Type type;
    uint32_t id;
    std::string text;
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
