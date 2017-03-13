#include "include/plotter.h"

Plotter::Plotter(const std::string& filePath)
{
  folder = filePath;

  arma::mat simtime, loader;
  simtime.load(folder + "simtime.dat");

  lengthSim = simtime.size();
  maxTime = arma::max(simtime.col(0));

  loader.load(folder + "state.dat");
  pos.zeros(lengthSim, 3);
  rot.zeros(lengthSim, 3);

  theta.zeros(lengthSim, 2);
  omega.zeros(lengthSim, 2);

  for (int i = 0; i < 3; i++) {
    pos.col(i) = loader.row(i+6).t();
    rot.col(i) = loader.row(i).t();
  }

  theta.col(0) = simtime;
  theta.col(1) = rot.col(0);
  omega.col(0) = simtime;
  omega.col(1) = loader.row(3).t();

  loader.clear();

  loader.load(folder + "network.dat");
  value.zeros(lengthSim, 2);
  value.col(0) = simtime;
  value.col(1) = loader.row(0).t();

  policy.zeros(lengthSim, 2);
  policy.col(0) = simtime;
  policy.col(1) = loader.row(1).t();

  dopa.zeros(lengthSim, 2);
  dopa.col(0) = simtime;
  dopa.col(1) = loader.row(2).t();

  loader.clear();

  loader.load(folder + "environment.dat");
  reward.zeros(lengthSim, 2);
  reward.col(0) = simtime;
  reward.col(1) = loader.row(0).t();

  tdError.zeros(lengthSim, 2);
  tdError.col(0) = simtime;
  tdError.col(1) = loader.row(1).t();
}

Plotter::Plotter() {}

Plotter::~Plotter()
{

}

void Plotter::InState()
{
  double limTheta[2],
         limOmega[2];
  limTheta[0] = arma::min(theta.col(1)) - arma::min(theta.col(1))/10;
  limTheta[1] = arma::max(theta.col(1)) + arma::max(theta.col(1))/10;
  limOmega[0] = arma::min(omega.col(1)) - arma::min(omega.col(1))/10;
  limOmega[1] = arma::max(omega.col(1)) + arma::max(omega.col(1))/10;

  gp << "set terminal x11 0\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set xrange [0:%f]\n", maxTime);
  gp << cmd;
  gp << "set multiplot layout 2,1 title 'Input State'\n";

  gp << "unset xlabel\nset ylabel 'theta 1 [rad]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTheta[0], limTheta[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(theta);

  gp << "set xlabel 'time [s]'\nset ylabel 'omega 1 [rad/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limOmega[0], limOmega[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(omega);

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/inputState.png'\n", folder.c_str());
  gp << cmd;
  gp << "set multiplot layout 2,1 title 'Input State'\n";

  gp << "unset xlabel\nset ylabel 'theta 1 [rad]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTheta[0], limTheta[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(theta);

  gp << "set xlabel 'time [s]'\nset ylabel 'omega 1 [rad/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limOmega[0], limOmega[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(omega);

  gp << "unset multiplot\n";
  gp << "reset\n";
}

void Plotter::RobotPos()
{
  double limX[2],
         limY[2],
         limZ;
  limX[0] = arma::min(pos.col(0)) - arma::min(pos.col(0))/10;
  limX[1] = arma::max(pos.col(0)) + arma::max(pos.col(0))/10;
  limY[0] = arma::min(pos.col(1)) - arma::min(pos.col(1))/10;
  limY[1] = arma::max(pos.col(1)) + arma::max(pos.col(1))/10;
  limZ = arma::max(pos.col(2)) + arma::max(pos.col(2))/10;

  arma::mat temp;
  arma::uvec id;
  id << 0 << 2;


  gp << "set terminal x11 1\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set yrange [0:%f]\n", limZ);
  gp << cmd;
  gp << "set multiplot layout 1,2 title 'Lateral Positions'\n";

  gp << "set xlabel 'x position (m)'\nset ylabel 'z position (m)'\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limX[0], limX[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(id);
  gp.send1d(temp);
  temp.clear();

  gp << "set xlabel 'y position (m)'\nunset ylabel\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limY[0], limY[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(1,2);
  gp.send1d(temp);
  temp.clear();

  gp << "unset multiplot\n";
  dummy = asprintf(&cmd, "set output '%s/lateral_pos.png'\n", folder.c_str());
  gp << cmd;
  gp << "set multiplot layout 1,2 title 'Lateral Positions'\n";

  gp << "set xlabel 'x position (m)'\nset ylabel 'z position (m)'\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limX[0], limX[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(id);
  gp.send1d(temp);
  temp.clear();

  gp << "set xlabel 'y position (m)'\nunset ylabel\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limY[0], limY[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(1,2);
  gp.send1d(temp);
  temp.clear();

  gp << "unset multiplot\n";
  gp << "reset\n";
}

