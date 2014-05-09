#include "informator.h"

uint32_t Informator::getClientId()
{
    return connection->GetId();
}

void Informator::Connect(std::string host, std::string port)
{
    connection = new Network(host, port);
    connection->Connect();
}

void Informator::Join(uint32_t Id)
{
    connection->Join(Id);
}

void Informator::Disconnect()
{
    delete connection;
}

Network * Informator::GetConnection()
{
    return connection;
}

void ShowError(QString title, QString text, QMessageBox::Icon icon)
{
    QMessageBox Error;
    Error.setWindowTitle(title);
    Error.setText(text);
    Error.setIcon(icon);
    Error.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    Error.exec();
}
