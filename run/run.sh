cd ../build &&
make &&
cp src/main ../run &&
cp src/analyze ../run &&
cd ../run &&
echo -n "Choose action (1. Simulate, 2. Analyze ) > "
read num
if [[ num -eq 1 ]]; then
  mpirun -np 2 music sim.music
elif [[ num -eq 2 ]]; then
  ./analyze
fi
