#include "include/plotter.h"

Plotter::Plotter(const std::string& filePath)
{
  folder = filePath;

  arma::mat simtime, loader;
  simtime.load(folder + "simtime.dat");

  lengthSim = simtime.size();
  maxTime = arma::max(simtime.col(0));
  limTime[0] = 0;
  limTime[1] = maxTime;

  loader.load(folder + "state.dat");
  pos.zeros(lengthSim, 3);
  rot.zeros(lengthSim, 3);

  theta.zeros(lengthSim, 2);
  omega.zeros(lengthSim, 2);

  for (int i = 0; i < 3; i++) {
    pos.col(i) = loader.row(i+6).t();
    rot.col(i) = loader.row(i).t();
  }

  limX[0] = arma::min(pos.col(0)) - arma::min(pos.col(0))*0.1;
  limX[1] = arma::max(pos.col(0)) + arma::max(pos.col(0))*0.1;
  limY[0] = arma::min(pos.col(1)) - arma::min(pos.col(1))*0.1;
  limY[1] = arma::max(pos.col(1)) + arma::max(pos.col(1))*0.1;
  limZ[0] = 0;
  limZ[1] = arma::max(pos.col(2)) + arma::max(pos.col(2))*0.1;

  theta.col(0) = simtime;
  theta.col(1) = rot.col(0);
  omega.col(0) = simtime;
  omega.col(1) = loader.row(3).t();

  limTheta[0] = arma::min(theta.col(1)) - arma::min(theta.col(1))*0.1;
  limTheta[1] = arma::max(theta.col(1)) + arma::max(theta.col(1))*0.1;
  limOmega[0] = arma::min(omega.col(1)) - arma::min(omega.col(1))*0.1;
  limOmega[1] = arma::max(omega.col(1)) + arma::max(omega.col(1))*0.1;

  loader.clear();

  loader.load(folder + "control.dat");
  lift.zeros(lengthSim, 2);
  tau1.zeros(lengthSim, 2);
  tau2.zeros(lengthSim, 2);

  lift.col(0) = simtime;
  lift.col(1) = loader.row(0).t();
  tau1.col(0) = simtime;
  tau1.col(1) = loader.row(1).t();
  tau2.col(0) = simtime;
  tau2.col(1) = loader.row(2).t();

  limLift[0] = arma::min(lift.col(1)) - arma::min(lift.col(1))*0.1;
  limLift[1] = arma::max(lift.col(1)) + arma::max(lift.col(1))*0.1;
  limTau1[0] = arma::min(tau1.col(1)) - arma::min(tau1.col(1))*0.1;
  limTau1[1] = arma::max(tau1.col(1)) + arma::max(tau1.col(1))*0.1;
  limTau2[0] = arma::min(tau2.col(1)) - arma::min(tau2.col(1))*0.1;
  limTau2[1] = arma::max(tau2.col(1)) + arma::max(tau2.col(1))*0.1;

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

  limValue[0] = arma::min(value.col(1)) - arma::min(value.col(1))*0.1;
  limValue[1] = arma::max(value.col(1)) + arma::max(value.col(1))*0.1;
  limPolicy[0] = arma::min(policy.col(1)) - arma::min(policy.col(1))*0.1;
  limPolicy[1] = arma::max(policy.col(1)) + arma::max(policy.col(1))*0.1;
  limDopa[0] = arma::min(dopa.col(1)) - arma::min(dopa.col(1))*0.1;
  limDopa[1] = arma::max(dopa.col(1)) + arma::max(dopa.col(1))*0.1;

  loader.clear();

  loader.load(folder + "environment.dat");
  reward.zeros(lengthSim, 2);
  reward.col(0) = simtime;
  reward.col(1) = loader.row(0).t();

  tdError.zeros(lengthSim, 2);
  tdError.col(0) = simtime;
  tdError.col(1) = loader.row(1).t();

  limRew[0] = arma::min(reward.col(1)) - arma::min(reward.col(1))*0.1;
  limRew[1] = arma::max(reward.col(1)) + arma::max(reward.col(1))*0.1;
  limTDerr[0] = arma::min(tdError.col(1)) - arma::min(tdError.col(1))*0.1;
  limTDerr[1] = arma::max(tdError.col(1)) + arma::max(tdError.col(1))*0.1;
}

Plotter::Plotter() {}

Plotter::~Plotter()
{

}

