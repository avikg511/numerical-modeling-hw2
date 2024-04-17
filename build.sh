cmake -B ./build -S .;
cd build;
make;
./LAPendulumModel;

gnuplot -persist <<-EOFMarker
	set datafile separator ","
	plot 'IEPredCorrData.csv' w l, 'FEData.csv' w l, 'SIEPredCorrData.csv' w l, 'LFrogData.csv' w l;
	quit
EOFMarker

cd ..;
