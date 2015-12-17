#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "tcp_server.hpp"


void tcp_session::start(){
    socket_.async_read_some(boost::asio::buffer(data_, max_length), boost::bind(&tcp_session::process_command, this, _1, _2 ));
}

void tcp_session::process_command(const boost::system::error_code& error, size_t bytes_transferred){
    if (!error){
        std::cout << "Received data: " << data_ << "  of len: " << bytes_transferred << std::endl;
        //boost::asio::async_write(socket_, boost::asio::buffer(data_, bytes_transferred),  boost::bind(&tcp_session::handle_write, this, _1));
        char arg1[max_length];
        char arg2[max_length];
        char arg3[max_length];

        int e = sscanf(data_, "%s %s %s", arg1,arg2,arg3);

        if (e!=-1){
            if (arg1[0]=='r'){

            }else if(arg1[0]=='s'){

            }else if(arg1[0]=='g'){
                switch (arg2[0]){
                    case 'l':
                        e = atoi(arg3);
                        std::cout << "Get sensor at desk: " << e << std::endl;
                        break;
                    case 'd':
                        e = atoi(arg3);

                        break;
                    case 'o':
                        e = atoi(arg3);

                        break;
                    case 'L':
                        e = atoi(arg3);

                        break;
                    case 'O':
                        e = atoi(arg3);

                        break;
                    case 'r':
                        e = atoi(arg3);

                        break;
                    case 'p':
                        if(arg3[0]=='T'){

                        }else{
                            e = atoi(arg3);

                        }
                        break;
                    case 'e':
                        if(arg3[0]=='T'){

                        }else{
                            e = atoi(arg3);
                        }
                        break;
                    case 'c':
                        if(arg3[0]=='T'){

                        }else{
                            e = atoi(arg3);

                        }
                        break;
                    case 'v':
                        if(arg3[0]=='T'){

                        }else{
                            e = atoi(arg3);

                        }
                        break;
                }
            }
        }
        boost::asio::async_write(socket_, boost::asio::buffer("Invalid Command", 15),  boost::bind( &tcp_session::after_write, this, _1));
    }else{
        delete this;
    }
}

void tcp_session::after_write(const boost::system::error_code& error){

    if(!error){

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
