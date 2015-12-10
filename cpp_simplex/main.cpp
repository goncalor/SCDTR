// g++ -Wall -Wextra -pedantic -std=c++11 -g simplex.cpp main.cpp

#include "simplex.hpp"
#include <iostream>
#include <vector>


/* INPUT
N = [1,2,3];
B = [4,5,6];
A = [1, 1, 3;
     2, 2, 5;
     4, 1, 2];
b = [30;24;36];
c = [3,1,2];
v = 0;
l = 6;
e = 1;
*/

/* OUTPUT
N = [6;2;3];
B = [4;5;1];
A = [-0.2500    0.7500    2.5000;
     -0.5000    1.5000    4.0000;
      0.2500    0.2500    0.5000];
b = [21;6;9];
c = [-0.7500, 0.25, 0.5];
v = 27;


N = [2;3;6];
B = [1;4;5];
A = [
     1.5     4     -0.5;
     0.25    0.5    0.25;
     0.75    2.5   -0.25;
    ];
b = [9;21;6];
c = [ 0.25, 0.5, -0.7500];
v = 27;


*/

int main() {
/*
    Simplex::pivot_struct test_struct;
    Simplex::pivot_struct out_struct;

    test_struct.v=0;
    test_struct.N.insert(1);
    test_struct.N.insert(2);
    test_struct.N.insert(3);
    test_struct.B.insert(4);
    test_struct.B.insert(5);
    test_struct.B.insert(6);

    test_struct.c[1]=3;
    test_struct.c[2]=1;
    test_struct.c[3]=2;

    test_struct.b[4]=30;
    test_struct.b[5]=24;
    test_struct.b[6]=36;

    std::map<int,float> A_l1;
    std::map<int,float> A_l2;
    std::map<int,float> A_l3;

    test_struct.A[4]=A_l1;
    test_struct.A[5]=A_l2;
    test_struct.A[6]=A_l3;

    test_struct.A[4][1]=1;
    test_struct.A[5][1]=2;
    test_struct.A[6][1]=4;
    test_struct.A[4][2]=1;
    test_struct.A[5][2]=2;
    test_struct.A[6][2]=1;
    test_struct.A[4][3]=3;
    test_struct.A[5][3]=5;
    test_struct.A[6][3]=2;

    int e = 1;
    int l = 6;

    //std::cout << "Before Pivot" <<std::endl;

    //Simplex::print_pivot_struct(test_struct);

    out_struct = Simplex::pivot(test_struct,e,l);

    //std::cout << "After Pivot" <<std::endl;

    //Simplex::print_pivot_struct(out_struct);

    std::vector<float> b;
    b.push_back(30);
    b.push_back(24);
    b.push_back(36);
    std::vector<float> c;
    c.push_back(3);
    c.push_back(1);
    c.push_back(2);
    std::vector<std::vector<float>> A;
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A[0].push_back(1);
    A[0].push_back(1);
    A[0].push_back(3);
    A[1].push_back(2);
    A[1].push_back(2);
    A[1].push_back(5);
    A[2].push_back(4);
    A[2].push_back(1);
    A[2].push_back(2);

    std::cout << "Create Simplex" <<std::endl;

    Simplex teste = Simplex(A,b,c);

    std::cout << "Print" <<std::endl;

    teste.print_internal_struct();

    std::vector<float> x = teste.solve();


    std::cout << "Solution" <<std::endl;

    for(auto i : x){
        std::cout << "x = " << i << std::endl;
    }

    */
/*
    std::cout << "Without initial feasible solution: " <<std::endl;

    std::vector<float> b1;
    b1.push_back(2);
    b1.push_back(-4);

    std::vector<float> c1;
    c1.push_back(2);
    c1.push_back(-1);

    std::vector<std::vector<float>> A1;
    A1.push_back(std::vector<float> ());
    A1.push_back(std::vector<float> ());
    A1[0].push_back(2);
    A1[0].push_back(-1);
    A1[1].push_back(1);
    A1[1].push_back(-5);

    Simplex teste1 = Simplex(A1,b1,c1);

    std::cout << "Print" <<std::endl;

    teste1.print_internal_struct();

    std::vector<float> x1 = teste1.solve();
*/

// TEST with Arduino LP

/*
# Created by Octave 3.8.1, Thu Dec 10 09:46:19 2015 WET <jmirandadealme@JMdeA-Zenbook>
# name: A
# type: matrix
# rows: 18
# columns: 9
 -10.21830350832508 -0.7271861952428118 -0.09748519653650334 -0.7271861952428118 -0.2769996701683155 -0.06890464691960219 -0.09748519653650334 -0.06890464691960219 -0.03280392761204531
 -0.7271861952428118 -10.21830350832508 -0.7271861952428118 -0.2769996701683155 -0.7271861952428118 -0.2769996701683155 -0.06890464691960219 -0.09748519653650334 -0.06890464691960219
 -0.09748519653650334 -0.7271861952428118 -10.21830350832508 -0.06890464691960219 -0.2769996701683155 -0.7271861952428118 -0.03280392761204531 -0.06890464691960219 -0.09748519653650334
 -0.7271861952428118 -0.2769996701683155 -0.06890464691960219 -10.21830350832508 -0.7271861952428118 -0.09748519653650334 -0.7271861952428118 -0.2769996701683155 -0.06890464691960219
 -0.2769996701683155 -0.7271861952428118 -0.2769996701683155 -0.7271861952428118 -10.21830350832508 -0.7271861952428118 -0.2769996701683155 -0.7271861952428118 -0.2769996701683155
 -0.06890464691960219 -0.2769996701683155 -0.7271861952428118 -0.09748519653650334 -0.7271861952428118 -10.21830350832508 -0.06890464691960219 -0.2769996701683155 -0.7271861952428118
 -0.09748519653650334 -0.06890464691960219 -0.03280392761204531 -0.7271861952428118 -0.2769996701683155 -0.06890464691960219 -10.21830350832508 -0.7271861952428118 -0.09748519653650334
 -0.06890464691960219 -0.09748519653650334 -0.06890464691960219 -0.2769996701683155 -0.7271861952428118 -0.2769996701683155 -0.7271861952428118 -10.21830350832508 -0.7271861952428118
 -0.03280392761204531 -0.06890464691960219 -0.09748519653650334 -0.06890464691960219 -0.2769996701683155 -0.7271861952428118 -0.09748519653650334 -0.7271861952428118 -10.21830350832508
 1 0 0 0 0 0 0 0 0
 0 1 0 0 0 0 0 0 0
 0 0 1 0 0 0 0 0 0
 0 0 0 1 0 0 0 0 0
 0 0 0 0 1 0 0 0 0
 0 0 0 0 0 1 0 0 0
 0 0 0 0 0 0 1 0 0
 0 0 0 0 0 0 0 1 0
 0 0 0 0 0 0 0 0 1


# name: b
# type: matrix
# rows: 18
# columns: 1
 -2.234087037518483
 -6.370072142271482
 -2.314137495496334
 -4.195003918678363
 -2.89337934873546
 -7.706424278239382
 -7.357001197283779
 -5.483351507948527
 -7.286096857845781
 1
 1
 1
 1
 1
 1
 1
 1
 1


# name: c
# type: range
# base, length, increment
-1 9 -0

c =

  -1  -1  -1  -1  -1  -1  -1  -1  -1


# name: random_pwm
# type: matrix
# rows: 9
# columns: 1
 0.1374661759009127
 0.5587605221930771
 0.1228113038292256
 0.3096195642559854
 0.104062656935522
 0.659452543924277
 0.6504349725253338
 0.4053939889252992
 0.6207688892129481


*/

}
