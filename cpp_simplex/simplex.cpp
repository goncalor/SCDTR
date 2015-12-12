#include "simplex.hpp"
#include <iostream>

#define INF 999999

//#define DEGUB

#define APPROX_ZERO 0.000001

float filter_zero(float to_filter){
    if (to_filter<APPROX_ZERO && to_filter>-APPROX_ZERO)
        return 0;
    return to_filter;

}


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
        if (j != e) {
            out.A[e][j] = in.A[l][j]/in.A[l][e];
            out.A[e][j] = filter_zero(out.A[e][j]);
        }
    }

    out.A[e][l]=1/in.A[l][e];

    //std::cout << "Updated A line " << e <<std::endl;
    //print_pivot_struct(out);

    for (auto i : in.B) {
        if (i != l){
            out.b[i] = in.b[i]-in.A[i][e]*out.b[e];
            out.b[i] = filter_zero(out.b[i]);
            for (auto j : in.N) {
                if (j != e){
                    out.A[i][j] = in.A[i][j]-in.A[i][e]*out.A[e][j];
                    out.A[i][j] = filter_zero(out.A[i][j]);
                }
            }
            out.A[i][l] = -in.A[i][e]*out.A[e][l];
            out.A[i][l] = filter_zero(out.A[i][l]);
        }
    }

    //print_pivot_struct(out);

    out.v = in.v+in.c[e]*out.b[e];
    out.v = filter_zero(out.v);

    for (auto j : in.N) {
        if (j != e){
            out.c[j] = in.c[j]- in.c[e]*out.A[e][j];
            out.c[j] = filter_zero(out.c[j]);
        }
    }

    out.c[l]=-in.c[e]*out.A[e][l];
    out.c[l] = filter_zero(out.c[l]);


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
        //std::cout << "b " << iter->first << " = " << iter->second <<std::endl;
        if(iter->second<min){
            iter_min=iter;
            min = iter->second;
        }
    }

    //std::cout << "min b = " << min <<std::endl;

    if (min < 0){
        // Unfeasible initial solution

        std::cout << "Unfeasible initial solution." << std::endl <<std::endl;


        //create auxiliar linear programm
        pivot_struct aux_programm(internal_struct);
        aux_programm.N.insert(0);
        for (auto j : aux_programm.N) {
            aux_programm.c[j] = 0;
        }
        aux_programm.c[0]=-1;
        for (auto j : aux_programm.A) {
            aux_programm.A[j.first][0]=-1;
        }

        std::cout << "Aux Linear programm "<< std::endl;
        print_pivot_struct(aux_programm);

        std::map<int,float> delta;
        int e = 0;
        int l = internal_struct.N.size() + internal_struct.B.size();

        l = *aux_programm.B.begin();
        float min = internal_struct.b[l];
        for(auto i : internal_struct.B){
            if(internal_struct.b[i] < min){
                min = internal_struct.b[i] ;
                l = i;
            }
        }

        std::cout << "Pivot: e = " << e << " l = " << l << std::endl;

        aux_programm = pivot(aux_programm,e,l);

        std::cout << "Pivot Result "<< std::endl;
        print_pivot_struct(aux_programm);

        while(!simplex_ended(aux_programm)){

            for(auto N_iter : aux_programm.N){
            //std::cout << "c[" << N_iter << "] = " << aux_programm.c[N_iter] << std::endl;
                if(aux_programm.c[N_iter]>APPROX_ZERO){
                    e=N_iter;
                    break;
                }
            }
            //std::cout << "e = " << e << std::endl;

            for(auto i : aux_programm.B){
                //std::cout << "A[" << i<< "]["<<e<<"] = "<< aux_programm.A[i][e] << std::endl;
                if(aux_programm.A[i][e]>APPROX_ZERO){
                    //delta[i] = aux_programm.b[i]/aux_programm.A[i][e];
                    if(aux_programm.b[i] > APPROX_ZERO || aux_programm.b[i] < -APPROX_ZERO ){
                        delta[i] = aux_programm.b[i]/aux_programm.A[i][e];
                    } else {
                        delta[i] = 0;
                    }
                } else {
                    delta[i] = INF;
                }
                //std::cout << "Delta (" << i << ") = " << delta[i] << std::endl;
            }

            l = *aux_programm.B.begin();
            float min = delta[l];
            for(auto i : aux_programm.B){
                if(delta[i] < min){
                    min = delta[i];
                    l = i;
                }
            }
            if(delta[l]>=INF){
                throw 2;
            }

            std::cout << "Pivot: e = " << e << " l = " << l << std::endl;

            aux_programm = pivot(aux_programm,e,l);

            std::cout << "Pivot Result "<< std::endl;
            print_pivot_struct(aux_programm);

        }

        if(aux_programm.v < APPROX_ZERO && aux_programm.v > -APPROX_ZERO ){
            std::cout << "There is a feasible solution." << std::endl;
            if(aux_programm.N.find(0)==aux_programm.N.end()){
                std::cout << "x0 is not a basic varialbe." << std::endl;
                //perform one (degenerate) pivot to make it nonbasic

                // we perform a pivot step to remove x0 from the basis, using any e in N such that a_0e != 0 as the entering variable
                for (auto j : aux_programm.N) {
                    if(aux_programm.A[0][j]!=0){
                        e=j;
                        break;
                    }
                }
                l=0;

                //std::cout << "Pivot: e = " << e << " l = " << l << std::endl;

                aux_programm = pivot(aux_programm,e,l);

                //std::cout << "Pivot Result "<< std::endl;
                //print_pivot_struct(aux_programm);

            }else{
                std::cout << "x0 is already a basic variable." << std::endl;
            }
            //from the final slack form of Laux , remove x0 from the constraints and restore the original objective function of L, but replace each basic variable in this objective function by the right-hand side of its associated constrain

            //   auto it = aux_programm.c.find(0);
            //   aux_programm.c.erase(it);

            for (auto j : aux_programm.N) {
                aux_programm.c.erase(j);
            }
            aux_programm.N.erase(0);

            /*for(auto j : internal_struct.N) {
                auto it = aux_programm.c.find(j);
                if(it!=aux_programm.c.end()){
                    // found a non basic varialble in aux_programm that is basic in the internal_struct
                    aux_programm.c[it] =


                }
            }*/
            aux_programm.v=internal_struct.v;

            for(auto i : internal_struct.N){

                if(aux_programm.B.find(i)!=aux_programm.B.end()){
                    std::cout << "v: adding inter_c["<< i<<"]*aux_b[" << i << "] = " << internal_struct.c[i] * aux_programm.b[i] <<std::endl;
                    aux_programm.v = internal_struct.c[i] * aux_programm.b[i]; //TODO FIX THIS
                    std::cout << "aux_v = " << aux_programm.v << std::endl;
                }/*else{
                    aux_programm.c[i]+=internal_struct.c[i];
                    for(auto j : internal_struct.N){
                        if(aux_programm.B.find(j)!=aux_programm.B.end()){
                            aux_programm.c[i]+=-internal_struct.c[j]*aux_programm.A[j][i];
                        }
                    }
                }*/
            }

            for(auto i : aux_programm.N){
                #if defined DEGUB
                    std::cout << "Calculating c["<<i<<"]"<<std::endl;
                    std::cout << "c["<<i<<"] = 0;"<<std::endl;
                #endif
                aux_programm.c[i]=0;
                if(internal_struct.N.find(i)!=internal_struct.N.end()){
                    #if defined DEGUB
                        std::cout << "c["<<i<<"] += "<< internal_struct.c[i] <<std::endl;
                    #endif
                    aux_programm.c[i]+=internal_struct.c[i];
                }
                for(auto j : internal_struct.N){
                    if(aux_programm.B.find(j)!=aux_programm.B.end()){
                        aux_programm.c[i]+=-internal_struct.c[j]*aux_programm.A[j][i];
                        #if defined DEGUB
                            std::cout << "c["<<i<<"] += "<< -internal_struct.c[j]*aux_programm.A[j][i] <<std::endl;
                        #endif
                    }
                }
            }
            std::cout << std::endl <<"New LP to solve: "<< std::endl;
            print_pivot_struct(aux_programm);

                internal_struct.A = aux_programm.A;
                internal_struct.b = aux_programm.b;
                internal_struct.c = aux_programm.c;
                internal_struct.N = aux_programm.N;
                internal_struct.B = aux_programm.B;
                internal_struct.v = aux_programm.v;





        }else{
            throw  1;
        }
    }


}

