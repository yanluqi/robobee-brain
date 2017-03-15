#include <iostream>
#include <string>
#include "environment.hh"

int main(int argc, char const *argv[]) {
  std::string folder;
  std::cout << "Insert folder name: ";
  std::cin >> folder;
  folder = "Simulations/" + folder + "/";

  double pi = 3.1415926535897,
         omegaBound = 6*pi+pi/2,
         thetaBound = pi;

  Plotter Plot(folder);
  Plot.Simulation();
  return 0;
}
