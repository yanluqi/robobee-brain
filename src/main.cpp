/*
 *  main.cpp
 *
 *  This file is part of RoboBrain.
 *  Copyright (C) 2016 Bernardo Fichera
 *
 *  RoboBrain is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RoboBrain is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RoboBrain.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Standard Libraries
#include <algorithm>
#include <ctime>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <string.h>

// My Libraries
#include <dynplot.hh>
#include "environment.hh"

// MPI Simulation
#include <mpi.h>
#include <music.hh>


#define IN_LATENCY (0.01)
#define TICK (0.01)

MPI::Intracomm comm;

int main(int argc, char **argv)
{
/*======================================================SETUP PHASE=======================================================*/

/*==================
|   ROBOBEE/ENV    |
==================*/

    const double pi = 3.1415926535897;

    arma::vec q,
              q0,
              q_desired(12, arma::fill::zeros),
              u(4, arma::fill::zeros);

    q0 = { 0.2, -0.2,    0,	 	// Angular Position (Body Attached)
             0,    0,    0, 	// Angular Velocity (Body Attached) -1,0,1
          0.04, 0.04, 0.01, 	// Linear Postion (Inertial Frame)
           0.1, -0.3,    0};	// Linear Velocity (Body Attached)

    q_desired(8) = 0.08;

    q = q0;

    double dynFreq = 1000, dynStep = 1/dynFreq,
           maxRew = 50, sigma = 3.0,
           reward = maxRew/2*cos(q(0)) +
                    maxRew*std::exp(-std::pow(q(3),2)/(2*std::pow(sigma,2))) - maxRew/2;


    // Objects Creation
    Robobee bee(q, dynFreq);     // ROBOBEE
    Controller ctr(q_desired);    // Controller

/*===========================
|   MUSIC Agent Connection  |
===========================*/
    // Create setup object ->  start setup phase
    MUSIC::Setup* setup = new MUSIC::Setup (argc, argv);

    // Read simulation time from configuration music file
    double simt;
    setup->config ("simtime", &simt);

    // Create Input and Output port
    MUSIC::EventInputPort *indata = setup->publishEventInput("p_in");
    MUSIC::EventOutputPort *outdata = setup->publishEventOutput("p_out");

    // Get number of processes and rank processor
    comm = setup->communicator();
    int nProcs = comm.Get_size();
    int rank = comm.Get_rank();

    // Figure out how many input/output channels we have
    int width[2], firstId[2], nLocal[2], rest;
    width[0] = outdata->width();
    width[1] = indata->width();

    // Divide channels evenly over the number of processes
    for (int i = 0; i < 2; ++i)
    {
        nLocal[i] = width[i] / nProcs;
        rest = width[i] % nProcs;
        firstId[i] = nLocal[i] * rank;
        if (rank < rest) {
            firstId[i] += rank;
            nLocal[i] += 1;
        } else
        firstId[i] += rest;
    }

    int idx[nLocal[0]];
    for (int i = 0; i < nLocal[0]; ++i)
        idx[i] = firstId[0] + i;

    // Create an index based on the rank channel asignment above
    MUSIC::LinearIndex outindex(firstId[0], nLocal[0]);
    MUSIC::LinearIndex inindex(firstId[1], nLocal[1]);

    /* NETWORK OUTPUT */
    int pops_size [] = {50, 60, 100}; // [critic_size, actor_size, dopa_size]

    double value_param[] = {1.5, -100.0, 1.0},       // [A_critic, b_critic, tau_r]
           policy_param[] = {2e-6, -2e-6},          // [F_max, F_min]
           dopa_param[] = {1, 0},                   // [A_dopa, b_dopa]
           *value,
           valueFunction = value_param[1],
           tdError = 0,
           policy = 0,
           dopaActivity = 0;

    // Generate an instance of Receiver
    Receiver *inhandler = new Receiver(pops_size, sizeof(pops_size)/sizeof(pops_size[0]));

    inhandler->SetCritic(0, value_param);
    inhandler->SetActor(1, policy_param);
    inhandler->SetDopa(2, dopa_param);

    /* NETWORK INPUT */
    double max_psg = 1000,            // Max rate Poisson process for place cells
           ranges[] = {2*pi, -10}; // Range per state (negative means symmetric with respect to the origin)

    int idState[] = {0,3},           // Define array with States' ID you want to use
        resState[] = {7,15};          // Number of place cells per state

    bool types[] = {true, false};    // true->angle false->anyother

    Sender *outhandler = new Sender(outdata, TICK);
    outhandler->CreatePlaceCells(2, idState, resState, types, ranges, max_psg);

    // Mapping Input/Output Port
    outdata->map(&outindex, MUSIC::Index::GLOBAL);
    indata->map(&inindex, inhandler, IN_LATENCY, 1);

