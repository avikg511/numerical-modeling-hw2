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
#include <numeric>
/*
    Constructor: Defines timestep, duration, and initializes the vectors with length duration / timestep and our given initial values.

    The vectors are filled with NAN (not a number) to allow for LAPendulumModels::outputData() to know when to stop outputting data (vectors sizes are not known at )

    Currently, the length is subject to some floating point errors, so that needs to be tested later on.
*/
LAPendulumModels::LAPendulumModels(const double timestep, const int duration, double initX1, double initX2)
{
    assert(timestep > 0 && "Timestep should be larger than 0");
    assert(duration > 0 && "Duration should be larger than 0");

    dt = timestep;
    dur = duration;
    
    x1Data = std::vector<double>(uint32_t(dur / dt), NAN);
    x2Data = std::vector<double>(uint32_t(dur / dt), NAN);
    timeSeries = std::vector<double>(uint32_t(dur / dt), NAN);

    std::iota(timeSeries.begin(), timeSeries.end(), 0);
    for (int i = 0; i < timeSeries.size(); i++) {
        timeSeries[i] *= dt;
    }

    timeSeries.erase(std::remove_if(std::begin(timeSeries), std::end(timeSeries), 
                                    [](const auto& value) { return std::isnan(value); }),
                                std::end(timeSeries));

    *x1Data.begin() = initX1;
    *x2Data.begin() = initX2;

    x1Init = initX1;
    x2Init = initX2;
}