void Plotter::InState()
{
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

void Plotter::Control()
{
  gp << "set terminal x11 5\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set xrange [0:%f]\n", maxTime);
  gp << cmd;
  gp << "set multiplot layout 3,1 title 'Control Forces'\n";

  gp << "unset xlabel\nset ylabel 'Lift Force []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limLift[0], limLift[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(lift);

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau1 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau1[0], limTau1[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau1);

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau2 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau2[0], limTau2[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau2);

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/netActivity.png'\n", folder.c_str());
  gp << cmd;
  gp << "set multiplot layout 3,1 title 'Network Activity'\n";

  gp << "unset xlabel\nset ylabel 'Lift Force []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limLift[0], limLift[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(lift);

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau1 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau1[0], limTau1[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau1);

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau2 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau2[0], limTau2[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau2);

  gp << "unset multiplot\n";
  gp << "reset\n";
}

void Plotter::RobotPos()
{
  arma::mat temp;
  arma::uvec id;
  id << 0 << 2;

  gp << "set terminal x11 1\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limZ[0], limZ[1]);
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

  gp << "set terminal png\n";
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
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limPolicy[0], limPolicy[1]);
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
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limPolicy[0], limPolicy[1]);
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
         vCols= 2*omegaBound/vRes + 1;

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

void Plotter::Draw(arma::mat& data,
                   int term,
                   int width, int length,
                   int xpos, int ypos,
                   double *xlim, double *ylim,
                   const std::string& title,
                   const std::string& xlabel,
                   const std::string& ylabel)
{
  dummy = asprintf(&cmd, "set terminal x11 %d size %d,%d position %d,%d\n", term, width, length, xpos, ypos);
  gp << cmd;
  dummy = asprintf(&cmd, "set title '%s'\n", title.c_str());
  gp << cmd;
  dummy = asprintf(&cmd, "set xlabel '%s'\n", xlabel.c_str());
  gp << cmd;
  dummy = asprintf(&cmd, "set ylabel '%s'\n", ylabel.c_str());
  gp << cmd;
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  dummy = asprintf(&cmd, "set xrange [%.15f:%.15f]\n", xlim[0], xlim[1]);
  gp << cmd;
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", ylim[0], ylim[1]);
  gp << cmd;
  gp << "plot" << gp.file1d(data) << "with lines notitle\n";
  gp << "reset\n";
}

void Plotter::Simulation() {
  Display* Frame = new Display("Hello World!", 800, 600);
  Camera *Cam = new Camera(glm::vec3(-0.3,0.2,0.2), glm::vec3(0,0,0), glm::vec3(0,1,0));
  Light *Lamp = new Light(glm::vec3(2,1,5));
  Shader *myShader = new Shader("../graphic/vertex.glsl", "../graphic/fragment.glsl");
  Model *Base = new Model("../graphic/","base");
  Model *Robot = new Model("../graphic/","robobee");

  for (int i = 0; i < lengthSim; i+=10) {
    Frame->Clear(0.0f, 0.1f, 0.15f, 1.0f);
    myShader->Bind();
    Robot->SetPos( glm::vec3( pos(i,1), pos(i,2), pos(i,0) ) );
    Robot->SetRot( glm::vec3( rot(i,1), rot(i,2), rot(i,0) ) );
    myShader->Update(*Robot, *Cam, *Lamp);
    Robot->Draw();
    myShader->Update(*Base, *Cam, *Lamp);
    Base->Draw();
    Frame->Update();

    toPlot = theta.rows(0,i);
    Draw(toPlot, 1, 300, 200, 0, 0, limTime, limTheta, "Angular Position", "time [s]", "theta [rad]");
    toPlot = omega.rows(0,i);
    Draw(toPlot, 2, 300, 200, 310, 0, limTime, limOmega, "Angular Velocity", "time [s]", "omega [rad/s]");

    toPlot = value.rows(0,i);
    Draw(toPlot, 3, 300, 200, 0, 250, limTime, limValue, "Value Function", "time [s]", "Value [units]");
    toPlot = policy.rows(0,i);
    Draw(toPlot, 4, 300, 200, 310, 250, limTime, limPolicy, "Policy", "time [s]", "Policy [Nm]");

    toPlot = tdError.rows(0,i);
    Draw(toPlot, 5, 300, 200, 0, 500, limTime, limTDerr, "TD-error", "time [s]", "TD-error [units/s]");
    toPlot = dopa.rows(0,i);
    Draw(toPlot, 6, 300, 200, 310, 500, limTime, limDopa, "Dopa Activity", "time [s]", "Dopa [Hz]");
    toPlot = reward.rows(0,i);
    Draw(toPlot, 7, 300, 200, 620, 500, limTime, limRew, "Reward", "time [s]", "Reward [units]");
  }
}
