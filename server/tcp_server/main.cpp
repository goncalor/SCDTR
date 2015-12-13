#include "tcp_server.hpp"


#include <iostream>
#include <vector>


int main(int argc, char* argv[]){
  try{
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }
    boost::asio::io_service io_service;
    using namespace std; // For atoi.
    tcp_server s(io_service, atoi(argv[1]));
    io_service.run();
  }catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
