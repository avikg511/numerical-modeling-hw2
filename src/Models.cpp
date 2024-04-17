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
#include <filesystem>

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

void LAPendulumModels::outputData(std::filesystem::path fileName) {
    assert(fileName.extension() == ".csv");
    std::ofstream myFile (fileName);
    
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

void LAPendulumModels::calcFE() {
    double x1NPlus1;
    double x2NPlus1;
    
    std::vector<double>::iterator x1Iter;
    std::vector<double>::iterator x2Iter;
    
    /*
     The below for loop is a bit confusing, so here's some added help.
     
     We are calculating x1NPlus1, x2NPlus2 and putting that in the vector.
     Iterators are at the next element after the beginning (line 1 of for loop), and we're incrementing both (line 2 of for loop), and then incrementing them (line 3 of for loop).
     
     n is the index just like the math. Since our iterator points to the (n+1)th element, we can find the nth elements of the arrays and use them.
     */
    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ ) 
    {
        x1NPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
        x2NPlus1 = x2Data.at(n) - sin(x1Data.at(n)) * dt;
        
        *x1Iter = x1NPlus1;
        *x2Iter = x2NPlus1;
    }
}
void LAPendulumModels::calcIEPredCorr() {
    double x1TestNPlus1;
    double x1NPlus1;
    double x2NPlus1;
    
    std::vector<double>::iterator x1Iter;
    std::vector<double>::iterator x2Iter;
    
    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ )
    {
        x1TestNPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
        
        x2NPlus1 = x2Data.at(n) - sin(x1TestNPlus1) * dt;
        x1NPlus1 = x1Data.at(n) + x2NPlus1 * dt;
        
        *x1Iter = x1NPlus1;
        *x2Iter = x2NPlus1;
    }
}
void LAPendulumModels::calcSIEPredCorr() {
    double x1TestNPlus1;
    double x1NPlus1;
    double x2NPlus1;
    
    std::vector<double>::iterator x1Iter;
    std::vector<double>::iterator x2Iter;
    
    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ )
    {
        x1TestNPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
        
        x2NPlus1 = x2Data.at(n) - ( sin(x1Data.at(n)) + sin(x1TestNPlus1) ) * dt / 2;
        x1NPlus1 = x1Data.at(n) + (x2NPlus1 + x2Data.at(n)) * dt / 2;
        
        *x1Iter = x1NPlus1;
        *x2Iter = x2NPlus1;
    }
}
void LAPendulumModels::calcLFrog() {
    double x1NPlus1;
    double x2NPlus1;
    
    std::vector<double>::iterator x1Iter;
    std::vector<double>::iterator x2Iter;
    
    int n = 0;
    
//    First iteration with FE
    x1NPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
    x2NPlus1 = x2Data.at(n) - sin(x1Data.at(n)) * dt;
    
//    Add FE values to array and adjust n for the for loop
    *std::next(x1Data.begin()) = x1NPlus1;
    *std::next(x2Data.begin()) = x2NPlus1;
    n++;
    
    for ( x1Iter = std::next(x1Data.begin(), 2), x2Iter = std::next(x2Data.begin(), 2);
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ )
    {
        x1NPlus1 = x1Data.at(n - 1) + 2 * x2Data.at(n) * dt;
        x2NPlus1 = x2Data.at(n - 1) - 2 * sin(x1Data.at(n)) * dt;
        
        *x1Iter = x1NPlus1;
        *x2Iter = x2NPlus1;
    }
}
void LAPendulumModels::calcFourthRungeKutta() {
    
}
