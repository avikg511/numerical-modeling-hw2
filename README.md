## Description
This is Homework 2 of an unnamed numerical modeling course :)

In this homework, we're numerically solving a large amplitude pendulum of the form:

$$ \frac{\partial^2 x}{\partial t^2} = - \sin x$$

using Forward Euler,, Implicit Euler with a predictor corrector test step, Semi-implicit Euler with the predictor-corrector test step, Leapfrog, and the 4th-order Runge-Kutta. Then given some initial conditions, we'll plot the approximations, plot the errors, computation times, and test the effects of varying timesteps.

## Overview of the Code
### Directory
├── CMakeLists.     <br />
├── LICENSE         <br />
├── README.md       <br />
├── build.sh        <br />
├── include         <br />
│   └── Models.h   <br />
├── no_plot_build.h <br />
└── src             <br />
    ├── Models.     <br />cpp
    └── main.       <br />cpp

There is a main.cpp file that runs everything, and then a LAPendulumModels class that contains much of the code (defined in Models.cpp). A few heads-ups:

1. C++ plotting is not as common as it is in Python, so we use LAPendulumModels::outputData() to output the data to a csv file, before we use a terminal utility called gnuplot to plot them.
	1. To plot with gnuplot, type in gnuplot (install with brew gnuplot, etc.)
	1. Then, setup the separators (commas for csv) with "set datafile separator ','"
	1. Then, type in "plot 'data.csv'" and it will plot the results for you with collumn 1 as the x-axis, collumn 2 as the y-axis.
1. Each algorithm is separated into its own method within LAPendulumModels, e.g. LAPendulumModels::calcFE(). From here, the data is always stored in the std::vector<double>'s called x1Data and x2Data (from the general form of the ODEs we're solving).
	1. As far as understanding the modeling, probably ignore CMakeLists.txt
1. To use the code, you can run the ./no_plot_build.sh script. If you want to see the plots, you would have to modify the with the included function at the bottom of main.cpp
 
Run the code with ./build.sh. Part of me has given up on cross compatibility so I've just made one for OSX/Unix. Also, this script includes gnuplot, which is the terminal utility I'm using to graph things. This system would probably just break if gnuplot wasn't installed, so just try out a quick 'brew install gnuplot' on OSX (or 'sudo apt install gnuplot' for UNIX) if you want to run it!
