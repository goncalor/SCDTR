//SERIALREAD.CPP
// g++ -std=c++11 serialread.cpp -lpthread -lboost_system

#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using namespace boost::asio;

int main() {
    io_service io;                //initialize services
    serial_port sp(io);        //create io object
    boost::system::error_code ec;
    sp.open("/dev/ttyACM0", ec);    //connect to port
    if( ec ) {cout << "Error"; return -1;}
    //set baud rate
    sp.set_option(serial_port_base::baud_rate(38400),ec);
    if( ec ) {cout << "Error"; return -1;}
    while(true){
        boost::asio::streambuf str;
        //read(sp, str, ec);
        //if( ec ) {cout << "Error" << ec << endl; return -1;}
        //read_until(sp, str, '>');
        read_until(sp, str, '\n');
        cout << &str;
    }
}
