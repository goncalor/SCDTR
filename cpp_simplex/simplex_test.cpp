// g++ -Wall -Wextra -pedantic -std=c++11 -g simplex.cpp  simplex_test.cpp -o simplex_test.out
#include <boost/test/minimal.hpp>
#include "simplex.hpp"
#include "util_testing.hpp"
#include <iostream>
#include <vector>

#define DELTA 0.001

// For instructions see: http://www.boost.org/doc/libs/1_36_0/libs/test/doc/html/minimal.html

// returns true if there is an error
bool test_Mr_Silva_1(){
    std::vector<float> sol;
    sol.push_back(30);
    sol.push_back(40);

    std::vector<std::vector<float>> A;
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A[0].push_back(2);
    A[0].push_back(1);
    A[1].push_back(4);
    A[1].push_back(3);
    std::vector<float> b;
    b.push_back(100);
    b.push_back(240);
    std::vector<float> c;
    c.push_back(7);
    c.push_back(5);

    Simplex teste = Simplex(A,b,c);
    std::vector<float> x = teste.solve();

    return !vector_within_delta(x,sol,DELTA);

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

    std::vector<float> sol;
    sol.push_back(8);
    sol.push_back(4);
    sol.push_back(0);


    return !vector_within_delta(x,sol,DELTA);

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

    std::vector<float> sol;
    sol.push_back(1.55555);
    sol.push_back(1.11111);

    return !vector_within_delta(x,sol,DELTA);
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

    std::vector<float> sol;
    sol.push_back(11.33333);
    sol.push_back(3.33333);

    return !vector_within_delta(x,sol,DELTA);

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
    std::vector<float> x = teste4.solve();


    std::vector<float> sol;
    sol.push_back(30);
    sol.push_back(40);

    return !vector_within_delta(x,sol,DELTA);


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
    std::vector<float> x = teste5.solve();

    std::vector<float> sol;
    sol.push_back(40);
    sol.push_back(10);

    return !vector_within_delta(x,sol,DELTA);

}