bool Simplex::simplex_ended( pivot_struct to_test){

    for (auto i : to_test.N) {
        if(to_test.c[i] > APPROX_ZERO){
            return false;
        }
    }

    return true;
}

std::vector<float> Simplex::solve(){
    std::vector<float> d(internal_struct.N.size(),0);
    try{
        init_Simplex();

        std::map<int,float> delta;
        int e;
        int l;

        std::cout << "Simplex Solve: "<< std::endl;
        print_internal_struct();

        while(!simplex_ended(internal_struct)){

            for(auto N_iter : internal_struct.N){
                if(internal_struct.c[N_iter]>APPROX_ZERO)
                    e=N_iter;
                    break;
            }

            for(auto i : internal_struct.B){
                //std::cout << "A[" << i<< "]["<<e<<"] = "<< internal_struct.A[i][e] << std::endl;
                if(internal_struct.A[i][e]>APPROX_ZERO){
                    if(internal_struct.b[i] > APPROX_ZERO || internal_struct.b[i] < -APPROX_ZERO ){
                        delta[i] = internal_struct.b[i]/internal_struct.A[i][e];
                    } else {
                        delta[i] = 0;
                    }
                    // delta[i] = internal_struct.b[i]/internal_struct.A[i][e];
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

            std::cout << "Pivot: e = " << e << " l = " << l << std::endl;

            internal_struct = pivot(internal_struct,e,l);

            std::cout << "Pivot Result "<< std::endl;
            print_internal_struct();

        }

        for(unsigned int i = 1; i<=d.size();i++){ 
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
