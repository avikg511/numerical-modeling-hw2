#include <iostream>
#include "Models.h"

#include <cmath>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <chrono>
#include <thread>
#include<unistd.h>


// Quick function to make sure the std::chrono is working properly on the system. Had previous issues where
// sleep() from time.h wasn't calculating the proper time.
void test_time() {
    std::cout << "Sleeping now . . . " << std::flush;;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Woke up" << std::endl;
    std::cout << "Calibration - Sleep for 1 second becomes: " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " seconds." << std::endl;
}
int main() {
    // Optional test to make sure the time is correctly calibrated
    // test_time();

    const double ModelTimestep = 0.01;
    const int ModelDuration = 10;
    double ModelInitX1 = acos(-1) / 4;
    double ModelInitX2 = 0;
    LAPendulumModels model = LAPendulumModels(ModelTimestep, ModelDuration, ModelInitX1, ModelInitX2);

    double GT_timestep = 1 / 1000000.0f;
    std::cout << GT_timestep << std::endl;
    int GT_duration = ModelDuration;
    double GT_initX1 = ModelInitX1;
    double GT_initX2 = ModelInitX2;
    LAPendulumModels groundTruthIsh = LAPendulumModels(GT_timestep, GT_duration, GT_initX1, GT_initX2);
    groundTruthIsh.calcFourthRungeKutta();

    std::cout << std::left << std::setw(30) << "Ground Truth-ish params\n" << groundTruthIsh.getModelParams() << std::endl << std::endl;
    std::cout << std::left << std::setw(30) << "General model params\n" << model.getModelParams() << std::endl;
    std::cout << std::internal << std::endl;

    // Run Models and Calculate Time
    std::chrono::steady_clock::time_point FEbegin = std::chrono::steady_clock::now();
    model.calcFE();
    std::chrono::steady_clock::time_point FEend = std::chrono::steady_clock::now();
    std::cout << std::setw(60) << "RMS Error Forward Euler: " << model.calculateRMSWRTGT(groundTruthIsh) << std::endl;

    std::chrono::steady_clock::time_point IEbegin = std::chrono::steady_clock::now();
    model.calcIEPredCorr();
    std::chrono::steady_clock::time_point IEend = std::chrono::steady_clock::now();
    std::cout << std::setw(60) << "RMS Error Implicit Euler Predictor Corrector: " << model.calculateRMSWRTGT(groundTruthIsh) << std::endl;

    std::chrono::steady_clock::time_point SIEbegin = std::chrono::steady_clock::now();
    model.calcSIEPredCorr();
    std::chrono::steady_clock::time_point SIEend = std::chrono::steady_clock::now();
    std::cout << std::setw(60) << "RMS Error Semi-implicit Euler Predictor Corrector: " << model.calculateRMSWRTGT(groundTruthIsh) << std::endl;

    std::chrono::steady_clock::time_point LFbegin = std::chrono::steady_clock::now();
    model.calcLFrog();
    std::chrono::steady_clock::time_point LFend = std::chrono::steady_clock::now();
    std::cout << std::setw(60) << "RMS Error Leap Frog: " << model.calculateRMSWRTGT(groundTruthIsh) << std::endl;

    std::chrono::steady_clock::time_point RK4begin = std::chrono::steady_clock::now();
    model.calcFourthRungeKutta();
    std::chrono::steady_clock::time_point RK4end = std::chrono::steady_clock::now();
    std::cout << std::setw(60) << "RMS Error Fourth Range Runge Kutta: " << model.calculateRMSWRTGT(groundTruthIsh) << std::endl;

    std::cout << std::endl;

    // Calculatetimes  
    double FEtime_spent = std::chrono::duration_cast<std::chrono::microseconds>(FEend - FEbegin).count() / 1000000.0;
    double IEtime_spent = std::chrono::duration_cast<std::chrono::microseconds>(IEend - IEbegin).count() / 1000000.0;
    double SIEtime_spent = std::chrono::duration_cast<std::chrono::microseconds>(SIEend - SIEbegin).count() / 1000000.0;
    double LFtime_spent = std::chrono::duration_cast<std::chrono::microseconds>(LFend - LFbegin).count() / 1000000.0;
    double RK4time_spent = std::chrono::duration_cast<std::chrono::microseconds>(RK4end - RK4begin).count() / 1000000.0;

    std::cout << std::setw(60) << "Time spent on Forward Euler: " << FEtime_spent << std::endl;
    std::cout << std::setw(60) << "Time spent on Implicit Euler Predictor Corrector: " << IEtime_spent << std::endl;
    std::cout << std::setw(60) << "Time spent on Semi-implicit Euler Predictor Corrector: " << SIEtime_spent << std::endl;
    std::cout << std::setw(60) << "Time spent on Leap Frog: " << LFtime_spent << std::endl;
    std::cout << std::setw(60) << "Time spent on Fourth Range Runge Kutta: " << RK4time_spent << std::endl;

    std::cout << std::endl << std::setw(60) << "-----End Re" << "sults------" << std::endl;
}


void exportData1A() {
    double timestep = 0.01;
    int duration = 100;
    double initX1 = acos(-1) / 4;
    double initX2 = 0;
    LAPendulumModels model = LAPendulumModels(timestep, duration, initX1, initX2);

    model.calcFE();
    std::filesystem::path FEPath ("./FEData.csv");
    model.exportData(FEPath);

    model.calcIEPredCorr();
    std::filesystem::path IEPredCorrPath ("./IEPredCorrData.csv");
    model.exportData(IEPredCorrPath);

    model.calcSIEPredCorr();
    std::filesystem::path SIEPredCorrPath ("./SIEPredCorrData.csv");
    model.exportData(SIEPredCorrPath);

    model.calcLFrog();
    std::filesystem::path LFrogPath ("./LFrogData.csv");
    model.exportData(LFrogPath);

    model.calcFourthRungeKutta();
    std::filesystem::path RK4Path ("./RK4Data.csv");
    model.exportData(RK4Path);
}