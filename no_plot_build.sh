cmake -B ./build -S . -DCMAKE_BUILD_TYPE=Debug;
cd build;
make;
./LAPendulumModel;
cd ..;
