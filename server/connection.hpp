#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>

#include <queue>
#include <atomic>

#include <generic_connection.hpp>
#include <connection_manager.hpp>

namespace {
  const auto LOGIN_MSG_SIZE = 40; // 8 bytes header, 16 bytes id, 16 bytes AES hash of id using password as key
  const auto ID_SIZE        = 16;
}


namespace bai = boost::asio::ip;

using HandlerType = std::function<void(const std::string id, const std::string& msg)>;

class Connection :
    public GenericConnection,
    public std::enable_shared_from_this<Connection> {
public:
  Connection(bai::tcp::socket&& socket, std::shared_ptr<ConnectionManager> manager)
    : GenericConnection(std::move(socket)),
      _manager(manager),
      _is_kicked(false)
  { }

  virtual ~Connection() = default;

  void deliver(const std::string& id, const Message& msg, bool is_kick_msg);

  void start();

private:
  void read_login_message();
  void do_read_header();
  void do_read_body();

  void do_write();



private:
  Message _read_message;
  std::queue<Message> _write_msgs;
  std::shared_ptr<ConnectionManager> _manager;
  std::atomic_bool _is_kicked;
};

#endif // CONNECTION_HPP
