#include "simplex.hpp"
#include <iostream>


//Pivot method used by the simplex algorithm based on the pseudo code present
// in "Introduction to Alorithms" 3rd Edition by Cormen, page 869.
pivot_struct pivot(pivot_struct in, int e, int l){
    pivot_struct out(in);


    out.N.erase(e);
    out.N.insert(l);
    out.B.erase(l);
    out.B.insert(e);

    out.b[e]=in.b[l]/in.A[l][e];
    for (auto j : in.N) {
        if (j != e)
        out.A[e][j] = in.A[l][j]/in.A[l][e];
    }



    out.A[e][l]=1/in.A[l][e];

    std::cout << "Updated A line " << e <<std::endl;
    print_pivot_struct(out);


    for (auto i : in.B) {
        if (i != l){
            out.b[i] = in.b[i]-in.A[i][e]*out.b[e];
            for (auto j : in.N) {
                if (j != e)
                out.A[i][j] = in.A[i][j]-in.A[i][e]*out.A[e][j];
            }
            out.A[i][l] = -in.A[i][e]*out.A[e][l];
        }
    }

    print_pivot_struct(out);


    out.v = in.v+in.c[e]*out.b[e];

    for (auto j : in.N) {
        if (j != e)
        out.c[j] = in.c[j]- in.c[e]*out.A[e][j];
    }

    out.c[l]=-in.c[e]*out.A[e][l];


    return out;
}

void print_pivot_struct(pivot_struct to_print){
    std::cout << "N = {";
    for (auto j : to_print.N) {
        std::cout << j << ",";
    }
    std::cout << "}" <<std::endl;

    std::cout << "B = {" ;
    for (auto j : to_print.B) {
        std::cout << j << ",";
    }
    std::cout << "}" <<std::endl;

    std::cout << "A = {" << std::endl;
    for (auto i : to_print.B) {
        for (auto j : to_print.N) {
            std::cout << to_print.A[i][j] << ",";
            //std::cout <<"A_"<< i << j <<" = " << to_print.A[i][j] << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "}" <<std::endl;

    std::cout << "b = {" ;
    for (auto j : to_print.B) {
        std::cout << to_print.b[j] << ",";
    }
    std::cout << "}" <<std::endl;

    std::cout << "c = {" ;
    for (auto j : to_print.N) {
        std::cout << to_print.c[j] << ",";
    }
    std::cout << "}" <<std::endl;

    std::cout << "c = " << to_print.v << std::endl << std::endl;

}