/*
    LAPendulumModels::getModelParams(void) -> std::string
        Provides debug string to define the method. 

        TODO: Look at overloading the << parameter in the future if it makes sense.
*/
std::string LAPendulumModels::getModelParams(void) {
    std::stringstream modelParams;
    modelParams << std::setw(15) << "Timestep: " 
                << std::setw(10) << std::setprecision(7) << dt 
                << std::setw(15) << "\tDuration: " 
                << std::setw(10) << dur 
                << std::setw(15) << "\tInitial X: " 
                << std::setw(10) << x1Data.at(0) 
                << std::setw(15) << "\tInitial X Prime: " 
                << std::setw(10) << x2Data.at(0);
    
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
void LAPendulumModels::exportData() {
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

void LAPendulumModels::exportData(std::filesystem::path fileName) {
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
    
    // This is redundant but it's nice to see
    *x1Data.begin() = x1Init;
    *x2Data.begin() = x2Init;
    
    /*
     n is the index just like the math. Since our iterator points to the (n+1)th element, we can find the nth elements of the arrays and use them.
     */
    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ ) 
    {
        // FE Step
        x1NPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
        x2NPlus1 = x2Data.at(n) - sin(x1Data.at(n)) * dt;
        
        // Storing the data
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
    
    // This is redundant but it's nice to see
    *x1Data.begin() = x1Init;
    *x2Data.begin() = x2Init;
    
    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ )
    {
        // FE step, predictor corrector
        x1TestNPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
        
        // IE step
        x2NPlus1 = x2Data.at(n) - sin(x1TestNPlus1) * dt;
        x1NPlus1 = x1Data.at(n) + x2NPlus1 * dt;
        
        // Storing the data
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
    
    // This is redundant but it's nice to see
    *x1Data.begin() = x1Init;
    *x2Data.begin() = x2Init;
    
    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ )
    {
        // Predictor Corrector
        x1TestNPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
        
        // Semi-Implicit Euler
        x2NPlus1 = x2Data.at(n) - ( sin(x1Data.at(n)) + sin(x1TestNPlus1) ) * dt / 2;
        x1NPlus1 = x1Data.at(n) + (x2NPlus1 + x2Data.at(n)) * dt / 2;
        
        // Storing Data
        *x1Iter = x1NPlus1;
        *x2Iter = x2NPlus1;
    }
}
void LAPendulumModels::calcLFrog() {
    double x1NPlus1;
    double x2NPlus1;
    
    std::vector<double>::iterator x1Iter;
    std::vector<double>::iterator x2Iter;

    // This is redundant but it's nice to see
    *x1Data.begin() = x1Init;
    *x2Data.begin() = x2Init;
    
    int n = 0;
    
    // First iteration with FE
    x1NPlus1 = x1Data.at(n) + x2Data.at(n) * dt;
    x2NPlus1 = x2Data.at(n) - sin(x1Data.at(n)) * dt;
    
    // Add FE values to array and adjust n for the for loop
    *std::next(x1Data.begin()) = x1NPlus1;
    *std::next(x2Data.begin()) = x2NPlus1;
    n++;
    
    for ( x1Iter = std::next(x1Data.begin(), 2), x2Iter = std::next(x2Data.begin(), 2);
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ )
    {
        // Calcuate LFrog
        x1NPlus1 = x1Data.at(n - 1) + 2 * x2Data.at(n) * dt;
        x2NPlus1 = x2Data.at(n - 1) - 2 * sin(x1Data.at(n)) * dt;
        
        // Store values
        *x1Iter = x1NPlus1;
        *x2Iter = x2NPlus1;
    }
}
void LAPendulumModels::calcFourthRungeKutta() {
    // First midpoint for x1 and x2
    double x1MidPointA;
    double x2MidPointA;

    // Second midpoint for x1 and x2
    double x1MidPointB;
    double x2MidPointB;

    // End point estimate for x1 and x2
    double x1EndPointStar;
    double x2EndPointStar;

    // Final estimate for x1 and x2
    double x1FinalEst;
    double x2FinalEst;

    std::vector<double>::iterator x1Iter;
    std::vector<double>::iterator x2Iter;

    // This is redundant but it's nice to see
    *x1Data.begin() = x1Init;
    *x2Data.begin() = x2Init;

    int n = 0;
    for ( x1Iter = std::next(x1Data.begin()), x2Iter = std::next(x2Data.begin());
                (x1Iter != x1Data.end()) || (x2Iter != x2Data.end());
                 x1Iter++, x2Iter++, n++ ) 
    {
        // MidPoint A's
        x1MidPointA = x1Data.at(n) + x2Data.at(n) * dt / 2;
        x2MidPointA = x2Data.at(n) - sin(x1Data.at(n)) * dt / 2;

        // MidPoint B's
        x1MidPointB = x1Data.at(n) + x2MidPointA * dt / 2;
        x2MidPointB = x2Data.at(n) - sin(x1MidPointA) * dt / 2;

        // End Point estimate
        x1EndPointStar = x1Data.at(n) + x2MidPointB * dt;
        x2EndPointStar = x2Data.at(n) - sin(x1MidPointB) * dt;

        // Final Estimate
        x1FinalEst = x1Data.at(n) + dt * (x2Data.at(n) / 6 + x2MidPointA / 3 + x2MidPointB / 3 + x2EndPointStar / 6);
        x2FinalEst = x2Data.at(n) - dt * (sin(x1Data.at(n)) / 6 + sin(x1MidPointA) / 3 + sin(x1MidPointB) / 3 + sin(x1EndPointStar) / 6);

        // Input data into vector
        *x1Iter = x1FinalEst;
        *x2Iter = x2FinalEst;
    }
}

// Calculate RMS WRT (with respect to) GT (ground truth)
// Please note that this method only works for 
double LAPendulumModels::calculateRMSWRTGT(LAPendulumModels& groundTruth) {
    // Verify Models are compatible
    assert(groundTruth.dt < this->dt);
    assert(groundTruth.dur == this->dur);
    assert(groundTruth.x1Init == this->x1Init);
    assert(groundTruth.x2Init == this->x2Init);

    LAPendulumModels interpolatedGT = interpolate(groundTruth);
    
    double error = 0;
    for (int i = 0; i < interpolatedGT.x1Data.size(); i++) {
        // Calculate RMS error and return the sum
        error += std::pow((interpolatedGT.x1Data.at(i) - this->x1Data.at(i)), 2);
    }

    return sqrt(error / interpolatedGT.x1Data.size());
}

LAPendulumModels LAPendulumModels::interpolate(LAPendulumModels& groundTruth) {
    // Indices are an vector where index i of the vector refers to j, where i is the location of j as well as the model place j refers to. j instead refers to a location on the ground truth array that has the interpolated values.
    std::vector<int> indices (this->x1Data.size(), NAN);
    double curTime = 0;
    double newError = 0.0f;

    indices[0] = 0;
    for (int i = 1; i < this->x1Data.size(); i++) {
        curTime = i * this->dt;
        
        // Find lowest timestep that is greater than our current time
        auto it = std::lower_bound(groundTruth.timeSeries.begin(), groundTruth.timeSeries.end(), curTime);

        // The correct index would be one less than the greater than.
        indices[i] = std::distance(groundTruth.timeSeries.begin(), it) - 1;
        newError += abs(this->timeSeries[i] - groundTruth.timeSeries.at(indices[i] - 1));
    }

    // Stores data in vector called interpolatedData and returns a new instance with this data.
    std::vector<double> interpolatedData;
    std::transform(indices.begin(), indices.end(),
                 std::back_inserter(interpolatedData),
                 [groundTruth](int index) { return groundTruth.x1Data.at(index); });
                
    LAPendulumModels interpGT = LAPendulumModels(groundTruth);
    interpGT.x1Data = interpolatedData;
    interpGT.timeSeries = this->timeSeries;
    return interpGT;
}