/*==================
|   OPENGL         |
==================*/
    bool ANIMATE = false, REC = false;
    int length = 800, height = 600;
	  double frameRate = 0.01;

	  Display* Frame = new Display("Hello World!", length, height);
	  Camera *Cam = new Camera(glm::vec3(-0.3,0.2,0.2), glm::vec3(0,0,0), glm::vec3(0,1,0));
	  Light *Lamp = new Light(glm::vec3(2,1,5));
	  Shader *myShader = new Shader("../graphic/vertex.glsl", "../graphic/fragment.glsl");
	  Model *Base = new Model("../graphic/","base");
	  Model *Robot = new Model("../graphic/","robobee");

    if (!ANIMATE){
      delete Frame;
      delete Cam;
      delete Lamp;
      delete myShader;
      delete Base;
      delete Robot;
    }

/*==================
|   RECORDING      |
==================*/
    // Create Saving Folder
    time_t timer;
   	time (&timer);
    struct tm *timeInfo = localtime(&timer);

    char *fname;
    int dummy = asprintf(&fname, "Simulations/simtime_%d-%d-%d/", timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);

    std::string folder(fname), netFolder(folder + "network/");
    boost::filesystem::path dir(folder), dirNet(netFolder);
    if(boost::filesystem::create_directory(dir) && boost::filesystem::create_directory(dirNet)) {
        std::cout << "Success" << "\n";
    }

    double lenghtVecs = dynFreq*simt + 1;
    arma::vec timeSim(lenghtVecs, 1);

    // Agent
    enum{VALUEFUN,POLICY,DOPA};
    arma::mat network(3, lenghtVecs);

    // Environment
    enum {REWARD,TDERROR};
    arma::mat environment(2, lenghtVecs);
    arma::mat state(q.size(), lenghtVecs),
              control(u.size(),lenghtVecs);

    // OpenGL frames
    if (REC)
      Frame->SetRecorder(folder + "flight.mp4");

    Iomanager manager("BeeBrain/", folder);
    manager.SetStream("trials.dat", "out");

/*========================================================================================================================*/



