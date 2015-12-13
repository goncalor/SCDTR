#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "tcp_server.hpp"


void tcp_session::start(){
    socket_.async_read_some(boost::asio::buffer(data_, max_length), boost::bind(&tcp_session::process_command, this, _1, _2 ));
}

void tcp_session::process_command(const boost::system::error_code& error, size_t bytes_transferred){
  if (!error){
    std::cout << "Received data: " << data_ << std::endl;
    //boost::asio::async_write(socket_, boost::asio::buffer(data_, bytes_transferred),  boost::bind(&tcp_session::handle_write, this, _1));
  }else{
    delete this;
  }
}

void tcp_server::start_accept(){
    tcp_session* new_session = new tcp_session(io_service_);
    acceptor_.async_accept(new_session->socket(), boost::bind(&tcp_server::handle_accept, this, new_session, _1));
}

void tcp_server::handle_accept(tcp_session* new_session, const boost::system::error_code& error){
  if (!error){
    new_session->start();
  }else{
    delete new_session;
  }
  start_accept();
}
