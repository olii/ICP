/**
 * @file informator.h
 *
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#ifndef INFORMATOR_H
#define INFORMATOR_H

#include "../network.h"

#include <string>

#include <QMessageBox>

const int TEXTURE_SIZE = 512;

/**
 * @brief The Informator class
 *        navrhovy vzor Singleton predstavuje zapuzdrenie komunikacie so sietou
 */
class Informator
{
    public:
        static Informator * getInstance()
        {
            static Informator    instance; // Guaranteed to be destroyed.
                                  // Instantiated on first use.
            return &instance;
        }

        uint32_t getClientId();

        void Connect(std::string host, std::string port);

        void Disconnect();

        void Join(uint32_t Id);
        Network * GetConnection();

    private:
        Network * connection;
        Informator() {}

};

///< funkcia pre zobrazenie okna s hlasenim chyby
void ShowError(QString title, QString text, QMessageBox::Icon icon);

#endif // INFORMATOR_H
