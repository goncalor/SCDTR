//g++ -Wall -Wextra -pedantic -std=c++11 -g main.cpp -lpthread -lboost_system  -o serial.out

#include "serial.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#define BOOST_ASIO_ENABLE_BUFFER_DEBUGGING
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

int main(){
  try{

    boost::asio::io_service io_service;
    serial_connection sc(io_service, "/dev/ttyACM0", 38400);
    //std::this_thread::sleep_for (std::chrono::seconds(15));
    sc.read_lux_to_screen();
    io_service.run();
  }catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
