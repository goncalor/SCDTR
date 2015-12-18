#ifndef SERIAL_HPP
#define SERIAL_HPP
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <exception>
#include <thread>
#include <chrono>
#include <stdlib.h>

/*class serial_exception: public std::exception {
    virtual const char* what() const throw(){
        return "Unnable to open serial port with those configurations.";
    }
} serial_excep;*/


class Serial_connection {

    private:
        boost::asio::io_service& io_service_; // Has a reference to a io_service
        boost::asio::serial_port sp_;
        boost::asio::streambuf read_buf_;
        bool ready_ = false;
        bool busy_ = true;

    public:
        Serial_connection(boost::asio::io_service& io_service, std::string port_name, int baud_rate)
          : io_service_(io_service),
            sp_(io_service_)
        {
            boost::system::error_code ec;
            sp_.open(port_name, ec);
            if(ec){
                throw -7;
            //    throw serial_excep;
            }
            sp_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate),ec);
            if(ec){
                throw -7;
            //    throw serial_excep;
            }
            wait_for_calibration();
            boost::asio::write(sp_, boost::asio::buffer("m"));
            //calibrate();
            ready_ = true;
            busy_ = false;
            std::cout << "Arduino READY" << std::endl;

            // TODO Call simplex. Actually the arduino should only be done after simplex is run once.
        }


        void calibrate(){
            wait_ready();
            busy_ = true;
            boost::asio::write(sp_, boost::asio::buffer("F"));
            wait_for_calibration();
            ready_=true;
            busy_ = false;
            std::cout << "Arduino READY" << std::endl;
        }


        /*void read_handler(const boost::system::error_code &ec, size_t nbytes) {
            if (!ec){
                std::cout << &read_buf_ << std::endl;
                std::cout <<"Read " << nbytes << " bytes" << std::endl << std::flush;;
                boost::asio::async_read_until(sp_,read_buf_,'\n',boost::bind(&Serial_connection::read_handler, this,_1,_2));
            }
        }*/


        // Test function
        /*void read_lux_to_screen(){
            busy_ = true;
            wait_ready();
            std::cout << "Writing: " << "i" << std::endl;
            boost::asio::write(sp_, boost::asio::buffer("i"));
            std::this_thread::sleep_for (std::chrono::seconds(1));
            //boost::asio::async_read(sp_,read_buf_, boost::bind(&Serial_connection::read_handler, this,_1,_2));
            boost::asio::async_read_until(sp_,read_buf_,'\n',boost::bind(&Serial_connection::read_handler, this,_1,_2));
            busy_ = false;
        }*/

        // BLOCKING!
        void wait_ready(){
            while(!ready_ || busy_){
                std::this_thread::sleep_for (std::chrono::seconds(1));
                std::cout << "Arduino Busy" << std::endl;

            }
        }
        // Blocks until the system is done calibrating.
        // Reading a byte at a time, not efficient but shouldn't matter.
        void wait_for_calibration(){
            busy_ = true;
            std::cout << "Waiting for the arduino to be ready" << std::endl;
            boost::system::error_code error;
            std::string s;
            while(true){
                boost::asio::read_until(sp_,read_buf_,'\n', error);
                std::istream str(&read_buf_);
                std::getline(str, s);
                //std::cout << "Received: "  << s << std::endl;

                if(s.find_first_of("D")!=std::string::npos){
                    break;
                }
            }
            busy_ = false;
        }

        // This will also be blocking
        void send_reset(){
            wait_ready();
            busy_ = true;
            boost::asio::write(sp_, boost::asio::buffer("r"));
            wait_for_calibration();
            busy_ = false;
        }

       void get_illuminance_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g l " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void answer_to_socket(boost::asio::ip::tcp::socket& to){
            std::cout << "Received: "  << std::endl;
            boost::system::error_code error;
            boost::asio::read_until(sp_,read_buf_,'\n', error);
            if(error){
                std::cout << "ERROR: "  << std::endl;
                throw std::runtime_error("Error reading serial port\n");
            }
            std::istream str(&read_buf_);
            std::string s;
            str >> s;
            std::cout << "Sending to client:" << s + '\n' << std::endl;
            boost::asio::write(to, boost::asio::buffer(s + '\n',100));
        }



};

#endif
