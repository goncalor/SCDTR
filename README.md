# SCDTR 
This project is a demonstration of the local illumination control for the first semester of SCDTR at IST. It exemplifies the ability to set LED dimming values, read illuminance values in Lux, define setpoints for local control, demonstrate the performance of the control system in step changes in the reference setpoint and under external disturbances.

This repo contains the files used to solve the labs and project of this course.

The final code for the Server is on  *server/main.cpp*  .
Compiled with:

```
g++ -Wall -Wextra -pedantic -std=c++11 -g utils.cpp cpp_simplex/simplex.cpp tcp_server/tcp_server.cpp main.cpp -lpthread -lboost_system  -o teste_server.out
```
The arduino code is on *Arduino/main_/main_.ino* .

