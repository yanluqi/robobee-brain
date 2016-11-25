cd ../build &&
make &&
cp src/main ../run &&
cd ../run &&
mpirun -np 2 music sim.music