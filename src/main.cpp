#include <iostream>
#include "Models.h"

#include <cmath>
#include <filesystem>

int main() {
    double timestep = 0.25;
    int duration = 70;
    double initX1 = acos(-1) / 4;
    double initX2 = 0;

    LAPendulumModels model = LAPendulumModels(timestep, duration, initX1, initX2);
    std::cout << model.getModelParams();

    model.calcFE();
    std::filesystem::path FEPath ("./FEData.csv");
    model.outputData(FEPath);

    model.calcIEPredCorr();
    std::filesystem::path IEPredCorrPath ("./IEPredCorrData.csv");
    model.outputData(IEPredCorrPath);

    model.calcSIEPredCorr();
    std::filesystem::path SIEPredCorrPath ("./SIEPredCorrData.csv");
    model.outputData(SIEPredCorrPath);

    model.calcLFrog();
    std::filesystem::path LFrogPath ("./LFrogData.csv");
    model.outputData(LFrogPath);
}