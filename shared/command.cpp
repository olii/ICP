#include "command.h"
#include <string>

Command::Command():id(0)
{

}

Command::Command(std::string txt): id(0)
{
    text = txt;
    type = Type::TEXT;
}

Command::Command(Command::Type typ )
{
    type = typ;
    text.clear();
}

void Command::SetType(Command::Type typ)
{
    type = typ;
}
