//#pragma once

#include <vector>
#include <set>
#include <map>



class simplex {


public:

    struct pivot_struct{
        std::map<int,std::map<int,float>> A;
        std::map<int,float> b;
        std::map<int,float> c;
        std::set<int> N;
        std::set<int> B;
        int v;
    };

    simplex(std::vector<std::vector<float>>,std::vector<float>,std::vector<float>);

    std::vector<float> solve();

    static pivot_struct pivot(pivot_struct in, int e, int l);

    static void print_pivot_struct(pivot_struct to_print);

    void init_simplex();

private:
    pivot_struct internal_struct;
};
