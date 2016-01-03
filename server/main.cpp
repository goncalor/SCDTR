/*

Creates a serial connection to the arduino and then a tcp server.

g++ -Wall -Wextra -pedantic -std=c++11 -g utils.cpp cpp_simplex/simplex.cpp tcp_server/tcp_server.cpp main.cpp -lpthread -lboost_system  -o teste_server.out

*/


#include "tcp_server/tcp_server.hpp"
#include "serial/serial.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

//#define BOOST_ASIO_ENABLE_BUFFER_DEBUGGING
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING


int main(int argc, char* argv[]){
    try{
        if (argc != 2){
          std::cerr << "Usage: async_tcp_echo_server <port>\n";
          return 1;
        }
        boost::asio::io_service io_service;
        Serial_connection sc(io_service, "/dev/ttyACM0", 19200);
        std::cout << "Connected to serial port" << std::endl;
        //sc.calibrate();
        //sc.send_reset();
        Tcp_server s(io_service, std::atoi(argv[1]), sc);
        std::cout << "Server Started" << std::endl;

        io_service.run();
    }catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }catch (int i){
        std::cerr << "Exception: Serial problem"<< std::endl;
    }

  return 0;
}
