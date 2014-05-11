/** @file command.cpp
* @author Oliver Nemček
* @brief Sprava Command
*
* Implementacia niektorych operacii nad spravou command
*/

#include "command.h"
#include <string>



/**
 * @brief defaultny konstruktor, musi byt pritomny z dovodu serializacie
 */
Command::Command():id(0)
{

}

/**
 * @brief konstruktor ktory vytvori spravu typu TEXT
 * @param txt string s textom spravy
 */
Command::Command(std::string txt): id(0)
{
    text = txt;
    type = Type::TEXT;
}

/**
 * @brief konstruktor ktory vytvori obecnu spravu zadaneho typu
 * @param typ enumeracia s typom spravy
 */
Command::Command(Command::Type typ )
{
    type = typ;
    text.clear();
    id = 0;
}

/**
 * @brief nastavi pozadovany typ spravy, nizkourovnovy pristup
 * @param typ typ spravy
 */
void Command::SetType(Command::Type typ)
{
    this->type = typ;
}

/**
 * @brief metoda vrati id, ktore mooze byt obsahom spravy
 */
uint32_t Command::GetId()
{
    return id;
}

/**
 * @brief nastavi id spravy
 * @param id nastavovane id
 */
void Command::SetId(uint32_t id)
{
    this->id = id;
}

/**
 * @brief metoda vrati typ spravy typu command
 *
 */
Command::Type Command::GetType()
{
    return type;
}

/**
 * @brief niekedy sprava obsahuje textovy retazec, tato metoda ho vrati
 */
std::string Command::GetText()
{
    return text;
}
