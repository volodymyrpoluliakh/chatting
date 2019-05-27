#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

#include <memory>
#include <deque>
#include <string>
#include <list>

#include <connection_manager.hpp>
#include <connection.hpp>

namespace bai = boost::asio::ip;

class Server {

public:
  Server(boost::asio::io_context& io_context, const bai::tcp::endpoint& endpoint)
    : _acceptor(io_context, endpoint),
      _manager(std::make_shared<ConnectionManager>())
  {
    std::cout << "Server started" << std::endl;
    do_accept();
  }

private:

  void do_accept() {
    _acceptor.async_accept(
          [this](boost::system::error_code ec, bai::tcp::socket socket)
    {
      if (!ec)
      {
        std::make_shared<Connection>(std::move(socket), _manager)->start();
      } else {
        std::cout << "Error occured while accepting connection:"
                  << " ec.value: " << ec.value()
                  << " ec.message: " << ec.message() << std::endl;
      }

      do_accept();
    });
  }

private:
  bai::tcp::acceptor _acceptor;
  std::shared_ptr<ConnectionManager> _manager;
};

#endif // SERVER_HPP
