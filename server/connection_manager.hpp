#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <memory>
#include <map>
#include <set>
#include <iostream>

#include <sqlite3.h>

#include <generic_connection.hpp>

class ConnectionManager {

public:
  void deliver(const std::string& id, const Message& msg);

  bool check_credentials(const std::string& id, const std::vector<char>& hash);
  void add_connection(const std::string& id, std::shared_ptr<GenericConnection> connection);
  void remove_connection(const std::string& id);

  std::string check_get_kick_msg(const std::string& id, const Message& msg);

  void add_to_temp_members(std::shared_ptr<GenericConnection> connection);
  void remove_from_temp_members(std::shared_ptr<GenericConnection> connection);

  std::map<std::string, std::shared_ptr<GenericConnection>> _members;
  std::set<std::shared_ptr<GenericConnection>> _temp_members;
};

#endif