bool test_lab_08_ex_random_pwm(){

    std::vector<float> b;
    std::vector<float> c;
    std::vector<std::vector<float>> A;
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A.push_back(std::vector<float> ());
    A[0].push_back(-10.2183035083);
    A[0].push_back(-0.7271861952);
    A[0].push_back(-0.0974851965);
    A[0].push_back(-0.7271861952);
    A[0].push_back(-0.2769996702);
    A[0].push_back(-0.0689046469);
    A[0].push_back(-0.0974851965);
    A[0].push_back(-0.0689046469);
    A[0].push_back(-0.0328039276);
    A[1].push_back(-0.7271861952428118);
    A[1].push_back(-10.21830350832508);
    A[1].push_back(-0.7271861952428118);
    A[1].push_back(-0.2769996701683155);
    A[1].push_back(-0.7271861952428118);
    A[1].push_back(-0.2769996701683155);
    A[1].push_back(-0.06890464691960219);
    A[1].push_back(-0.09748519653650334);
    A[1].push_back(-0.06890464691960219);
    A[2].push_back(-0.09748519653650334);
    A[2].push_back(-0.7271861952428118);
    A[2].push_back(-10.21830350832508);
    A[2].push_back(-0.06890464691960219);
    A[2].push_back(-0.2769996701683155);
    A[2].push_back(-0.7271861952428118);
    A[2].push_back(-0.03280392761204531);
    A[2].push_back(-0.06890464691960219);
    A[2].push_back(-0.09748519653650334);
    A[3].push_back(-0.7271861952428118);
    A[3].push_back(-0.2769996701683155 );
    A[3].push_back(-0.06890464691960219);
    A[3].push_back(-10.21830350832508);
    A[3].push_back(-0.7271861952428118);
    A[3].push_back(-0.09748519653650334);
    A[3].push_back(-0.7271861952428118);
    A[3].push_back(-0.2769996701683155);
    A[3].push_back(-0.06890464691960219);
    A[4].push_back(-0.2769996701683155);
    A[4].push_back(-0.7271861952428118);
    A[4].push_back(-0.2769996701683155);
    A[4].push_back(-0.7271861952428118);
    A[4].push_back(-10.21830350832508);
    A[4].push_back(-0.7271861952428118);
    A[4].push_back(-0.2769996701683155);
    A[4].push_back(-0.7271861952428118);
    A[4].push_back(-0.2769996701683155);
    A[5].push_back(-0.06890464691960219);
    A[5].push_back(-0.2769996701683155);
    A[5].push_back(-0.7271861952428118);
    A[5].push_back(-0.09748519653650334);
    A[5].push_back(-0.7271861952428118);
    A[5].push_back(-10.21830350832508);
    A[5].push_back(-0.06890464691960219);
    A[5].push_back(-0.2769996701683155);
    A[5].push_back(-0.7271861952428118);
    A[6].push_back(-0.09748519653650334);
    A[6].push_back(-0.06890464691960219);
    A[6].push_back(-0.03280392761204531);
    A[6].push_back(-0.7271861952428118);
    A[6].push_back(-0.2769996701683155);
    A[6].push_back(-0.06890464691960219);
    A[6].push_back(-10.21830350832508);
    A[6].push_back(-0.7271861952428118);
    A[6].push_back(-0.09748519653650334);
    A[7].push_back(-0.06890464691960219);
    A[7].push_back(-0.09748519653650334);
    A[7].push_back(-0.06890464691960219);
    A[7].push_back(-0.2769996701683155);
    A[7].push_back(-0.7271861952428118);
    A[7].push_back(-0.2769996701683155);
    A[7].push_back(-0.7271861952428118);
    A[7].push_back(-10.21830350832508);
    A[7].push_back(-0.7271861952428118);
    A[8].push_back(-0.03280392761204531);
    A[8].push_back(-0.06890464691960219);
    A[8].push_back(-0.09748519653650334);
    A[8].push_back(-0.06890464691960219);
    A[8].push_back(-0.2769996701683155);
    A[8].push_back(-0.7271861952428118);
    A[8].push_back(-0.09748519653650334);
    A[8].push_back(-0.7271861952428118);
    A[8].push_back(-10.21830350832508);
    A[9].push_back(1);
    A[9].push_back(0);
    A[9].push_back(0);
    A[9].push_back(0);
    A[9].push_back(0);
    A[9].push_back(0);
    A[9].push_back(0);
    A[9].push_back(0);
    A[9].push_back(0);
    A[10].push_back(0);
    A[10].push_back(1);
    A[10].push_back(0);
    A[10].push_back(0);
    A[10].push_back(0);
    A[10].push_back(0);
    A[10].push_back(0);
    A[10].push_back(0);
    A[10].push_back(0);
    A[11].push_back(0);
    A[11].push_back(0);
    A[11].push_back(1);
    A[11].push_back(0);
    A[11].push_back(0);
    A[11].push_back(0);
    A[11].push_back(0);
    A[11].push_back(0);
    A[11].push_back(0);
    A[12].push_back(0);
    A[12].push_back(0);
    A[12].push_back(0);
    A[12].push_back(1);
    A[12].push_back(0);
    A[12].push_back(0);
    A[12].push_back(0);
    A[12].push_back(0);
    A[12].push_back(0);
    A[13].push_back(0);
    A[13].push_back(0);
    A[13].push_back(0);
    A[13].push_back(0);
    A[13].push_back(1);
    A[13].push_back(0);
    A[13].push_back(0);
    A[13].push_back(0);
    A[13].push_back(0);
    A[14].push_back(0);
    A[14].push_back(0);
    A[14].push_back(0);
    A[14].push_back(0);
    A[14].push_back(0);
    A[14].push_back(1);
    A[14].push_back(0);
    A[14].push_back(0);
    A[14].push_back(0);
    A[15].push_back(0);
    A[15].push_back(0);
    A[15].push_back(0);
    A[15].push_back(0);
    A[15].push_back(0);
    A[15].push_back(0);
    A[15].push_back(1);
    A[15].push_back(0);
    A[15].push_back(0);
    A[16].push_back(0);
    A[16].push_back(0);
    A[16].push_back(0);
    A[16].push_back(0);
    A[16].push_back(0);
    A[16].push_back(0);
    A[16].push_back(0);
    A[16].push_back(1);
    A[16].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(0);
    A[17].push_back(1);
    b.push_back(-2.234087037518483);
    b.push_back(-6.370072142271482);
    b.push_back(-2.314137495496334);
    b.push_back(-4.195003918678363);
    b.push_back(-2.89337934873546);
    b.push_back(-7.706424278239382);
    b.push_back(-7.357001197283779);
    b.push_back(-5.483351507948527);
    b.push_back(-7.286096857845781);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    b.push_back(1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);
    c.push_back(-1);

    Simplex teste = Simplex(A,b,c);
    std::vector<float> x = teste.solve();

    std::vector<float> sol;

    sol.push_back(0.137466);
    sol.push_back(0.55876);
    sol.push_back(0.122811);
    sol.push_back(0.309627);
    sol.push_back(0.104059);
    sol.push_back(0.659453);
    sol.push_back(0.650444);
    sol.push_back(0.405394);
    sol.push_back(0.620769);

    return !vector_within_delta(x,sol,DELTA);


}

bool test_with_real_arduino_values(){

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

        std::vector<float> sol;
        sol.push_back(11.33333);
        sol.push_back(3.33333);

        return !vector_within_delta(x,sol,DELTA);
}

int test_main( int, char *[] ){             // note the name!
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
    if(test_lab_08_ex_random_pwm())
        BOOST_ERROR( "Test Simplex Lab random PWM" );
    if(test_with_real_arduino_values())
        BOOST_ERROR( "Test Simplex with O and E matrix from Arduino" );

    return 0;
}
