
#include <vector>



struct lp_struct{
    std::vector<std::vector<float>> A;
    std::vector<float> b;
    std::vector<float> c;
};

lp_struct generate_linear_programm(std::vector<std::vector<float>> E, std::vector<float> O, std::vector<float> L);
