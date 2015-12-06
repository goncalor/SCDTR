#include "simplex.hpp"
#include <iostream>


Simplex::Simplex(std::vector<std::vector<float>> A, std::vector<float> b, std::vector<float> c){



    for(unsigned int i=1;i<=c.size();i++){
        internal_struct.N.insert(i);
    }

    for(unsigned int i=c.size()+1;i<=c.size()+b.size();i++){
        internal_struct.B.insert(i);
    }

    internal_struct.v=0;


    for (auto i : internal_struct.B) {
        internal_struct.A[i]=std::map<int,float>();
    }
    auto iter_l = A.begin();
    for (auto i : internal_struct.B) {
        auto iter_c = (*iter_l).begin();
        for (auto j : internal_struct.N) {
            internal_struct.A[i][j]=(*iter_l)[*iter_c];
            iter_c++;
        }
        iter_l++;
    }

    auto iter = b.begin();
    for (auto i : internal_struct.B) {
        internal_struct.b[i]=*iter;
        ++iter;
    }
    auto iter_a = c.begin();
    for (auto i : internal_struct.N) {
        internal_struct.c[i]=*iter_a;
        ++iter_a;
    }

}

//Pivot method used by the Simplex algorithm based on the pseudo code present
// in "Introduction to Alorithms" 3rd Edition by Cormen, page 869.
Simplex::pivot_struct Simplex::pivot(pivot_struct in, int e, int l){
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

void Simplex::print_pivot_struct(pivot_struct to_print){
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


void Simplex::init_Simplex(){

}

std::vector<float> Simplex::solve(){
    std::vector<float> d;
    init_Simplex();

    return d;
}
