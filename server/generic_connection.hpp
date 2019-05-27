#ifndef GENERIC_CONNECTION_HPP
#define GENERIC_CONNECTION_HPP

#include <../include/message.hpp>

#include <boost/asio.hpp>

class GenericConnection {
public:
  GenericConnection(boost::asio::ip::tcp::socket&& socket)
    : _socket(std::move(socket))
  {}

  virtual ~GenericConnection() = default;

  virtual void deliver(const std::string& _id, const Message& msg, bool is_kick_msg) = 0;

  std::string get_id() { return _id; }

protected:
  std::string _id;

  boost::asio::ip::tcp::socket _socket;
};

#endif // GENERIC_CONNECTION_HPP
