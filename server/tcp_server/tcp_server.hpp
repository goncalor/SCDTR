

//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP
#include <iostream>
#include <boost/bind.hpp>
#include "../serial/serial.hpp"
#include <boost/asio.hpp>
#define BUF_LEN 1024

using boost::asio::ip::tcp;

class Tcp_session{
    private:
        tcp::socket socket_;
        enum { max_length = BUF_LEN };
        char data_[max_length];
        Serial_connection&  sc_;

    public:
        Tcp_session(boost::asio::io_service& io_service, Serial_connection& sc)
            : socket_(io_service),
            sc_(sc)
        {        }

        tcp::socket& socket(){return socket_;}

        void start();

    private:

        void process_command(const boost::system::error_code& error, size_t bytes_transferred);

        void after_write(const boost::system::error_code& error);



};


class Tcp_server {

    private:
        boost::asio::io_service& io_service_; // Has a reference to a io_service
        tcp::acceptor acceptor_;
        Serial_connection& sc_;

        void start_accept();

        void handle_accept(Tcp_session* new_session, const boost::system::error_code& error);
    public:
        Tcp_server(boost::asio::io_service& io_service, short port, Serial_connection& sc)
          : io_service_(io_service),
            acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
            sc_(sc)
        { start_accept(); }

};
#endif
