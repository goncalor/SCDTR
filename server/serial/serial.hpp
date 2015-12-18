#ifndef SERIAL_HPP
#define SERIAL_HPP
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <exception>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include "../utils.hpp"
#include "../cpp_simplex/simplex.hpp"
#define MIN_SAMPLES 20

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

            //calibrate();
            //run_simplex();

            std::cout << "Arduino READY" << std::endl;
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
        void send_reset(boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            boost::asio::write(sp_, boost::asio::buffer("r"));
            std::string s("ack");
            boost::asio::write(to, boost::asio::buffer(s + '\n',100));
            wait_for_calibration();
            busy_ = false;
        }

        void set_occupancy_at(int i, boost::asio::ip::tcp::socket& to, bool new_state){
            wait_ready();
            busy_ = true;
            if(new_state){
                boost::asio::write(sp_, boost::asio::buffer("s " + std::to_string(i) + " 1"));
            }else{
                boost::asio::write(sp_, boost::asio::buffer("s " + std::to_string(i) + " 0"));
            }
            std::string s("ack");
            boost::asio::write(to, boost::asio::buffer(s + '\n',100));
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

        void get_duty_cycle_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g d " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_occupancy_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g o " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_desired_lux_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g L " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_external_illuminance_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g O " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_ref_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g r " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_power_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g p " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_energy_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g e " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_confort_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g c " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_flicker_at(int i, boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "g v " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            answer_to_socket(to);
            busy_ = false;
        }

        void get_total_power(boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            double accum = 0;
            std::cout << "Received: "  << std::endl;
            for(int j = 1; j<=3;j++){
                std::string a = "g p " + std::to_string(j);
                boost::asio::write(sp_, boost::asio::buffer(a));
                boost::system::error_code error;
                boost::asio::read_until(sp_,read_buf_,'\n', error);
                if(error){
                    std::cout << "ERROR: "  << std::endl;
                    throw std::runtime_error("Error reading serial port\n");
                }
                if( read_buf_.size()<=2){
                    read_buf_.consume(2);
                    boost::asio::read_until(sp_,read_buf_,'\n', error);
                    if(error){
                        std::cout << "ERROR: "  << std::endl;
                        throw std::runtime_error("Error reading serial port\n");
                    }
                    //std::cout << "Size of buf: " << read_buf_.size() << std::endl;
                }
                std::istream str(&read_buf_);
                std::string s;
                str >> s;
                std::cout << s << std::endl;
                std::cout << std::stod(s) << std::endl;
                accum += std::stod(s);
                std::cout << std::to_string(accum) << std::endl;


            }
            boost::asio::write(to, boost::asio::buffer(std::to_string(accum) + '\n',100));
            busy_ = false;
        }

        void get_total_energy( boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            double accum = 0;
            std::cout << "Received: "  << std::endl;
            for(int j = 1; j<=3;j++){
                std::string a = "g e " + std::to_string(j);
                boost::asio::write(sp_, boost::asio::buffer(a));
                boost::system::error_code error;
                boost::asio::read_until(sp_,read_buf_,'\n', error);
                if(error){
                    std::cout << "ERROR: "  << std::endl;
                    throw std::runtime_error("Error reading serial port\n");
                }
                if( read_buf_.size()<=2){
                    read_buf_.consume(2);
                    boost::asio::read_until(sp_,read_buf_,'\n', error);
                    if(error){
                        std::cout << "ERROR: "  << std::endl;
                        throw std::runtime_error("Error reading serial port\n");
                    }
                    //std::cout << "Size of buf: " << read_buf_.size() << std::endl;
                }
                std::istream str(&read_buf_);
                std::string s;
                str >> s;
                std::cout << s << std::endl;
                std::cout << std::stod(s) << std::endl;
                accum += std::stod(s);
                std::cout << std::to_string(accum) << std::endl;


            }
            boost::asio::write(to, boost::asio::buffer(std::to_string(accum) + '\n',100));
            busy_ = false;
        }

        void get_total_confort(boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            double accum = 0;
            std::cout << "Received: "  << std::endl;
            for(int j = 1; j<=3;j++){
                std::string a = "g c " + std::to_string(j);
                boost::asio::write(sp_, boost::asio::buffer(a));
                boost::system::error_code error;
                boost::asio::read_until(sp_,read_buf_,'\n', error);
                if(error){
                    std::cout << "ERROR: "  << std::endl;
                    throw std::runtime_error("Error reading serial port\n");
                }
                if( read_buf_.size()<=2){
                    read_buf_.consume(2);
                    boost::asio::read_until(sp_,read_buf_,'\n', error);
                    if(error){
                        std::cout << "ERROR: "  << std::endl;
                        throw std::runtime_error("Error reading serial port\n");
                    }
                    //std::cout << "Size of buf: " << read_buf_.size() << std::endl;
                }
                std::istream str(&read_buf_);
                std::string s;
                str >> s;
                std::cout << s << std::endl;
                std::cout << std::stod(s) << std::endl;
                accum += std::stod(s);
                std::cout << std::to_string(accum) << std::endl;


            }
            boost::asio::write(to, boost::asio::buffer(std::to_string(accum) + '\n',100));
            busy_ = false;
        }

        void get_total_flicker(boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            double accum = 0;
            std::cout << "Received: "  << std::endl;
            for(int j = 1; j<=3;j++){
                std::string a = "g v " + std::to_string(j);
                boost::asio::write(sp_, boost::asio::buffer(a));
                boost::system::error_code error;
                boost::asio::read_until(sp_,read_buf_,'\n', error);
                if(error){
                    std::cout << "ERROR: "  << std::endl;
                    throw std::runtime_error("Error reading serial port\n");
                }
                if( read_buf_.size()<=2){
                    read_buf_.consume(2);
                    boost::asio::read_until(sp_,read_buf_,'\n', error);
                    if(error){
                        std::cout << "ERROR: "  << std::endl;
                        throw std::runtime_error("Error reading serial port\n");
                    }
                    //std::cout << "Size of buf: " << read_buf_.size() << std::endl;
                }
                std::istream str(&read_buf_);
                std::string s;
                str >> s;
                std::cout << s << std::endl;
                std::cout << std::stod(s) << std::endl;
                accum += std::stod(s);
                std::cout << std::to_string(accum) << std::endl;


            }
            boost::asio::write(to, boost::asio::buffer(std::to_string(accum) + '\n',100));
            busy_ = false;
        }

        void run_simplex(){
            wait_ready();
            busy_ = true;

            std::vector<std::vector<float>> E;
            std::vector<float> O;
            std::vector<float> L;
            std::string s;
            E.push_back(std::vector<float>());
            E.push_back(std::vector<float>());
            E.push_back(std::vector<float>());

            boost::asio::write(sp_, boost::asio::buffer("O"));
            s = read_line();
            int aux1, aux2, aux3;
            sscanf(s.c_str(),"%d %d %d", &aux1, &aux2, &aux3);
            O.push_back(aux1*1.0/1024);
            O.push_back(aux2*1.0/1024);
            O.push_back(aux3*1.0/1024);

            boost::asio::write(sp_, boost::asio::buffer("E"));
            for(int i = 0; i<3; i++){
                s = read_line();
                sscanf(s.c_str(),"%d %d %d", &aux1, &aux2, &aux3);
                E[i].push_back(aux1*1.0/1024);
                E[i].push_back(aux2*1.0/1024);
                E[i].push_back(aux3*1.0/1024);
            }
            float aux4;
            for(int i = 0; i<3; i++){
                boost::asio::write(sp_, boost::asio::buffer("g L " + std::to_string(i+1)));
                s = read_line();
                sscanf(s.c_str(),"%f", &aux4);
                aux4=lux_to_pwm(aux4);
                L.push_back(aux4*1.0/255);
            }

            lp_struct lp = generate_linear_programm(E,O,L);

            Simplex to_solve = Simplex(lp.A,lp.b,lp.c);

            std::vector<float> x = to_solve.solve();
            std::cout << "Simplex Solution: " << std::endl;
            for(unsigned int i=0; i<x.size();i++){
                std::cout << "x = " << round(x[i]*255) << std::endl;
                x[i] = round(x[i]*255);
            }
            char temp[100];
            sprintf(temp, "h %d %d %d", (int) x[0], (int) x[1], (int) x[2]);
            std::string t(temp);
            boost::asio::write(sp_, boost::asio::buffer(t));

            busy_ = false;
        }


        void get_minute_energy_at(int i,boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "e " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            std::string s;
            for(int i=0;i<MIN_SAMPLES;i++){
                s = read_line();
                std::cout << "E[t-" << (MIN_SAMPLES-i)*(60/MIN_SAMPLES) <<"] = " << s <<std::endl;
            }
            std::string s1("ack");
            boost::asio::write(to, boost::asio::buffer(s1 + '\n',100));
            std::cout << "Sent" << std::endl;
            busy_ = false;
        }

        void get_minute_confort_at(int i,boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "n " + std::to_string(i);
            std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            std::string s;
            for(int i=0;i<MIN_SAMPLES;i++){
                s = read_line();
                std::cout << "Ce[t-" << (MIN_SAMPLES-i)*(60/MIN_SAMPLES) <<"] = " << s <<std::endl;
            }
            std::string s1("ack");
            boost::asio::write(to, boost::asio::buffer(s1 + '\n',100));
            busy_ = false;
        }

        void get_minute_flicker_at(int i,boost::asio::ip::tcp::socket& to){
            wait_ready();
            busy_ = true;
            //std::cout << "Sending to Arduino:" << i << std::endl;
            std::string a = "f " + std::to_string(i);
            //std::cout << "Sending to Arduino:" << a << std::endl;
            boost::asio::write(sp_, boost::asio::buffer(a));
            std::string s;
            for(int i=0;i<MIN_SAMPLES;i++){
                s = read_line();
                std::cout << "Cv[t-" << (MIN_SAMPLES-i)*(60/MIN_SAMPLES) <<"] = " << s <<std::endl;
            }
            std::string s1("ack");
            boost::asio::write(to, boost::asio::buffer(s1 + '\n',100));
            busy_ = false;
        }

    private:
        std::string read_line(){
            boost::system::error_code error;
            boost::asio::read_until(sp_,read_buf_,'\n', error);
            if(error){
                std::cout << "ERROR: "  << std::endl;
                throw std::runtime_error("Error reading serial port\n");
            }
            //std::cout << "Size of buf: " << read_buf_.size() << std::endl;
            if( read_buf_.size()<=2){
                read_buf_.consume(2);
                boost::asio::read_until(sp_,read_buf_,'\n', error);
                if(error){
                    std::cout << "ERROR: "  << std::endl;
                    throw std::runtime_error("Error reading serial port\n");
                }
                //std::cout << "Size of buf: " << read_buf_.size() << std::endl;
            }
            std::istream str(&read_buf_);
            std::string s;
            std::getline(str, s);
            //std::cout << "Received: "  << std::endl;
            //std::cout << s << std::endl;
            return s;
        }

        void answer_to_socket(boost::asio::ip::tcp::socket& to){
            std::string s = read_line();
            //std::cout << "Sending to client:" << s + '\n' << std::endl;
            boost::asio::write(to, boost::asio::buffer(s + '\n',100));
        }



};



#endif