/*=====================================================RUNTIME PHASE======================================================*/
    // Create runtime object -> start runtime phase (end setup phase)
    MUSIC::Runtime *runtime = new MUSIC::Runtime(setup, TICK);

    // Initialize
    int iter = 0,
        prevStep = 0,
        trials = 0;

    double tickt = runtime->time(), // Neuro-Controller/MUSIC TICK time
           dynTime = 0,             // Simulation time
           loadDopa = 1.0,          // Dopaminergic neurons loading time
           startSim = 2.0,          // Simulation start time
           punishTime = -1.0,        // Time of punish deliverying
           clockStop = 1.0,         // Resting time between trials
           trialTime = 0,           // Record duration of each trial
           prevRew = 0,
           robotPos = 0,
           thetaCheck = 0,
           omegaCheck = 0,
           cageBound = std::pow(q_desired(8),2),
           thetaBound = 2*abs(ranges[0]),
           omegaBound = abs(ranges[1]),
           controlRate = 0.55,
           cumulativeRew = 0.0,
           succTrial = 0;

    bool netControl = true;

    arma::vec prevState(12, arma::fill::zeros),
              crashState(12, arma::fill::zeros),
              falseState(12, arma::fill::zeros);

    // Simulation Loop
    manager.Print() << "Simulation start time " << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec << std::endl;
    manager.Print() << std::setw(15) << "Trial"
                    << std::setw(15) << "Start Time"
                    << std::setw(15) << "End Time"
                    << std::setw(15) << "Trial Time"
                    << std::setw(15) << "Avg Reward" << std::endl;
    while (tickt < simt) {

        // Real Time Robot Motion with 100Hz framerate
        if (ANIMATE && std::abs(remainder(tickt,frameRate)) < 0.00001){
          Frame->Clear(0.0f, 0.1f, 0.15f, 1.0f);
			    myShader->Bind();
			    Robot->SetPos( glm::vec3( q(7), q(8), q(6) ) );
			    Robot->SetRot( glm::vec3( q(1), q(2), q(0) ) );
			    myShader->Update(*Robot, *Cam, *Lamp);
			    Robot->Draw();
			    myShader->Update(*Base, *Cam, *Lamp);
			    Base->Draw();
			    Frame->Update();
        }

        // 1000Hz Classical Controller calculates 3 control torques
        u = arma::join_vert(ctr.AltitudeControl(q), ctr.DampingControl(q));

        // 100Hz Neural Controller
        if(dynTime >= TICK && std::abs(remainder(dynTime,TICK)) < 0.00001)
        {
          prevStep = tickt/dynStep;
          prevState = state.col(prevStep);
          prevRew =  environment(REWARD, prevStep);
          if (tickt > startSim)
            cumulativeRew += prevRew;

          if (dynTime >= loadDopa)
            outhandler->SendState(prevState, tickt); // falseState

          // Dopaminergic Neurons Stimulation
          if (tdError >= 400.0)
            tdError = 400.0;
          else if (tdError <= -400.0)
            tdError = -400.0;
          outhandler->SendReward(tdError, tickt); // 0

          runtime->tick();  // Music Communication: spikes are sent and received here
          tickt = runtime->time();

          policy = inhandler->GetAction(tickt);        // Policy
          dopaActivity = inhandler->GetDopa(tickt);    // Dopaminergi neurons activity
          value = inhandler->GetValue(tickt, prevRew); // Value Function and TD-error
          valueFunction = value[0];
          tdError = value[1];

          if (dynTime > punishTime + TICK && dynTime <= startSim)
            tdError = 0;
        }

        // Recording
        timeSim(iter) = dynTime;
        state.col(iter) = q;
        control.col(iter) = u;
        network(VALUEFUN, iter) = valueFunction;
        network(POLICY, iter) = policy;
        network(DOPA, iter) = dopaActivity;
        environment(REWARD, iter) = reward;
        environment(TDERROR, iter) = tdError;

        // Activate Neural Controller
        if (netControl)
          u(1) = controlRate*u(1) + policy;

        // Environment (RoboBee) generates the new state and reward
        q = bee.BeeDynamics(u);
        reward = maxRew/2*cos(q(0)) +
                 maxRew*std::exp(-std::pow(q(3),2)/(2*std::pow(sigma,2))) - maxRew/2;

        // Check Boundaries
        if (dynTime >= startSim){
            robotPos = std::pow(q(6),2)/8 + std::pow(q(7),2)/8 + std::pow(q(8)-q_desired(8),2);
            thetaCheck = std::abs(q(0));
            omegaCheck = std::abs(q(3));
        }
        // Crashing condition
        if (thetaCheck > thetaBound || omegaCheck > omegaBound || robotPos > cageBound){
          crashState = state.col(tickt/dynStep);
          trialTime = dynTime - startSim;
          if (trialTime < 1.0)
            controlRate += 0.01;
          manager.Print() << std::setw(15) << trials
                          << std::setw(15) << startSim
                          << std::setw(15) << dynTime
                          << std::setw(15) << trialTime
                          << std::setw(15) << cumulativeRew/trialTime << std::endl;
          punishTime = tickt + TICK;
          startSim = punishTime + clockStop;
          trials++;
          thetaCheck = 0;
          omegaCheck = 0;
          robotPos = 0;
          cumulativeRew = 0;
        }
        else if (dynTime - startSim >= 5.0 && netControl) {
          succTrial += 1;
          controlRate -= 0.01;
          trialTime = dynTime - startSim;
          manager.Print() << std::setw(15) << trials
                          << std::setw(15) << startSim
                          << std::setw(15) << dynTime
                          << std::setw(15) << trialTime
                          << std::setw(15) << cumulativeRew/trialTime << std::endl;
          startSim = tickt + TICK + clockStop;
          trials++;
          cumulativeRew = 0;
        }

        // Stop Simulation
        if (dynTime <= punishTime + TICK){
          q = crashState;
          reward = -50;
          bee.InitRobot(q);
          ctr.Reset();
        }
        else if (dynTime > punishTime + TICK && dynTime <= startSim) {
          q = q0;
          bee.InitRobot(q);
          ctr.Reset();
        }

        // Increment Counters
        dynTime += dynStep;
        iter++;
    }

    // End runtime phase
    runtime->finalize();

    time (&timer);
    timeInfo = localtime(&timer);
    manager.Print() << "Simulation end time " << timeInfo->tm_hour << ":" << timeInfo->tm_min << ":" << timeInfo->tm_sec << std::endl;
    manager.Print() << "Control Rate: " << controlRate << std::endl;
    manager.Print() << "Successful trials: " << succTrial << std::endl;
/*========================================================================================================================*/



/*=====================================================SAVING PHASE=======================================================*/


    state.save(folder + "state.dat",arma::raw_ascii); // arma::raw_ascii
    control.save(folder + "control.dat",arma::raw_ascii);
    timeSim.save(folder + "simtime.dat",arma::raw_ascii);
    network.save(folder + "network.dat",arma::raw_ascii);
    environment.save(folder + "environment.dat",arma::raw_ascii);

    arma::mat netParams;
    netParams.load("BeeBrain/pCellsIDs.dat");
    netParams.save(netFolder + "pCellsIDs.dat",arma::raw_ascii);
    netParams.clear();
    netParams.load("BeeBrain/criticIDs.dat");
    netParams.save(netFolder + "criticIDs.dat",arma::raw_ascii);
    netParams.clear();
    netParams.load("BeeBrain/actorIDs.dat");
    netParams.save(netFolder + "actorIDs.dat",arma::raw_ascii);
    netParams.clear();
    netParams.load("BeeBrain/connToCritic.dat");
    netParams.save(netFolder + "connToCritic.dat",arma::raw_ascii);
    netParams.clear();
    netParams.load("BeeBrain/connToActor.dat");
    netParams.save(netFolder + "connToActor.dat",arma::raw_ascii);
    netParams.clear();

/*========================================================================================================================*/



/*====================================================CLEANING PHASE======================================================*/

    delete runtime;
    delete inhandler;
    delete outhandler;

    // OpenGL
    if (ANIMATE){
      delete Frame;
      delete Cam;
      delete Lamp;
      delete myShader;
      delete Base;
      delete Robot;
    }

/*========================================================================================================================*/
}
