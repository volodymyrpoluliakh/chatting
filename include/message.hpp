#include <vector>
#include <cstring>
#include <string>
#include <stdexcept>

namespace {
  const auto HEADER_SIZE = 8;
}

class Message
{
public:
Message() {
  _data.resize(HEADER_SIZE);
}

Message(const Message& message) = default;

Message(Message&& message) = default;

Message(const std::string& msg) {
  _data.resize(HEADER_SIZE + msg.size());
  std::copy(std::begin(msg), std::end(msg), std::begin(_data) + HEADER_SIZE);

  const auto size = msg.size();
  std::memcpy(&_data.front(), &size, HEADER_SIZE);
}

std::string get_string() const {
  return std::string (body(), get_body_length());
}

inline const char* data() const {
  return _data.data();
}

inline char* data() {
  return _data.data();
}

inline std::size_t length() const {
  return _data.size();
}

inline const char* body() const {
  return _data.data() + HEADER_SIZE;
}

inline char* body() {
  return _data.data() + HEADER_SIZE;
}

inline size_t get_body_length() const {
  if (_data.size() < HEADER_SIZE) {
    return 0;
  }

  size_t size = {};
  std::memcpy(&size, _data.data(), HEADER_SIZE);

  return size;
}

void body_length(std::size_t new_length) {
  _data.reserve(HEADER_SIZE + new_length);
  _data.resize(HEADER_SIZE + new_length);

  size_t body_size = _data.size() - HEADER_SIZE;
  std::memcpy(&_data.at(0), &body_size, HEADER_SIZE);
}

inline char& operator[](size_t size) {
  return _data[size];
}

inline std::vector<char>& get_vector() {
  return _data;
}

bool decode_header()
{
  if (_data.size() < HEADER_SIZE) {
    return false;
  }

  size_t size = {};
  std::memcpy(&_data.at(0), &size, HEADER_SIZE);
  _data.resize(HEADER_SIZE + size);

  return true;
}

void encode_message(const std::string& message)
{
  _data.resize(HEADER_SIZE + message.size());
  size_t body_size = message.size();
  std::memcpy(&_data.front(), &body_size, HEADER_SIZE);
}

private:
  std::vector<char> _data;
};
