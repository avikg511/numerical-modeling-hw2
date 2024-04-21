cmake -B ./build -S . -DCMAKE_BUILD_TYPE=Debug;
cd build;
make;
./LAPendulumModel;

gnuplot -persist <<-EOFMarker
	reset session
	set yr [-2:2]
	set datafile separator ","

	set term qt 0
	plot 'IEPredCorrData.csv' w l;

	set term qt 1
	plot 'FEData.csv' w l ;

	set term qt 2
	plot 'SIEPredCorrData.csv' w l 

	set term qt 3
	plot 'LFrogData.csv' w l 

	set term qt 4
	plot 'RK4Data.csv' w l 
	quit
EOFMarker

cd ..;
