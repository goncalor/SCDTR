#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
//#include "../serial/serial.hpp"

#include "tcp_server.hpp"


void Tcp_session::start(){
    socket_.async_read_some(boost::asio::buffer(data_, max_length), boost::bind(&Tcp_session::process_command, this, _1, _2 ));
}

void Tcp_session::process_command(const boost::system::error_code& error, size_t bytes_transferred){
    if (!error){
        std::cout << "Received data: " << data_ << "  of len: " << bytes_transferred << std::endl;
        //boost::asio::async_write(socket_, boost::asio::buffer(data_, bytes_transferred),  boost::bind(&Tcp_session::handle_write, this, _1));
        char arg1[max_length];
        char arg2[max_length];
        char arg3[max_length];

        int e = sscanf(data_, "%s %s %s", arg1,arg2,arg3);

        if (e!=-1){
            if (arg1[0]=='r'){
                sc_.send_reset(socket_);
            }else if(arg1[0]=='s'){
                e = atoi(arg2);
                int aux = atoi(arg3);
                sc_.set_occupancy_at(e,socket_, (bool) aux);
            }else if(arg1[0]=='S'){
                sc_.run_simplex();
            }else if(arg1[0]=='C'){
                sc_.calibrate();
            }else if(arg1[0]=='g'){
                switch (arg2[0]){
                    case 'l':
                        e = atoi(arg3);
                        std::cout << "Get sensor at desk: " << e << std::endl;
                        sc_.get_illuminance_at(e,socket_);
                        break;
                    case 'd':
                        e = atoi(arg3);
                        std::cout << "Get duty_cycle at desk: " << e << std::endl;
                        sc_.get_duty_cycle_at(e,socket_);
                        break;
                    case 'o':
                        e = atoi(arg3);
                        std::cout << "Get occupancy at desk: " << e << std::endl;
                        sc_.get_occupancy_at(e,socket_);
                        break;
                    case 'L':
                        e = atoi(arg3);
                        std::cout << "Get desired lux at desk: " << e << std::endl;
                        sc_.get_desired_lux_at(e,socket_);
                        break;
                    case 'O':
                        e = atoi(arg3);
                        std::cout << "Get external illuminance at desk: " << e << std::endl;
                        sc_.get_external_illuminance_at(e,socket_);
                        break;
                    case 'r':
                        e = atoi(arg3);
                        std::cout << "Get reference at desk: " << e << std::endl;
                        sc_.get_ref_at(e,socket_);
                        break;
                    case 'p':
                        if(arg3[0]=='T'){
                            sc_.get_total_power(socket_);
                        }else{
                            e = atoi(arg3);
                            std::cout << "Get power at desk: " << e << std::endl;
                            sc_.get_power_at(e,socket_);
                        }
                        break;
                    case 'e':
                        if(arg3[0]=='T'){
                            sc_.get_total_energy(socket_);
                        }else{
                            e = atoi(arg3);
                            std::cout << "Get energy at desk: " << e << std::endl;
                            sc_.get_energy_at(e,socket_);
                        }
                        break;
                    case 'c':
                        if(arg3[0]=='T'){
                            sc_.get_total_confort(socket_);

                        }else{
                            e = atoi(arg3);
                            std::cout << "Get confort at desk: " << e << std::endl;
                            sc_.get_confort_at(e,socket_);
                        }
                        break;
                    case 'v':
                        if(arg3[0]=='T'){
                            sc_.get_total_flicker(socket_);
                        }else{
                            e = atoi(arg3);
                            std::cout << "Get confort variance at desk: " << e << std::endl;
                            sc_.get_flicker_at(e,socket_);
                        }
                        break;
                }
            }else if(arg1[0]=='G'){
                switch (arg2[0]){
                    case 'e':
                        e = atoi(arg3);
                        std::cout << "Get last minute energy at desk: " << e << std::endl;
                        sc_.get_minute_energy_at(e,socket_);
                        break;
                    case 'c':
                        e = atoi(arg3);
                        std::cout << "Get last minute confort at desk: " << e << std::endl;
                        sc_.get_minute_confort_at(e,socket_);
                        break;
                    case 'v':
                        e = atoi(arg3);
                        std::cout << "Get last minute confort variance at desk: " << e << std::endl;
                        sc_.get_minute_flicker_at(e,socket_);
                        break;
                }
            }
        }
    }else{
        delete this;
    }
}

void Tcp_session::after_write(const boost::system::error_code& error){

    if(!error){

    }else{
      delete this;
    }

}

void Tcp_server::start_accept(){
    Tcp_session* new_session = new Tcp_session(io_service_, sc_);
    acceptor_.async_accept(new_session->socket(), boost::bind(&Tcp_server::handle_accept, this, new_session, _1));
}

void Tcp_server::handle_accept(Tcp_session* new_session, const boost::system::error_code& error){
  if (!error){
    new_session->start();
  }else{
    delete new_session;
  }
  start_accept();
}
