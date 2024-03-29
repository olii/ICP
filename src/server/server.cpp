/** @file server.cpp
* @author Oliver Nemček
* @brief implementuje pracu asio servera
*
* Prebrate a mierne upravene z boost dokumentacie
*/

#include "server.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include "player.h"

server::server( boost::asio::io_service& io_service, const tcp::endpoint& endpoint ):
    io_service_( io_service ), acceptor_(io_service, endpoint)
{
    start_accept();
}


/**
 * @brief Metoda spusta asynchronne cakanie na sockete
 */
void server::start_accept()
{
    boost::shared_ptr<player> sharedPtr( new player( io_service_ ) );
    
    acceptor_.async_accept(sharedPtr->socket_,
        boost::bind(&server::handle_accept, this, sharedPtr,
          boost::asio::placeholders::error));
}


/**
 * @brief definuje ako co sa ma robit po akceptacii noveho spojenia
 * @param[in] sharedPtr zdielany pointer na novo vytvoreneho hraca
 * @param error boost::asio chyba, nastavena v pripade chyby
 */
void server::handle_accept(boost::shared_ptr<player> sharedPtr,
    const boost::system::error_code& error)
{
  if (!error)
  {
    sharedPtr->start();
  }

  start_accept();
}
