//#pragma once

#include <vector>
#include <set>
#include <map>

typedef struct pivot_struct{
    std::map<int,std::map<int,float>> A;
    std::map<int,float> b;
    std::map<int,float> c;
    std::set<int> N;
    std::set<int> B;
    int v;
} pivot_struct;


pivot_struct pivot(pivot_struct in, int e, int l);
