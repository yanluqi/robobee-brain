#include "include/plotter.h"

Plotter::Plotter(const std::string& filePath, bool LOAD) : pi(3.1415926535897)
{
  // Load Simulation Data
  std::string picFolder(filePath + "pictures/");
  boost::filesystem::path dir(picFolder);
  if(boost::filesystem::create_directory(dir)) {
      std::cout << "Success" << "\n";
  }

  folder = filePath;
  saveFolder = picFolder;
  arma::mat simtime, loader;

  // Simulation Time
  simtime.load(folder + "simtime.dat");
  lengthSim = simtime.size();
  maxTime = arma::max(simtime.col(0));
  freq = lengthSim/maxTime;
  limTime[0] = 0;
  limTime[1] = maxTime;

  // Robot Motion View
  loader.load(folder + "state.dat");
  pos.zeros(lengthSim, 3);
  rot.zeros(lengthSim, 3);
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

  // Network Input States
  theta.zeros(lengthSim, 2);
  omega.zeros(lengthSim, 2);
  theta.col(0) = simtime;
  theta.col(1) = rot.col(0);
  omega.col(0) = simtime;
  omega.col(1) = loader.row(3).t();
  limTheta[0] = arma::min(theta.col(1)) - arma::min(theta.col(1))*0.1;
  limTheta[1] = arma::max(theta.col(1)) + arma::max(theta.col(1))*0.1;
  limOmega[0] = arma::min(omega.col(1)) - arma::min(omega.col(1))*0.1;
  limOmega[1] = arma::max(omega.col(1)) + arma::max(omega.col(1))*0.1;
  loader.clear();

  // Classical Controller Forces
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

  // Network Populations Activity
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
  limPolicy[0] = arma::min(policy.col(1));
  limPolicy[1] = arma::max(policy.col(1));
  limDopa[0] = arma::min(dopa.col(1)) - arma::min(dopa.col(1))*0.1;
  limDopa[1] = arma::max(dopa.col(1)) + arma::max(dopa.col(1))*0.1;
  loader.clear();

  // Environment Reward and TD-error calculation
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
  loader.clear();

  // Netowrk Weights
  connToCritic.load(folder + "network/connToCritic.dat");
  connToActor.load(folder + "network/connToActor.dat");

  // Value Function Surf
  if (LOAD)
    valueMat.load(folder + "valueMatrix.dat");

  BuilValueMat();
}

