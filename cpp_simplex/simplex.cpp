#include "simplex.hpp"
#include <iostream>

#define INF 999999

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
            internal_struct.A[i][j]=*iter_c;
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

    //std::cout << "Updated A line " << e <<std::endl;
    //print_pivot_struct(out);


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

    //print_pivot_struct(out);


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

    std::cout << "v = " << to_print.v << std::endl << std::endl;

}


void Simplex::init_Simplex(){

    auto iter_min = internal_struct.b.begin();
    float min = iter_min->second;
    for (auto iter = internal_struct.b.begin(); iter != internal_struct.b.end(); ++iter){
        if(iter->second<min){
            iter_min=iter;
            min = iter->second;
        }
    }

    if (min < 0){
        // Unfeasible initial solution
        throw  1;
    }


}

bool Simplex::simplex_ended(){

    for (auto i : internal_struct.N) {
        if(internal_struct.c[i] > 0){
            return false;
        }
    }

    return true;
}

std::vector<float> Simplex::solve(){
    std::vector<float> d(internal_struct.B.size(),0);
    try{
        init_Simplex();

        std::map<int,float> delta;
        int e;
        int l;

        while(!simplex_ended()){

            for(auto N_iter : internal_struct.N){
                if(internal_struct.c[N_iter]>0)
                    e=N_iter;
                    break;
            }

            for(auto i : internal_struct.B){
                //std::cout << "A[" << i<< "]["<<e<<"] = "<< internal_struct.A[i][e] << std::endl;
                if(internal_struct.A[i][e]>0){
                    delta[i] = internal_struct.b[i]/internal_struct.A[i][e];
                } else {
                    delta[i] = INF;
                }
                //std::cout << "Delta (" << i << ") = " << delta[i] << std::endl;
            }

            l = *internal_struct.B.begin();
            float min = delta[l];
            for(auto i : internal_struct.B){
                if(delta[i] < min){
                    min = delta[i];
                    l = i;
                }
            }
            if(delta[l]==INF){
                throw 2;
            }

            //std::cout << "Pivot: e = " << e << " l = " << l << std::endl;

            internal_struct = pivot(internal_struct,e,l);

            //std::cout << "Pivot Result "<< std::endl;
            //print_internal_struct();

        }

        for(unsigned int i = 0; i<internal_struct.B.size();i++){
            if (internal_struct.B.find(i)!=internal_struct.B.end()){
                d[i-1]=internal_struct.b[i];
            }
        }

    }
    catch(int e){
        if(e==1){
            std::cout << "Unfeasible initial solution!" << std::endl;
        }
        if(e==2){
            std::cout << "Unbounded!" << std::endl;
        }
    }

    return d;
}
