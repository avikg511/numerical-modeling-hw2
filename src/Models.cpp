//
//  models.cpp
//  homework2
//
//  Created by Avik Ghosh on 4/13/24.
//
#include "Models.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

/*
    Constructor: Defines timestep, duration, and initializes the vectors with length duration / timestep and our given initial values.

    The vector is filled with NAN (not a number) to allow for LAPendulumModels::outputData() to know when to stop outputting data. 

    Currently, the length is subject to some floating point errors, so that needs to be tested later on.
*/
LAPendulumModels::LAPendulumModels(double timestep, int duration, double initX1, double initX2)
{
    dt = timestep;
    dur = duration;
    
    x1Data = std::vector<double>(dur / dt, NAN);
    x2Data = std::vector<double>(dur / dt, NAN);
    
    *x1Data.begin() = initX1;
    *x2Data.begin() = initX2;
}

/*
    LAPendulumModels::getModelParams(void) -> std::string
        Provides debug string to define the method. 

        TODO: Look at overloading the << parameter in the future if it makes sense.
*/
std::string LAPendulumModels::getModelParams(void) {
    std::stringstream modelParams;
    modelParams << "Timestep: " << std::setprecision(7) << dt << " Duration: " << dur << " Initial X: " << x1Data.at(0) << " Initial X Prime: " << x2Data.at(0) << "\n";
    
    return modelParams.str();
}

// Duration and Timestep must be positive.
void LAPendulumModels::setDuration(double duration) {
    assert(duration > 0);
    dur = duration;
}

void LAPendulumModels::setTimestep(double timestep) {
    assert(timestep > 0);
    dt = timestep;
}

/*
    LAPendulumModels::outputData() -> void
        Currently outputs data to a file called "data.csv" for plotting using gnuplot, Excel, etc.

        Checks for vector values that are NAN to ensure data is correct. Saves us from issues with gnuplot, Excel, etc. not recognizing how to plot a NAN value. Outputs some debug messages to console if needed.
*/
void LAPendulumModels::outputData() {
    std::ofstream myFile ("data.csv");
    for (int i = 0; i < x1Data.size(); i++) {
        if (x1Data.at(i) != NAN) {
            myFile << i * dt << "," << x1Data.at(i) << "\n";
        } else {
            std::cout << "EOF reached or NAN values stored at end of vector\n";
            myFile.close();
            return;
        }
    }
    std::cout << "EOF reached\n";
    myFile.close();
    return;
}