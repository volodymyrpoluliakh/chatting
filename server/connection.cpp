#include <connection.hpp>

#include <iostream>

void Connection::start() {
  _manager->add_to_temp_members(shared_from_this());
  _read_message.body_length(LOGIN_MSG_SIZE - HEADER_SIZE);
  read_login_message();
}

void Connection::read_login_message() {
  auto self = shared_from_this();
  boost::asio::async_read(_socket,
    boost::asio::buffer(_read_message.data(), LOGIN_MSG_SIZE),
    [this, self](boost::system::error_code ec, size_t bytes)
  {
    _manager->remove_from_temp_members(shared_from_this());
    if (!ec && bytes == LOGIN_MSG_SIZE) {
      std::string id = _read_message.body()[ID_SIZE - 1] == '\0'
          ? _read_message.body() : std::string(_read_message.body(), ID_SIZE);

      std::vector<char> hash = std::vector<char>(std::begin(_read_message.get_vector()) + HEADER_SIZE + ID_SIZE,
                                                 std::end(_read_message.get_vector()));

      if (_manager->check_credentials(id, hash)) {
        _id = id;
        _manager->add_connection(id, self);
        do_read_header();
      }
    } else {
      std::cout << "Error occured while reading login msg from client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;
      _socket.close();
    }

  });
}

void Connection::do_read_header() {
  auto self(shared_from_this());
  boost::asio::async_read(_socket,
    boost::asio::buffer(_read_message.data(), HEADER_SIZE),
    [this, self](boost::system::error_code ec, size_t )
  {
    if (!ec) {
      auto size = _read_message.get_body_length();
      _read_message.body_length(size);
      do_read_body();
    } else {
      std::cout << "Error occured while reading header msg from client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;

      _manager->remove_connection(_id);
      _socket.close();
    }

  });
}

void Connection::do_read_body() {
  auto self(shared_from_this());
  boost::asio::async_read(_socket,
    boost::asio::buffer(_read_message.data() + HEADER_SIZE, _read_message.get_body_length()),
    [this, self](boost::system::error_code ec, size_t )
  {
    if (!ec) {
      _manager->deliver(_id, _read_message);
      do_read_header();
    } else {
      std::cout << "Error occured while reading body msg from client id:" << _id
                << " ec.value: " << ec.value()
                << " ec.message: " << ec.message() << std::endl;

      _manager->remove_connection(_id);
      _socket.close();
    }
  });
}

void Connection::do_write() {
  auto self(shared_from_this());
  if (_is_kicked) {
    try {
      Message msg = std::string("You have been kicked form chat!");
      boost::asio::async_write(_socket,
        boost::asio::buffer(msg.data(), msg.length()),
        [this , self](boost::system::error_code ec, std::size_t )
      {
        if (!ec) {
          std::cout << "Client with id:" << _id << " recieved his kick message" << std::endl;
        } else {
          std::cout << "Client with id:" << _id << " did not recieve his kick message" << std::endl;
        }

        _socket.close();
      });


    } catch (const boost::system::system_error& ce) {
      std::cout << "Exception occured while sending kick message to client with"
                << " id:" << _id
                << ", code:" << ce.code() << " " << ce.what() << std::endl;
    } catch (...) {
      std::cout << "Exception occured while sending kick message to client with id:" << _id << std::endl;
    }
    return;
  }

  boost::asio::async_write(_socket,
    boost::asio::buffer(_write_msgs.front().data(), _write_msgs.front().length()),
                       [this , self](boost::system::error_code ec, std::size_t )
  {
     if (!ec) {
       _write_msgs.pop();
       if (!_write_msgs.empty()) {
         do_write();
       }
     } else {
       _socket.close();
       _manager->remove_connection(_id);
    }
  });
 }

void Connection::deliver(const std::string &id, const Message &msg, bool is_kick_msg) {
  if (is_kick_msg && id == _id) {
    _is_kicked.store(true);
    do_write();
  } else {
    bool write_in_progress = !_write_msgs.empty();
    _write_msgs.push(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }
}
