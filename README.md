# SCDTR 
This is the final project of the [Distributed Control of Real-Time Systems (SCDTR) course at IST](https://fenix.tecnico.ulisboa.pt/cursos/meec/disciplina-curricular/1529008375879). 

The project was developed by:
- [Gonçalo Ribeiro](https://github.com/goncalor);
- [João Almeida](https://github.com/Joao-M-Almeida/);

The project was implemented on a wooden box with 3 LEDS controled by 3 arduinos. The goal is to demonstrated the control of local illumication. It exemplifies the ability to set LED dimming values, read illuminance values in Lux, define setpoints for local control, demonstrate the performance of the control system in step changes in the reference setpoint and under external disturbances. To speed up the response to changes in the system the Simplex algortithm was implemented following [Cormen's Introduction to Algorithms](https://en.wikipedia.org/wiki/Introduction_to_Algorithms) pseudo-code, to calculate the optimal values of illuminance for each LED.

This repository contains the files used to solve the labs and project of this course.

The report for this project can be previewed and downloaded [here](report/final/report.pdf).

The final code for the Server is on  *server/main.cpp*. It can be compiled with:

```
g++ -Wall -Wextra -pedantic -std=c++11 -g utils.cpp cpp_simplex/simplex.cpp tcp_server/tcp_server.cpp main.cpp -lpthread -lboost_system  -o teste_server.out
```
The arduino code is on *Arduino/main_/main_.ino* .

The project makes use of the Boost ASIO libraries to implement the asynchronous TCP server.