Plotter::Plotter(): pi(3.1415926535897) {}

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
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'omega 1 [rad/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limOmega[0], limOmega[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(omega);
  gp.flush();

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/inputState.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "set multiplot layout 2,1 title 'Input State'\n";

  gp << "unset xlabel\nset ylabel 'theta 1 [rad]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTheta[0], limTheta[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(theta);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'omega 1 [rad/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limOmega[0], limOmega[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(omega);
  gp.flush();

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
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau1 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau1[0], limTau1[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau1);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau2 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau2[0], limTau2[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau2);
  gp.flush();

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/netActivity.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "set multiplot layout 3,1 title 'Network Activity'\n";

  gp << "unset xlabel\nset ylabel 'Lift Force []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limLift[0], limLift[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(lift);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau1 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau1[0], limTau1[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau1);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Tau2 Torques []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limTau2[0], limTau2[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tau2);
  gp.flush();

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
  gp.flush();
  temp.clear();

  gp << "set xlabel 'y position (m)'\nunset ylabel\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limY[0], limY[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(1,2);
  gp.send1d(temp);
  gp.flush();
  temp.clear();

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/lateral_pos.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "set multiplot layout 1,2 title 'Lateral Positions'\n";

  gp << "set xlabel 'x position (m)'\nset ylabel 'z position (m)'\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limX[0], limX[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(id);
  gp.send1d(temp);
  gp.flush();
  temp.clear();

  gp << "set xlabel 'y position (m)'\nunset ylabel\n";
  dummy = asprintf(&cmd, "set xrange [%f:%f]\n", limY[0], limY[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  temp = pos.cols(1,2);
  gp.send1d(temp);
  gp.flush();
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
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Policy []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limPolicy[0], limPolicy[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(policy);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Dopa Activity []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limDopa[0], limDopa[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(dopa);
  gp.flush();

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/netActivity.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "set multiplot layout 3,1 title 'Network Activity'\n";

  gp << "unset xlabel\nset ylabel 'Value Function []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limValue[0], limValue[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(value);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Policy []'\n";
  dummy = asprintf(&cmd, "set yrange [%.15f:%.15f]\n", limPolicy[0], limPolicy[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(policy);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'Dopa Activity []'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limDopa[0], limDopa[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(dopa);
  gp.flush();

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
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'TD-error [units/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTDerr[0], limTDerr[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tdError);
  gp.flush();

  gp << "unset multiplot\n";

  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/envActivity.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "set multiplot layout 2,1 title 'Input State'\n";

  gp << "unset xlabel\nset ylabel 'Reward [units]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limRew[0], limRew[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(reward);
  gp.flush();

  gp << "set xlabel 'time [s]'\nset ylabel 'TD-error [units/s]'\n";
  dummy = asprintf(&cmd, "set yrange [%f:%f]\n", limTDerr[0], limTDerr[1]);
  gp << cmd;
  gp << "plot '-' with lines notitle\n";
  gp.send1d(tdError);
  gp.flush();

  gp << "unset multiplot\n";
  gp << "reset\n";
}

void Plotter::ValueMat()
{
  gp << "set terminal x11 4\n";
  gp << "set title 'State-Value Function'\n";
  gp << "set pm3d;\n";
  gp << "set palette\n";
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
  gp << "splot " << gp.file1d(valueMat) << "matrix nonuniform with lines notitle\n";

  gp << "set terminal png\n"; // size 350,262 enhanced font 'Verdana,10'
  dummy = asprintf(&cmd, "set output '%s/valueMatrix.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "splot " << gp.file1d(valueMat) << "matrix nonuniform with lines notitle\n";
  gp.flush();
  gp << "reset\n";

  // set pm3d
  // set hidden3d
  // splot "trajectory.dat" using 4:6:10 title "trajectory"  with lines lt -1,\
  //       "surface.dat" using 1:3:7 title "surface"
}

void Plotter::Weights()
{
  gp << "set pm3d;\n";
  gp << "set palette\n";
  gp << "set hidden3d\n";
  gp << "set dgrid3d 50,50 qnorm 2\n";
  gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
  gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";

  gp << "set terminal x11 7\n";
  gp << "set title 'Actor Weights Start'\n";
  gp << "set xlabel 'Place Cells ID'\nset ylabel 'Actor ID'\nset zlabel 'Weight'\n";
  gp << "splot " << gp.file1d(connToActor) << "using 1:2:3 with lines notitle\n";
  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/weightsActorStart.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "splot " << gp.file1d(connToActor) << "using 1:2:3 with lines notitle\n";

  gp << "set terminal x11 8\n";
  gp << "set title 'Actor Weights End'\n";
  gp << "set xlabel 'Place Cells ID'\nset ylabel 'Actor ID'\nset zlabel 'Weight'\n";
  gp << "splot " << gp.file1d(connToActor) << "using 1:2:4 with lines notitle\n";
  gp << "set terminal png\n";
  dummy = asprintf(&cmd, "set output '%s/weightsActorEnd.png'\n", saveFolder.c_str());
  gp << cmd;
  gp << "splot " << gp.file1d(connToActor) << "using 1:2:4 with lines notitle\n";

  gp.flush();
  gp << "reset\n";
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
  gp.flush();
  gp << "reset\n";
}

void Plotter::Results(double start, double end) {
  int startIndex = start*freq,
      endIndex = end*freq;
  double timeline [] = {start, end};

  toPlot = theta.rows(startIndex,endIndex);
  Draw(toPlot, THETA, 480, 300, 0, 0, timeline, limTheta, "Angular Position", "time [s]", "theta [rad]");
  toPlot = omega.rows(startIndex,endIndex);
  Draw(toPlot, OMEGA, 480, 300, 310, 0, timeline, limOmega, "Angular Velocity", "time [s]", "omega [rad/s]");

  toPlot = value.rows(startIndex,endIndex);
  Draw(toPlot, VALUE, 480, 300, 0, 250, timeline, limValue, "Value Function", "time [s]", "Value [units]");
  toPlot = policy.rows(startIndex,endIndex);
  Draw(toPlot, POLICY, 480, 300, 310, 250, timeline, limPolicy, "Policy", "time [s]", "Policy [Nm]");

  toPlot = tdError.rows(startIndex,endIndex);
  Draw(toPlot, TDERROR, 480, 300, 0, 500, timeline, limTDerr, "TD-error", "time [s]", "TD-error [units/s]");
  toPlot = dopa.rows(startIndex,endIndex);
  Draw(toPlot, DOPA, 480, 300, 310, 500, timeline, limDopa, "Dopa Activity", "time [s]", "Dopa [Hz]");
  toPlot = reward.rows(startIndex,endIndex);
  Draw(toPlot, REWARD, 480, 300, 620, 500, timeline, limRew, "Reward", "time [s]", "Reward [units]");
}

void Plotter::Simulation(double start, double end) {
  Display* Frame = new Display("Hello World!", 800, 600);
  Camera *Cam = new Camera(glm::vec3(-0.3,0.2,0.2), glm::vec3(0,0,0), glm::vec3(0,1,0));
  Light *Lamp = new Light(glm::vec3(2,1,5));
  Shader *myShader = new Shader("../graphic/vertex.glsl", "../graphic/fragment.glsl");
  Model *Base = new Model("../graphic/","base");
  Model *Robot = new Model("../graphic/","robobee");
  Frame->SetRecorder(saveFolder + "flight.mp4");

  int startIndex = start*freq,
      endIndex = end*freq;
  double timeline [] = {start, end};

  for (int i = startIndex; i < endIndex; i+=10) {
    Frame->Clear(0.0f, 0.1f, 0.15f, 1.0f);
    myShader->Bind();
    Robot->SetPos( glm::vec3( pos(i,1), pos(i,2), pos(i,0) ) );
    Robot->SetRot( glm::vec3( rot(i,1), rot(i,2), rot(i,0) ) );
    myShader->Update(*Robot, *Cam, *Lamp);
    Robot->Draw();
    myShader->Update(*Base, *Cam, *Lamp);
    Base->Draw();
    Frame->Update();

    toPlot = theta.rows(startIndex,i);
    Draw(toPlot, 1, 300, 200, 0, 0, timeline, limTheta, "Angular Position", "time [s]", "theta [rad]");
    toPlot = omega.rows(startIndex,i);
    Draw(toPlot, 2, 300, 200, 310, 0, timeline, limOmega, "Angular Velocity", "time [s]", "omega [rad/s]");

    toPlot = value.rows(startIndex,i);
    Draw(toPlot, 3, 300, 200, 0, 250, timeline, limValue, "Value Function", "time [s]", "Value [units]");
    toPlot = policy.rows(startIndex,i);
    Draw(toPlot, 4, 300, 200, 310, 250, timeline, limPolicy, "Policy", "time [s]", "Policy [Nm]");

    toPlot = tdError.rows(startIndex,i);
    Draw(toPlot, 5, 300, 200, 0, 500, timeline, limTDerr, "TD-error", "time [s]", "TD-error [units/s]");
    toPlot = dopa.rows(startIndex,i);
    Draw(toPlot, 6, 300, 200, 310, 500, timeline, limDopa, "Dopa Activity", "time [s]", "Dopa [Hz]");
    toPlot = reward.rows(startIndex,i);
    Draw(toPlot, 7, 300, 200, 620, 500, timeline, limRew, "Reward", "time [s]", "Reward [units]");
  }
  delete Frame;
  delete Cam;
  delete Lamp;
  delete myShader;
  delete Base;
  delete Robot;
}

void Plotter::BuilValueMat()
{
  double vRes = 0.1,
         wrappedAngle = 0;

  if (valueMat.is_empty()) {
    const double pi = 3.1415926535897;
    double omegaBound = 10,
           thetaBound = 2*pi - vRes,
           vRows= thetaBound/vRes + 1,
           vCols= 2*omegaBound/vRes + 1;

    valueMat.zeros(vRows+1,vCols+1);

    valueMat(0,0) = vCols+1;
    valueMat.col(0).rows(1,vRows) = arma::regspace<arma::vec>(0, vRes, thetaBound);
    valueMat.row(0).cols(1,vCols) = arma::regspace<arma::rowvec>(-omegaBound, vRes, omegaBound);
    for (int i = 0; i < vRows; ++i)
        valueMat.col(0).row(i) = round(valueMat.col(0).row(i)/vRes)*vRes;
    for (int i = 0; i < vCols; ++i)
        valueMat.row(0).col(i) = round(valueMat.row(0).col(i)/vRes)*vRes;
  }

  for (int k = 0; k < lengthSim; k++) {
    wrappedAngle = WrapTo2Pi(theta(k,1));
    for (int i = 0; i < valueMat.n_rows; ++i) {
      if (valueMat(i,0) == round(wrappedAngle/vRes)*vRes) {
        for (int j = 0; j < valueMat.n_cols; ++j) {
          if (valueMat(0,j) == round(omega(k,1)/vRes)*vRes)
                    valueMat(i, j) = value(k,1);
        }
      }
    }
  }

  explorePath.zeros(lengthSim,3);
  explorePath.col(0) = theta.col(1);
  explorePath.col(1) = omega.col(1);
  explorePath.col(2) = value.col(1);

  for (int i = 0; i < lengthSim; i++)
    explorePath(i,0) = WrapTo2Pi(explorePath(i,0));

  explorePath.save(folder + "explorePath.dat", arma::raw_ascii);
  valueMat.save(folder + "valueMatrix.dat",arma::raw_ascii);
  XYZSurfReshape(valueMat);
  valueReshaped.save(folder + "valueReshaped.dat",arma::raw_ascii);
}

void Plotter::XYZSurfReshape(arma::mat& A)
{
  arma::mat z = A.submat(1,1,A.n_rows-1,A.n_cols-1),
            x(arma::size(z), arma::fill::zeros),
            y(arma::size(z), arma::fill::zeros);

  valueReshaped.zeros(z.n_elem, 3);


  for (int i = 0; i < x.n_cols; i++)
    x.col(i) = A.submat(1,0,A.n_rows-1,0);

 	for (int i = 0; i < y.n_rows; i++)
    y.row(i) = A.submat(0,1,0,A.n_cols-1);

  valueReshaped.col(0) = arma::vectorise(x);
  valueReshaped.col(1) = arma::vectorise(y);
  valueReshaped.col(2) = arma::vectorise(z);
}
