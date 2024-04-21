//
//  models.h
//  homework2
//
//  Created by Avik Ghosh on 4/16/24.
//

#ifndef models_hpp
#define models_hpp

#include <stdio.h>
#include <vector>
#include <filesystem>

/*
    Class Description: LAPendulumModels (Large Amplitude Penulum Models), models referring to the numerical models that will be used to solve the problem. We'll be solving x'' = -sin(x).

    Variables: 
        dt: timestep, where dt refers to the differential dt w.r.t time (seconds)
        dur: duration, t_max (seconds)

        Both variables must be positive or setTimestep() and setDuration() will return errors (assert).

    Methods:
        setTimestep()/setDuration() 
            As named, but asserts that timestep and duration are both positive ( > 0)
        getModelParams() -> string. 
            Here, we return a string of the parameters of the model in case we want to double check what the model is actually working with at this point. This includes timestep, duration, as well as initial conditions.
        calcXXX() -> void
            Here we attempt to model the ODE above using method XXX. The methods are abbreviated as follows:
                FE: Forward Euler
                IEPredCorr: Implicit Euler (w/) Predictor Corrector
                SIEPredCorr: Semi Implicit Euler (w/) Predictor Corrector
                LFrog: Leap Frog
                FourthRungeKutta: Fourth Order Runge Kutta
        outputData() -> void
            Here, we output the vector of data (length = dur / dt) to a file currently labeled data.csv. The method of outputting the data may be updated throughout the next few commits.
    
    Use:
        After setting parameters (init values, timesteps, durations), we calculate each model's data at at time, and then output it to a csv file. Then, we can plot it with gnuplot, change what method we're plotting, etc.

*/
class LAPendulumModels {
public:
    LAPendulumModels(const double timestep, const int duration, double initX, double initXPrime);
    std::vector<double> x1Data;
    std::vector<double> x2Data;
    std::vector<double> timeSeries;
    void setTimestep(double timestep);
    void setDuration(double duration);
    void calcFE();
    void calcIEPredCorr();
    void calcSIEPredCorr();
    void calcLFrog();
    void calcFourthRungeKutta();
    void exportData();
    void exportData(std::filesystem::path fileName);
    double calculateRMSWRTGT(LAPendulumModels& groundTruth);
    LAPendulumModels interpolate(LAPendulumModels& groundTruth);
    std::string getModelParams();

    double dt;
    int dur;
    double x1Init;
    double x2Init;
};


#endif /* models_hpp */
