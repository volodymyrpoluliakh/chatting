#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstdlib>
#include <queue>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <../include/message.hpp>

using boost::asio::ip::tcp;

namespace {
  const auto LOGIN_MESSAGE_BODY_LENGTH = 32;
}

class Client
{
public:
  Client(boost::asio::io_context& io_context,
         const tcp::resolver::results_type& endpoints,
         const std::string& id,
         const std::string& password)
    : _io_context(io_context),
      _socket(io_context),
      _id(id),
      _password(password)
  {
    if (_id.empty()       || _id.size()       > LOGIN_MESSAGE_BODY_LENGTH/2 ||
        _password.empty() || _password.size() > LOGIN_MESSAGE_BODY_LENGTH/2  ) {
      throw std::runtime_error("Invalid credentials given. Unable to create client with" \
                               " id:" + id + " password:" + password);
    }

    do_connect(endpoints);
  }

public:
  void write(const Message& msg);
  void close();

private:
  void do_connect(const tcp::resolver::results_type& endpoints);
  void do_send_login_msg();
  void do_read_header();
  void do_read_body();
  void do_write();

  Message create_login_message();
  std::string get_hash();

private:
  boost::asio::io_context& _io_context;
  tcp::socket _socket;
  Message _read_message;
  std::queue<Message> _write_msgs;
  const std::string _id;
  const std::string _password;
};

#endif // CLIENT_HPP
