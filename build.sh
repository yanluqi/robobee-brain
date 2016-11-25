#!/bin/sh

if [ ! -d "build" ]; then
  mkdir build
fi

cd build && 
../configure && 
make && 
cp src/main ../run && 
echo Program installed. Move to run folder to start the simulation.