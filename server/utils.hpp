#include <vector>


#define RESISTENCIA 10000.0
#define LDR_B 4.7782
#define LDR_A 0.6901



struct lp_struct{
    std::vector<std::vector<float>> A;
    std::vector<float> b;
    std::vector<float> c;
};

lp_struct generate_linear_programm(std::vector<std::vector<float>> E, std::vector<float> O, std::vector<float> L);

int lux_to_pwm(float lux);
