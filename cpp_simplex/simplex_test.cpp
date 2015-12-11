// g++ -Wall -Wextra -pedantic -std=c++11 -g simplex.cpp  simplex_test.cpp -o simplex_test.out
#include <boost/test/minimal.hpp>
#include "simplex.hpp"
#include <iostream>
#include <vector>

#define DELTA 0.001

// EXAMPLE
/*
//____________________________________________________________________________//

int add( int i, int j ) { return i+j; }

//____________________________________________________________________________//


// returns true if there is an error
bool first_test(){
    return add( 2,2 ) != 5;
}

int test_main( int, char *[] )             // note the name!
{
    // six ways to detect and report the same error:
    BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error
    BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error
    if(first_test() )
        BOOST_ERROR( "Ouch..." );          // #3 continues on error
    if( add( 2,2 ) != 4 )
        BOOST_FAIL( "Ouch..." );           // #4 throws on error
    if( add( 2,2 ) != 4 ) throw "Oops..."; // #5 throws on error

    return add( 2, 2 ) == 4 ? 0 : 1;       // #6 returns error code
}
*/
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

int test_main( int, char *[] )             // note the name!
{
    if(test_Mr_Silva_1() )
        BOOST_ERROR( "Test Mr. Silva 1" );
    if(test_Cormen_basic() )
        BOOST_ERROR( "Test Basic Cormen 865" );

    if(test_Cormen_no_initial_solution() )
        BOOST_ERROR( "Test Basic Cormen 888" );


    return 0;
}
