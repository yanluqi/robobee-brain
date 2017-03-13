#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>
#include <armadillo>
#include <boost/tuple/tuple.hpp>
#include "gnuplot-iostream.h"

class Plotter
{
public:
  Plotter(const std::string& filePath);
  Plotter();
  virtual ~Plotter();

  void InState();
  void RobotPos();
  void NetActivity();
  void EnvActivity();
  void ValueMat(double thetaBound, double omegaBound);

protected:

private:
  Gnuplot gp;
  std::string folder;
  char *cmd;
  arma::mat pos, rot,
            theta, omega,
            reward, tdError,
            value, policy, dopa,
            valueMat;
  int lengthSim, dummy;
  double maxTime;
};

#endif // PLOTTER_H
