#include <iostream>
#include "Models.h"

#include <cmath>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iterator>
#include <fstream>
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

void calculateRMS() {
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

void compareRMSWithdt(double dt, std::vector<double>& errors, LAPendulumModels& gt) {
    const double ModelTimestep = dt;
    const int ModelDuration = 10;
    double ModelInitX1 = acos(-1) / 4;
    double ModelInitX2 = 0;
    LAPendulumModels model = LAPendulumModels(ModelTimestep, ModelDuration, ModelInitX1, ModelInitX2);

    std::vector<double>::iterator it = errors.begin();

    // Run Models and Calculate Time
    model.calcFE();
    *it = model.calculateRMSWRTGT(gt);
    std::advance(it, 1);

    model.calcIEPredCorr();
    *it = model.calculateRMSWRTGT(gt);
    std::advance(it, 1);

    model.calcSIEPredCorr();
    *it = model.calculateRMSWRTGT(gt);
    std::advance(it, 1);

    model.calcLFrog();
    *it = model.calculateRMSWRTGT(gt);
    std::advance(it, 1);

    model.calcFourthRungeKutta();
    *it = model.calculateRMSWRTGT(gt);
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


void compareRMSWithCalcTime(double dt, std::vector<double>& errors, std::vector<double>& times, LAPendulumModels& groundTruthIsh) {
    const double ModelTimestep = dt;
    const int ModelDuration = 10;
    double ModelInitX1 = acos(-1) / 4;
    double ModelInitX2 = 0;
    LAPendulumModels model = LAPendulumModels(ModelTimestep, ModelDuration, ModelInitX1, ModelInitX2);

    std::vector<double>::iterator itErr = errors.begin();
    std::vector<double>::iterator itTimes = times.begin();

    // Run Models and Calculate Time
    std::chrono::steady_clock::time_point FEbegin = std::chrono::steady_clock::now();
    model.calcFE();
    *itErr = model.calculateRMSWRTGT(groundTruthIsh);
    std::advance(itErr, 1);
    std::chrono::steady_clock::time_point FEend = std::chrono::steady_clock::now();
    *itTimes = std::chrono::duration_cast<std::chrono::microseconds>(FEend - FEbegin).count() / 1000000.0;
    std::advance(itTimes, 1);

    std::chrono::steady_clock::time_point IEbegin = std::chrono::steady_clock::now();
    model.calcIEPredCorr();
    *itErr = model.calculateRMSWRTGT(groundTruthIsh);
    std::advance(itErr, 1);
    std::chrono::steady_clock::time_point IEend = std::chrono::steady_clock::now();
    *itTimes = std::chrono::duration_cast<std::chrono::microseconds>(IEend - IEbegin).count() / 1000000.0;
    std::advance(itTimes, 1);

    std::chrono::steady_clock::time_point SIEbegin = std::chrono::steady_clock::now();
    model.calcSIEPredCorr();
    *itErr = model.calculateRMSWRTGT(groundTruthIsh);
    std::advance(itErr, 1);
    std::chrono::steady_clock::time_point SIEend = std::chrono::steady_clock::now();
    *itTimes = std::chrono::duration_cast<std::chrono::microseconds>(SIEend - SIEbegin).count() / 1000000.0;
    std::advance(itTimes, 1);

    std::chrono::steady_clock::time_point LFbegin = std::chrono::steady_clock::now();
    model.calcLFrog();
    *itErr = model.calculateRMSWRTGT(groundTruthIsh);
    std::advance(itErr, 1);
    std::chrono::steady_clock::time_point LFend = std::chrono::steady_clock::now();
    *itTimes = std::chrono::duration_cast<std::chrono::microseconds>(LFend - LFbegin).count() / 1000000.0;
    std::advance(itTimes, 1);

    std::chrono::steady_clock::time_point RK4begin = std::chrono::steady_clock::now();
    model.calcFourthRungeKutta();
    *itErr = model.calculateRMSWRTGT(groundTruthIsh);
    std::chrono::steady_clock::time_point RK4end = std::chrono::steady_clock::now();
    *itTimes = std::chrono::duration_cast<std::chrono::microseconds>(RK4end - RK4begin).count() / 1000000.0;
}

int main() {
    // exportData1A();
    // calculateRMS();

    std::vector<double> dtVals = {0.01, 0.03, 0.05, 0.07, 0.1, 0.13, 0.17, 0.20, 0.25, 0.3, 0.35, 0.4, 0.45};

    double GT_timestep = 1 / 1000000.0f;
    int GT_duration = 10;
    double GT_initX1 = acos(-1) / 4;
    double GT_initX2 = 0;
    LAPendulumModels groundTruthIsh = LAPendulumModels(GT_timestep, GT_duration, GT_initX1, GT_initX2);
    groundTruthIsh.calcFourthRungeKutta();


    std::vector<std::filesystem::path> fileNames = {
        std::filesystem::path("FEData.csv"),
        std::filesystem::path("IEPredCorrData.csv"),
        std::filesystem::path("SIEPredCorrData.csv"),
        std::filesystem::path("LFrogData.csv"),
        std::filesystem::path("RK4Data.csv")
    };
    int i = 0;
    for (double dt : dtVals) {
        std::vector<double> curErrors (5, 0);
        std::vector<double> curTimes (5, 0);
        compareRMSWithCalcTime(dt, curErrors, curTimes, groundTruthIsh);
        compareRMSWithdt(dt, curErrors, groundTruthIsh);
        int counter = 0;
        for (std::filesystem::path path : fileNames) {
            std::ofstream myFile;
            myFile.open(path, std::fstream::app);
            if (myFile.is_open()) {
                myFile << (curTimes.at(counter)) << "," << (curErrors.at(counter)) << "\n";
            }
            myFile.close();
            counter++;
        }
        std::cout << "Completed the " << dt << " run!" << std::endl;
        
    }
}

