#include <connection_manager.hpp>

#include <boost/algorithm/string.hpp>

namespace {
  const auto HASH_SIZE = 16;
  const auto MAX_ID_SIZE = 16;

  std::string_view pass1 = "password12345678";
  std::string_view pass2 = "12345678password";

  const std::map<std::string, const std::vector<char>> credentials =
  {
    { "Admin", {std::begin(pass2), std::end(pass2)}},
    { "User1", {std::begin(pass1), std::end(pass1)}},
    { "User1", {std::begin(pass1), std::end(pass1)}}
  };

  const std::string ADMIN = "Admin";
  const std::string KICK  = "kick";
}

void ConnectionManager::add_to_temp_members(std::shared_ptr<GenericConnection> conn) {
  _temp_members.insert(conn);
}

void ConnectionManager::remove_from_temp_members(std::shared_ptr<GenericConnection> conn) {
  _temp_members.erase(conn);
}

bool ConnectionManager::check_credentials(const std::string& id, const std::vector<char>& hash) {
  if (hash.size() != HASH_SIZE && id.size() > MAX_ID_SIZE) {
    std::cout << "Login failed! Credentials are incorrect: id=" << id
              << " hash = " << std::string(hash.data(), hash.size())
              << std::endl;
    return false;
  }

  if (auto cred_el = credentials.find(id); cred_el != std::end(credentials)) {
    auto cred_it = std::begin(cred_el->second);
    auto hash_it = std::begin(hash);
    for (; hash_it != std::end(hash); ++cred_it, ++hash_it) {
      if (*hash_it != *cred_it) {
        std::cout << "Login failed! Credentials are incorrect: id=" << id
                  << " hash = " << std::string(hash.data(), hash.size())
                  << std::endl;
        return false;
      }
    }
  }

  if (_members.find(id) != std::end(_members)) {
    std::cout << " Connection of the new chat member with id=" << id << " refused! "
              << "Member with such id has been already created!" << std::endl;
    return false;
  }

  return true;
}

void ConnectionManager::add_connection(const std::string& id, std::shared_ptr<GenericConnection> connection) {
  _members.insert({id, connection});
  std::string connect_msg(id + ": connected to chat!");
  std::cout << connect_msg << '\n';
  for (auto [m_id, m_connection] : _members) {
    m_connection->deliver(id, connect_msg, false);
  }
}


void ConnectionManager::remove_connection(const std::string &id) {
  if (auto cred_el = _members.find(id); cred_el == std::end(_members)) {
    std::cout << "Unable to kick member with id=" << id << " : no member with this id found" << std::endl;
    return;
  }

  std::string kick_msg (id + ": kicked from chat!");

  for (auto [m_id, m_connection] : _members) {
    m_connection->deliver(id, kick_msg, true);
  }

  _members.erase(id);
}

void ConnectionManager::deliver(const std::string &id, const Message &msg) {
  if (auto id_to_remove = check_get_kick_msg(id, msg); !id_to_remove.empty()) {
    remove_connection(id_to_remove);
    return;
  }

  Message marked_message = '[' + id + "]: " + msg.get_string();
  for (auto [m_id, m_connection] : _members) {
    m_connection->deliver(id, marked_message, false);
  }
}

std::string ConnectionManager::check_get_kick_msg(const std::string& id, const Message& msg) {
  if (id != ADMIN) {
    return std::string();
  }

  std::string msg_str = msg.get_string();
  std::vector<std::string> splitted_msg;

  boost::split(splitted_msg, msg_str, boost::is_any_of(" "));

  if (splitted_msg.size() == 2 && splitted_msg.front() == KICK && !splitted_msg.back().empty()) {
    return splitted_msg.back();
  }

  return std::string();
}
