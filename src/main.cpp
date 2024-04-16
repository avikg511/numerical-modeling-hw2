#include <iostream>
#include "Models.h"

#include <cmath>

int main() {
    double timestep = 0.01;
    int duration = 100;
    double initX1 = acos(-1) / 4;
    double initX2 = 0;

    LAPendulumModels model = LAPendulumModels(timestep, duration, initX1, initX2);
    std::cout << model.getModelParams();

}