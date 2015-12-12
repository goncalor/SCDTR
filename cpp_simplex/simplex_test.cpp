// g++ -Wall -Wextra -pedantic -std=c++11 -g simplex.cpp  simplex_test.cpp -o simplex_test.out
#include <boost/test/minimal.hpp>
#include "simplex.hpp"
#include <iostream>
#include <vector>

#define DELTA 0.001

// For instructions see: http://www.boost.org/doc/libs/1_36_0/libs/test/doc/html/minimal.html

// returns true if there is an error
bool test_Mr_Silva_1(){
    std::vector<std::vector<float>> A3;
    A3.push_back(std::vector<float> ());
    A3.push_back(std::vector<float> ());
    A3.push_back(std::vector<float> ());
    A3[0].push_back(2);
    A3[0].push_back(1);
    A3[1].push_back(4);
    A3[1].push_back(3);
    std::vector<float> b3;
    b3.push_back(100);
    b3.push_back(240);
    std::vector<float> c3;
    c3.push_back(7);
    c3.push_back(5);

    Simplex teste3 = Simplex(A3,b3,c3);

    std::vector<float> x3 = teste3.solve();

    return (x3[0]!=30 || x3[1]!=40);

}

// returns true if there is an error
bool test_Cormen_basic(){
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

    Simplex teste = Simplex(A,b,c);
    std::vector<float> x = teste.solve();

    return (x[0]!=8 || x[1]!=4 ||  x[2]!=0 );
}

// returns true if there is an error
bool test_Cormen_no_initial_solution(){

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

    std::vector<float> x = teste1.solve();

    return ((x[0] > (1.55556 + DELTA) || x[0] < (1.55556 - DELTA)) ||  (x[1] > (1.11111 + DELTA) || x[1] < (1.11111 - DELTA)));
}

bool test_Cormen_29_5(){

    std::vector<float> b;
    b.push_back(8);
    b.push_back(-3);
    b.push_back(2);

    std::vector<float> c;
    c.push_back(1);
    c.push_back(3);

    std::vector<std::vector<float>> A;
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A[0].push_back(1);
    A[0].push_back(-1);
    A[1].push_back(-1);
    A[1].push_back(-1);
    A[2].push_back(-1);
    A[2].push_back(4);

    Simplex teste = Simplex(A,b,c);

    std::vector<float> x = teste.solve();

    return ((x[0] > (11.33333 + DELTA) || x[0] < (11.33333 - DELTA)) ||  (x[1] > (3.33333 + DELTA) || x[1] < (3.33333 - DELTA)));
}


bool test_Mr_Silva_unnecessary_constrain(){
    // Mr Silva with unnecessary constrain

    std::vector<std::vector<float>> A4;
    A4.push_back(std::vector<float> ());
    A4.push_back(std::vector<float> ());
    A4.push_back(std::vector<float> ());
    A4[0].push_back(2);
    A4[0].push_back(1);
    A4[1].push_back(4);
    A4[1].push_back(3);
    A4[2].push_back(-1);
    A4[2].push_back(0);

    std::vector<float> b4;
    b4.push_back(100);
    b4.push_back(240);
    b4.push_back(-20);

    std::vector<float> c4;
    c4.push_back(7);
    c4.push_back(5);

    Simplex teste4 = Simplex(A4,b4,c4);
    std::vector<float> x4 = teste4.solve();

    for(auto i : x4){
        std::cout << "x = " << i << std::endl;
    }

    return ((x4[0] > (30 + DELTA) || x4[0] < (30 - DELTA)) ||  (x4[1] > (40 + DELTA) || x4[1] < (40 - DELTA)));
}

bool test_simplex_with_initial_solution1(){
    std::vector<std::vector<float>> A5;
    A5.push_back(std::vector<float> ());
    A5.push_back(std::vector<float> ());
    A5.push_back(std::vector<float> ());
    A5[0].push_back(0);
    A5[0].push_back(-1);
    A5[1].push_back(1);
    A5[1].push_back(2);
    A5[2].push_back(3);
    A5[2].push_back(4);


    std::vector<float> b5;
    b5.push_back(20);
    b5.push_back(60);
    b5.push_back(160);

    std::vector<float> c5;
    c5.push_back(5);
    c5.push_back(7);


    Simplex teste5 = Simplex(A5,b5,c5);
    std::vector<float> x5 = teste5.solve();



    return ((x5[0] > (40 + DELTA) || x5[0] < (40 - DELTA)) ||  (x5[1] > (10 + DELTA) || x5[1] < (10 - DELTA)));

}

int test_main( int, char *[] )             // note the name!
{
    if(test_Mr_Silva_1() )
        BOOST_ERROR( "Test Mr. Silva simple" );
    if(test_Cormen_basic() )
        BOOST_ERROR( "Test Basic Cormen 865" );
    if(test_Cormen_no_initial_solution() )
        BOOST_ERROR( "Test Basic Cormen 888" );
    if(test_Mr_Silva_unnecessary_constrain() )
        BOOST_ERROR( "Test Mr Silva with unnecessary constrain" );
    if(test_simplex_with_initial_solution1() )
        BOOST_ERROR( "Test Simplex with initial solution 1" );
    if(test_Cormen_29_5 () )
        BOOST_ERROR( "Test Cormen 29-5.5" );


    return 0;
}
