cmake -B ./build -S . -DCMAKE_BUILD_TYPE=Debug;
cd build;
rm 'FEData.csv';
rm 'IEPredCorrData.csv';
rm 'SIEPredCorrData.csv';
rm 'LFrogData.csv';
rm 'RK4Data.csv';

make;
./LAPendulumModel;

# Can add set yr [-2:2] for y range

gnuplot -persist <<-EOFMarker
	reset session
	set datafile separator ","
	set yr [-20:6]
	
	set term qt 0
	plot 'FEData.csv' ;
	set xlabel 'log(dt) (s)'
	set ylabel 'log(RMS Error)'

	set term qt 1
	plot 'IEPredCorrData.csv' 
	set xlabel 'log(dt) (s)'
	set ylabel 'log(RMS Error)'

	set term qt 2
	plot 'SIEPredCorrData.csv'  
	set xlabel 'log(dt) (s)'
	set ylabel 'log(RMS Error)'

	set term qt 3
	plot 'LFrogData.csv'  
	set xlabel 'log(dt) (s)'
	set ylabel 'log(RMS Error)'

	set term qt 4
	plot 'RK4Data.csv'  
	set xlabel 'log(dt) (s)'
	set ylabel 'log(RMS Error)'

	quit
EOFMarker

cd ..;
