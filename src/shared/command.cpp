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
    id = 0;
}

void Command::SetType(Command::Type typ)
{
    this->type = typ;
}

uint32_t Command::GetId()
{
    return id;
}

void Command::SetId(uint32_t id)
{
    this->id = id;
}

Command::Type Command::GetType()
{
    return type;
}

std::string Command::GetText()
{
    return text;
}
