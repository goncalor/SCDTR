#include <vector>


bool is_within_delta(double a, double b, double delta){
    return (a<(b+delta) && a >(b-delta));
}

bool vector_within_delta(std::vector<float> a,std::vector<float> b,double delta){
    if(a.size()!=b.size()){
        return false;
    }
    for(unsigned int i = 0; i<a.size();i++){
        if(!is_within_delta(a[i],b[i],delta)){
            return false;
        }
    }
    return true;
}
