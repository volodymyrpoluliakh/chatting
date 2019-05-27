#include <server.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main_callback(int argc, char* argv[]) {
  po::options_description desc("Options");
  desc.add_options()
      ("help,h", "Print help messages")
      ("port,p", po::value<uint16_t>()->default_value(5000), "Servers port number");

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

  uint16_t port = opts["port"].as<uint16_t>();

  try {
    boost::asio::io_context io_context;
    bai::tcp::endpoint endpoint (bai::tcp::v4(), port);

    std::cout << "Running server on port:" << port << std::endl;
    Server server(io_context, endpoint);
    io_context.run();

  } catch (const std::exception& ce) {
    std::cout << "Exception occured while setuping server: "
              << typeid(ce).name() << " " << ce.what() << std::endl;
    return 1;
  } catch (...) {
    std::cout << "Unknown exception occured while setuping server" << std::endl;
    return 1;
  }

  return 0;
}

int main (int argc, char* argv[]) {
  return main_callback(argc, argv);
}
