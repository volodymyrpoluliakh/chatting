#include <client.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main_callback(int argc, char* argv[]) {
  po::options_description desc("Options");
  desc.add_options()
      ("help,h", "Print help messages")
      ("host,h", po::value<std::string>()->default_value("127.0.0.1"), "Server`s host address")
      ("port,", po::value<std::string>()->default_value("5000"), "Server`s port number")
      ("id,i", po::value<std::string>()->default_value("Admin"), "Server`s port number")
      ("password,", po::value<std::string>()->default_value("password1234578"), "Server`s port number");

  po::variables_map opts;

  try {
    po::store(po::parse_command_line(argc, argv, desc), opts);

    if (opts.count("help")) {
        desc.print(std::cout);
        return 1;
    }

  } catch (const std::exception& ce) {
    std::cout << "Exception occured while parsing commanf line arguments: "
              << typeid (ce).name() << " " << ce.what() << std::endl;
    return 1;
  } catch (...) {
    std::cout << "Unknown exception occured while parsing commanf line arguments" << std::endl;
  }

  std::string host     = opts["host"].as<std::string>();
  std::string port     = opts["port"].as<std::string>();
  std::string id       = opts["id"].as<std::string>();
  std::string password = opts["password"].as<std::string>();

  try {

    boost::asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);
    Client client(io_context, endpoints, id, password);

    std::thread t([&io_context](){ io_context.run(); });

    std::string client_message;
    client_message.reserve(10000);
    while (std::getline(std::cin, client_message))
    {
      Message msg = client_message;
      client.write(msg);
    }

    client.close();
    t.join();

  } catch (const std::exception& ce) {
    std::cout << "Exception occured while setuping client: "
              << typeid(ce).name() << " " << ce.what() << std::endl;
    return 1;
  } catch (...) {
    std::cout << "Unknown exception occured while setuping client" << std::endl;
    return 1;
  }

  return 0;
}

int main (int argc, char* argv[]) {
  return main_callback(argc, argv);
}
