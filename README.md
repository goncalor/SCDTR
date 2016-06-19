# SCDTR 
This is the final project of the Distributed Control Real-Time Systems course at IST. It is a demonstration of local illumination control. It exemplifies the ability to set LED dimming values, read illuminance values in Lux, define setpoints for local control, demonstrate the performance of the control system in step changes in the reference setpoint and under external disturbances.

The project was developed by:
- [Gonçalo Ribeiro](https://github.com/goncalor);
- [João Almeida](https://github.com/Joao-M-Almeida/);

This repository contains the files used to solve the labs and project of this course.

The report for this project can be previewed and downloaded [here](report/final/report.pdf).

The final code for the Server is on  *server/main.cpp*. It can be compiled with:

```
g++ -Wall -Wextra -pedantic -std=c++11 -g utils.cpp cpp_simplex/simplex.cpp tcp_server/tcp_server.cpp main.cpp -lpthread -lboost_system  -o teste_server.out
```
The arduino code is on *Arduino/main_/main_.ino* .