void Plotter::NetActivity()
{
  double limValue[2],
         limPolicy[2],
         limDopa[2];
  limValue[0] = arma::min(value.col(1)) - arma::min(value.col(1))/10;
  limValue[1] = arma::max(value.col(1)) + arma::max(value.col(1))/10;
  limPolicy[0] = arma::min(policy.col(1)) - arma::min(policy.col(1))/10;
  limPolicy[1] = arma::max(policy.col(1)) + arma::max(policy.col(1))/10;
  limDopa[0] = arma::min(dopa.col(1)) - arma::min(dopa.col(1))/10;
  limDopa[1] = arma::max(dopa.col(1)) + arma::max(dopa.col(1))/10;

  gp << "set terminal x11 2\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set xrange [0:%f]\n", maxTime);
  gp << cmd;
  gp << "set multiplot layout 3,1 title 'Network Activity'\n";

  gp << "unset xlabel\nset ylabel 'Value Function []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limValue[0], limValue[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(value);

  gp << "set xlabel 'time [s]'\nset ylabel 'Policy []'\n";
  dummy = asprintf(&cmd, "set yrange [%.10f:%.10f]\n", limPolicy[0], limPolicy[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(policy);

  gp << "set xlabel 'time [s]'\nset ylabel 'Dopa Activity []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limDopa[0], limDopa[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(dopa);

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/netActivity.png'\n", folder.c_str());
  gp << cmd;
  gp << "set multiplot layout 3,1 title 'Network Activity'\n";

  gp << "unset xlabel\nset ylabel 'Value Function []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limValue[0], limValue[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(value);

  gp << "set xlabel 'time [s]'\nset ylabel 'Policy []'\n";
  dummy = asprintf(&cmd, "set yrange [%.10f:%.10f]\n", limPolicy[0], limPolicy[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(policy);

  gp << "set xlabel 'time [s]'\nset ylabel 'Dopa Activity []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limDopa[0], limDopa[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(dopa);

  gp << "unset multiplot\n";
  gp << "reset\n";
}

void Plotter::EnvActivity()
{
  double limRew[2],
         limTDerr[2];
  limRew[0] = arma::min(reward.col(1)) - arma::min(reward.col(1))/10;
  limRew[1] = arma::max(reward.col(1)) + arma::max(reward.col(1))/10;
  limTDerr[0] = arma::min(tdError.col(1)) - arma::min(tdError.col(1))/10;
  limTDerr[1] = arma::max(tdError.col(1)) + arma::max(tdError.col(1))/10;

  gp << "set terminal x11 3\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set xrange [0:%f]\n", maxTime);
  gp << cmd;
  gp << "set multiplot layout 2,1 title 'Environment Activity'\n";

  gp << "unset xlabel\nset ylabel 'Reward [units]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limRew[0], limRew[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(reward);

  gp << "set xlabel 'time [s]'\nset ylabel 'TD-error [units/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTDerr[0], limTDerr[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tdError);

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/envActivity.png'\n", folder.c_str());
  gp << cmd;
  gp << "set multiplot layout 2,1 title 'Input State'\n";

  gp << "unset xlabel\nset ylabel 'Reward [units]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limRew[0], limRew[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(reward);

  gp << "set xlabel 'time [s]'\nset ylabel 'TD-error [units/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTDerr[0], limTDerr[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tdError);

  gp << "unset multiplot\n";
  gp << "reset\n";
}

void Plotter::ValueMat(double thetaBound, double omegaBound)
{
  double vRes = 0.1,
         vRows= 2*thetaBound/vRes + 1,
         vCols= 2*omegaBound/vRes + 1,
         limValue[2];

  limValue[0] = arma::min(value.col(1)) - arma::min(value.col(1))/10;
  limValue[1] = arma::max(value.col(1)) + arma::max(value.col(1))/10;

  valueMat.zeros(vRows+1,vCols+1);

  valueMat(0,0) = vCols+1;
  valueMat.col(0).rows(1,vRows) = arma::regspace<arma::vec>(-thetaBound, vRes, thetaBound);
  valueMat.row(0).cols(1,vCols) = arma::regspace<arma::rowvec>(-omegaBound, vRes, omegaBound);
  for (int i = 0; i < vRows; ++i)
      valueMat.col(0).row(i) = round(valueMat.col(0).row(i)/vRes)*vRes;
  for (int i = 0; i < vCols; ++i)
      valueMat.row(0).col(i) = round(valueMat.row(0).col(i)/vRes)*vRes;

  gp << "set terminal x11 4\n";
  gp << "set title 'State-Value Function'\n";
  gp << "set pm3d; set palette\n";
  gp << "set hidden3d\n";
  gp << "set dgrid3d 50,50 qnorm 2\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set xlabel 'omega 1 [rad]'\nset ylabel 'theta 1 [rad/s]'\nset zlabel 'VF [reward units]'\n";
  // dummy = asprintf(&cmd, "set xrange [%f:%f]\nset yrange [%f:%f]\nset zrange [%f:%f]\n",
  //                  -omegaBound - omegaBound/10, omegaBound + omegaBound/10,
  //                  -thetaBound - thetaBound/10, thetaBound + thetaBound/10,
  //                  limValue[0], limValue[1]);
  // gp << cmd;

  for (int k = 0; k < lengthSim; k++) {
    for (int i = 0; i < valueMat.n_rows; ++i)
    {
        if (valueMat(i,0) == round(theta(k,1)/vRes)*vRes)
        {
            for (int j = 0; j < valueMat.n_cols; ++j)
            {
                if (valueMat(0,j) == round(omega(k,1)/vRes)*vRes)
                    valueMat(i, j) = value(k,1);
            }
        }
    }
  }
  gp << "splot " << gp.file1d(valueMat) << "matrix nonuniform with lines notitle\n";

  gp << "set terminal png\n"; // size 350,262 enhanced font 'Verdana,10'
  dummy = asprintf(&cmd, "set output '%s/valueMatrix.png'\n", folder.c_str());
  gp << cmd;
  gp << "splot " << gp.file1d(valueMat) << "matrix nonuniform with lines notitle\n";
  gp << "reset\n";

  valueMat.save(folder + "valueMatrix.dat",arma::raw_ascii);
}
