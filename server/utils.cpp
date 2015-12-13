#include <vector>
#include "utils.hpp"

lp_struct generate_linear_programm(std::vector<std::vector<float>> E, std::vector<float> O, std::vector<float> L){
    lp_struct out;
    unsigned int n = O.size();
    out.b = std::vector<float>(2*n,0);
    out.c = std::vector<float>(n,0);

    for(unsigned int i = 0; i<n;i++){
        out.b[i] = O[i]-L[i];
        out.b[i+n] = 1;
        out.c[i] = -1;
        out.A.push_back(std::vector<float> ());
        out.A.push_back(std::vector<float> ());
    }
    for(unsigned int i = 0; i<n;i++){
        for(unsigned int j = 0; j<n;j++){
            out.A[i].push_back(-E[i][j]);
            if(i==j){
                out.A[i+n].push_back(1);
            }else{
                out.A[i+n].push_back(0);
            }

        }
    }

    return out;
}
