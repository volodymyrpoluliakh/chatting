#include <client.hpp>

std::string Client::get_hash() {
  // TODO replace this function with cryptographic hash function HASH(_id, _password)
  return _password;
}

void Client::close() {
  boost::asio::post(_io_context, [this]() { _socket.close(); });
}

void Client:: do_connect(const tcp::resolver::results_type& endpoints)
{
  boost::asio::async_connect(_socket, endpoints,
    [this](boost::system::error_code ec, tcp::endpoint)
  {
    if (!ec)
    {
      do_send_login_msg();
    } else {
      std::cout << "Error occured while connection to server of client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;
    }
  });
}

Message Client::create_login_message() {
  Message msg;
  msg.body_length(LOGIN_MESSAGE_BODY_LENGTH);
  std::copy(std::begin(_id), std::end(_id), msg.data() + HEADER_SIZE);
  std::copy(std::begin(_password), std::end(_password),
            msg.data() + HEADER_SIZE + LOGIN_MESSAGE_BODY_LENGTH/2);
  return msg;
}

void Client::do_send_login_msg() {
  auto login_msg = create_login_message();
  boost::asio::async_write(_socket,
    boost::asio::buffer(login_msg.data(),
    LOGIN_MESSAGE_BODY_LENGTH + HEADER_SIZE),
    [this](boost::system::error_code ec, std::size_t /*length*/)
  {
    if (!ec) {
      do_read_header();
    } else {
      std::cout << "Error occured while sending login message of client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;
      _socket.close();
    }
  });
}

void Client::do_read_header()
{
  boost::asio::async_read(_socket,
    boost::asio::buffer(_read_message.data(), HEADER_SIZE),
    [this](boost::system::error_code ec, std::size_t )
  {
    if (!ec) {
      try {
        auto size = _read_message.get_body_length();
        _read_message.body_length(size);
      } catch (const std::exception& ce) {
        std::cout << "do_read_header Exception: " << typeid(ce).name() << " " << ce.what() << std::endl;
      }

      do_read_body();
    } else {
      std::cout << "Error occured while reading header msg from server on client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;

      _socket.close();
    }
  });
}

void Client::do_read_body()
{
  boost::asio::async_read(_socket,
    boost::asio::buffer(_read_message.data() + HEADER_SIZE, _read_message.get_body_length()),
    [this](boost::system::error_code ec, std::size_t )
  {
    if (!ec)
    {
      std::cout << std::string_view(_read_message.body(), _read_message.get_body_length()) << std::endl;
      do_read_header();
    } else {
      std::cout << "Error occured while reading body msg from server on client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;
      _socket.close();
    }
  });
}

void Client::write(const Message& msg) {
  boost::asio::post(_io_context,
    [this, msg]()
  {
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  });
}

void Client::do_write()
{
  boost::asio::async_write(_socket,
    boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
    [this](boost::system::error_code ec, std::size_t /*length*/)
  {
    if (!ec) {
      _write_msgs.pop();
      if (!_write_msgs.empty()) {
        do_write();
      }
    } else {
      std::cout << "Error occured while reading body msg from server on client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;
      _socket.close();
    }
  });
}
