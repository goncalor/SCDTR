#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#define BUF_LEN 1024

using boost::asio::ip::tcp;

class tcp_session{
    private:
        tcp::socket socket_;
        enum { max_length = BUF_LEN };
        char data_[max_length];

    public:
        tcp_session(boost::asio::io_service& io_service)
            : socket_(io_service)
        {}

        tcp::socket& socket(){return socket_;}

        void start();

    private:

        void process_command(const boost::system::error_code& error, size_t bytes_transferred);

        
};


class tcp_server {

    private:
        boost::asio::io_service& io_service_; // Has a reference to a io_service
        tcp::acceptor acceptor_;

        void start_accept();

        void handle_accept(tcp_session* new_session, const boost::system::error_code& error);
    public:
        tcp_server(boost::asio::io_service& io_service, short port)
          : io_service_(io_service),
            acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
        { start_accept(); }

};
