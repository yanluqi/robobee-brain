#ifndef PLOTTER_H
#define PLOTTER_H

#include <string>
#include <algorithm>
#include <armadillo>
#include <dynplot.hh>
#include <boost/tuple/tuple.hpp>
#include "gnuplot-iostream.h"

class Plotter
{
public:
  Plotter(const std::string& filePath, bool LOAD);
  Plotter();
  virtual ~Plotter();

  void InState();
  void Control();
  void RobotPos();
  void NetActivity();
  void EnvActivity();
  void ValueMat();
  void Weights();
  void Draw(arma::mat& data,
            int term,
            int width, int length,
            int xpos, int ypos,
            double *xlim, double *ylim,
            const std::string& title,
            const std::string& xlabel,
            const std::string& ylabel);
  void Simulation(double start, double end);

protected:
  void BuilValueMat();
  void XYZSurfReshape(arma::mat& A);

private:
  Gnuplot gp;
  std::string folder;

  char *cmd;

  arma::mat toPlot,
            pos, rot,
            theta, omega,
            lift, tau1, tau2,
            reward, tdError,
            value, policy, dopa,
            connToCritic, connToActor,
            valueMat, valueReshaped;

  int lengthSim, dummy;

  double maxTime, limTime[2], freq,
         limTheta[2], limOmega[2],
         limLift[2], limTau1[2], limTau2[2],
         limX[2], limY[2], limZ[2],
         limValue[2], limPolicy[2], limDopa[2],
         limRew[2], limTDerr[2];
};

#endif // PLOTTER_H
