// g++ -Wall -Wextra -pedantic -std=c++11 simplex.cpp main.cpp

#include "simplex.hpp"
#include <iostream>


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

    pivot_struct test_struct;
    pivot_struct out_struct;

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

    std::cout << "Before Pivot" <<std::endl;

    print_pivot_struct(test_struct);

    out_struct = pivot(test_struct,e,l);

    std::cout << "After Pivot" <<std::endl;

    print_pivot_struct(out_struct);



    return 0;
}
