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

// Useful Standard Libraries
#include <algorithm>
#include <ctime>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <string.h>

// MPI Simulation
#include <mpi.h>
#include <music.hh>

// RoboBee Integration
#include <armadillo>
#include "include/robobee.h"
#include "include/controller.h"

// Transmission Data
#include "include/decoder.h"
#include "include/receiver.h"

// My OpenGL Library
#include "../graphic/include/display.h"
#include "../graphic/include/shader.h"
#include "../graphic/include/mesh.h"
#include "../graphic/include/texture.h"
#include "../graphic/include/camera.h"
#include "../graphic/include/objloader.hpp"
#include "../graphic/include/writer.h"
#include "../graphic/include/vboindexer.hpp"
#include "../graphic/include/transform.h"
#include "../graphic/include/light.h"
#include "../graphic/include/plotter.h"

// Save/load Data
#include "include/iomanager.h"

// Gnuplot Stream
#include <boost/tuple/tuple.hpp>
#include <boost/random.hpp>
#include "gnuplot-iostream.h"


#define IN_LATENCY (0.001)	
#define TICK (0.001)

// Distributions
typedef boost::uniform_real<> distType;

// Numbers Generators
typedef boost::minstd_rand genType1;    // 16%
typedef boost::mt19937 genType2;        // 93%
typedef boost::hellekalek1995 genType3; // 2%
typedef boost::mt11213b genType4;       // 100%
typedef boost::taus88 genType5;         // 100%

// Generator
typedef boost::variate_generator<genType2&, distType> numGen;

distType distribution(0,0.98);
genType2 generator(42);
numGen numberGenerator(generator, distribution);

MPI::Intracomm comm;

void send_poisson(MUSIC::EventOutputPort* outport, double rate, double tickt, int index) {

    double t = -log(numberGenerator())/rate;

    while (t<TICK) {
        outport -> insertEvent(tickt+t, MUSIC::GlobalIndex(index));
        t = t - log(numberGenerator())/rate;
    }
}

int main(int argc, char **argv)
{
/*======================================================SETUP PHASE=======================================================*/

/*==================
|   ROBOBEE/ENV    |
==================*/

    const double pi = 3.1415926535897;
    double reward, TD_error,
           CTRL_HZ = 1000;

    arma::mat q,
              u(4,1, arma::fill::zeros), 
              q_desired,
              boundary;
    
    // Define Initial & Desired State
    q  << 0.2 << arma::endr << -0.2 << arma::endr << 0 << arma::endr        // Angular Position (Body Attached)
       << 0 << arma::endr << 0 << arma::endr << 1 << arma::endr             // Angular Velocity (Body Attached)
       << 0.04 << arma::endr << 0.04 << arma::endr << 0.01 << arma::endr    // Linear Postion (Inertial Frame)
       << 0.1 << arma::endr << -0.3 << arma::endr << 0;                     // Linear Velocity (Body Attached)

    q_desired << 0 << arma::endr << 0 << arma::endr << 0 << arma::endr      // Angular Position (Body Attached)
              << 0 << arma::endr << 0 << arma::endr << 0 << arma::endr      // Angular Velocity (Body Attached)
              << 0 << arma::endr << 0 << arma::endr << 0.08 << arma::endr   // Linear Postion (Inertial Frame)
              << 0 << arma::endr << 0 << arma::endr << 0;                   // Linear Velocity (Body Attached)

    // Set Boundaries (zero value means unbounded)
    boundary.zeros(q.size(),2);
    boundary.col(0).rows(0,1).fill(-3.15);
    boundary.col(0).rows(3,5).fill(-5);
    boundary.col(0).rows(6,7).fill(-0.15);

    boundary.col(1).rows(0,1).fill(3.15);
    boundary.col(1).rows(3,5).fill(5);
    boundary.col(1).rows(6,8).fill(0.15);

    // Objects Creation
    Robobee bee(q, CTRL_HZ);     // ROBOBEE
    Controller ctr(q_desired);    // Controller
    Decoder dec(1.0);             // Decoder


/*==================
|   NETWORK INPUT  |
==================*/
    const double max_psg = 500,            
                 states[] = {0,3},         // Define array with States' ID you want to use             
                 resState[] = {7,7},       // Number of place cells per state
                 ranges[] = {2*pi, -3*pi}; // Range per state (negative means symmetric with respect to the origin)

    // Get place cells poisitions
    std::vector <double> currDim;
    std::vector < std::vector<double> > pCells;

    for (int i = 0; i < sizeof(states)/sizeof(states[0]); ++i)
    {
        if (ranges[i] > 0)
        {
            for (int j = 0; j < resState[i]; ++j)
                currDim.push_back(std::abs(ranges[i])*j/resState[i]);
        }
        else
        {
            for (int j = -(resState[i]-1)/2; j <= (resState[i]-1)/2; ++j)
                currDim.push_back(std::abs(ranges[i])*j/resState[i]);
        }
        pCells.push_back(currDim);
        currDim.clear();
    }

    // Define all parameters for dynamic nested loops
    int MAXROWS = pCells.size(), // Max depth
        arrs[pCells.size()],     // Array to move inside a level 
        exit_cond = 0,           // Variable for exit condition
        total = 0,               // Variable used to check exit condition
        cellsCounter = 0;        // Holds the current channel       

    bool status = false, // bool variable for external loop
         change = true; // bool variable for internal loop

    double poisson_rate = 0;        // Rate to feed the poisson spike generator

    for (int r=0;r<MAXROWS;r++)
        arrs[r] = 0;
                          
    for (int i = 0; i < MAXROWS; ++i)
        exit_cond = exit_cond + (pCells[i].size()-1);
    

/*==================
|   NETWORK OUTPUT |
==================*/
    int critic_size = 50,
        actor_size = 60,
        dopa_size = 100,
        pops_size [] = {critic_size, actor_size, dopa_size};

    double valueFunction, policy, dopaActivity,
           A_critic = 1,
           A_dopa = 1,
           b_critic = 0,
           b_dopa = 0,
           tau_r = 1,
           F_max = 2e-6,
           sumActor = 0;

    std::vector < std::vector<double> > *curr_pop;


/*==================
|   MUSIC          |
==================*/
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

    // Generate an instance of Receiver
    Receiver inhandler(pops_size, sizeof(pops_size)/sizeof(pops_size[0]));

    // Mapping Input/Output Port
    outdata->map(&outindex, MUSIC::Index::GLOBAL);
    indata->map(&inindex, &inhandler, IN_LATENCY,1);


/*==================
|   OPENGL         |
==================*/
    bool ANIMATE = true;

    Display *myFrame = new Display("Hello World!", 800, 600);
    Camera *myCam = new Camera(glm::vec3(-0.3,0.2,0.4), glm::vec3(0,0,0), glm::vec3(0,1,0));
    Light *Lamp = new Light( glm::vec3(2,1,5) );
    Shader *myShader = new Shader("../graphic/shaders/vertex.glsl", "../graphic/shaders/fragment.glsl");

    

    // PLANE
    GLfloat vertex_data[] = {
        -0.1, 0.0, 0.1,
         0.1, 0.0, 0.1,
         0.1, 0.0,-0.1,
        -0.1, 0.0,-0.1
    };

    GLfloat uv_data[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    GLfloat normal_data[] = {
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    };

    unsigned short indices[] = {0, 1, 2, 2, 3, 0};  


    Model Plane(vertex_data, uv_data, normal_data, sizeof(vertex_data)/sizeof(vertex_data[0]));

    Plane.SetInidices(indices, sizeof(indices)/sizeof(indices[0]));

    Mesh meshPlane(Plane);

    Texture PlaneTex("../graphic/textures/test.png");

    Transform PlaneState;

    PlaneState.SetPos(glm::vec3(0,-0.013/2,0));

    
    // ROBOBEE
    Model Cube;

    Mesh meshCube("../graphic/objs/cube.obj", Cube);

    Texture CubeTex("../graphic/textures/bricks.jpg");

    Transform CubeState;

    // // 2D TEXT
    // char text[256];
    // Writer Tempo;
    
    // // GRAPHs
    // Display myFrame2("Hello World2!", 800, 600);
    // int num = 3;
    // double max [] = {2.5, 2.5, 2.5}, min [] = {-2.5, -2.5, -2.5}, data [num];
    // Plotter giotto(max, min, num);

    if (!ANIMATE)
        delete myFrame, myCam, myShader, Lamp;



/*==================
|   RECORDING      |
==================*/
    // Create Saving Folder
    time_t timer = time(NULL);

    struct tm *tm_struct = localtime(&timer);

    char *fname;
    int dummy = asprintf(&fname, "Simulations/simtime_%d-%d-%d", tm_struct->tm_hour, tm_struct->tm_min, tm_struct->tm_sec);

    boost::filesystem::path dir(fname);
    if(boost::filesystem::create_directory(dir)) {
        std::cout << "Success" << "\n";
    }

    std::string folder(fname);

    // Brain Output Signals
    arma::mat network(3, CTRL_HZ*simt + 1);
    enum
    {
        VALUEFUN,

        POLICY,

        DOPA    
    };

    double vRes = 0.1,
           vRows= (boundary(0,1)-boundary(0,0))/vRes + 1,
           vCols= (boundary(3,1)-boundary(3,0))/vRes + 1;

    arma::mat valueMat(vRows+1,vCols+1, arma::fill::zeros);

    valueMat(0,0) = vCols+1;
    valueMat.col(0).rows(1,vRows) = arma::regspace<arma::vec>(boundary(0,0), vRes, boundary(0,1));
    valueMat.row(0).cols(1,vCols) = arma::regspace<arma::rowvec>(boundary(3,0), vRes, boundary(3,1));

    for (int i = 0; i < vRows; ++i)
        valueMat.col(0).row(i) = round(valueMat.col(0).row(i)/vRes)*vRes;

    for (int i = 0; i < vCols; ++i)
        valueMat.row(0).col(i) = round(valueMat.row(0).col(i)/vRes)*vRes;

    // Environment Calculations
    arma::mat envorinment(2, CTRL_HZ*simt + 1);

    enum
    {
        REWARD,

        TDERROR    
    };

    // Robobee parameters
    arma::mat rec_state(q.size(), CTRL_HZ*simt + 1), 
              rec_time(CTRL_HZ*simt + 1, 1), 
              rec_control(u.size(),CTRL_HZ*simt + 1);

    // OpenGL frames
    std::string cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 800x600 -i - "
                    "-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip " + folder + "/flight.mp4";

    FILE* ffmpeg = popen(cmd.c_str(), "w");

    int length = 800, height = 600;
    int* buffer = new int[length*height];

    double frameRate = 0.01;

    // Initialize
    int iter = 0;

    rec_state.col(iter) = q;
    rec_time(iter, 0) = 0;

    network.col(iter).fill(0);

    envorinment.col(iter).fill(50*cos(q(0,0)));

    iter++;

/*========================================================================================================================*/ 
    
  
    
/*=====================================================RUNTIME PHASE======================================================*/
    // Create runtime object -> start runtime phase (end setup phase)
    MUSIC::Runtime *runtime = new MUSIC::Runtime(setup, TICK);
    double tickt = runtime->time();

    while (tickt < simt) {

        if (ANIMATE)
        {
            if(std::abs(remainder(tickt,frameRate)) < 0.00001)
            {
                myFrame->Clear(0.0f, 0.1f, 0.15f, 1.0f);
        
                myShader->Bind();
                
                // ROBOBEE
                CubeState.SetPos( glm::vec3( q(7,0), q(8,0), q(6,0) ) );
                CubeState.SetRot( glm::vec3( q(1,0), q(2,0), q(0,0) ) );
                myShader->Update( CubeState, myCam, Lamp );
                CubeTex.Bind(0);
                meshCube.Draw();
            
                // PLANE
                myShader->Update( PlaneState, myCam, Lamp );
                PlaneTex.Bind(0);
                meshPlane.Draw();
        
                // // TEXT
                // sprintf(text,"%.3fsec", tickt );
                // Tempo.Print(text);
                
                myFrame->Update();
                
                glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
                fwrite(buffer, sizeof(int)*length*height, 1, ffmpeg);
                
        
                // myFrame2.Clear(0.0f, 0.0f, 0.0f, 1.0f);
        
                // // GRAPH
                // data[0] = q(3,0);
                // data[1] = q(4,0);
                // data[2] = q(5,0);
        
                // giotto.SetData(data, tickt);
                // giotto.Plot();
        
                // myFrame2.Update();
            }
        }
        // Classical Controller calculates 3 control torques
        u = arma::join_vert(ctr.AltitudeControl(q), ctr.DampingControl(q));

        // Dynimic nested loop to feed place cells
        while (!status) { 
    
            total = 0;

            for (int r=0;r<MAXROWS;r++)
                total +=arrs[r];

            if (total == exit_cond)
                status = true;

            change = true;
            int r = MAXROWS-1; // Variable to move through levels
            
            while (change && r>=0) {
            
                if (++arrs[r] > pCells[r].size()-1)
                    change = true;
                else
                    change = false;

                if (r==MAXROWS-1)
                {
                    poisson_rate = max_psg;
                    for (int i = 0; i < r; ++i)
                    {
                        if (states[i] < 3 && std::abs(2*pi + q(states[i],0) - pCells[i][arrs[i]]) < std::abs(q(states[i],0) - pCells[i][arrs[i]]))
                            poisson_rate = poisson_rate/std::exp(std::pow(2*pi + q(states[i],0) - pCells[i][arrs[i]], 2) / std::pow(std::abs(ranges[i])/resState[i], 2));
                        else
                            poisson_rate = poisson_rate/std::exp(std::pow(q(states[i],0) - pCells[i][arrs[i]], 2) / std::pow(std::abs(ranges[i])/resState[i], 2));

                    }
                    if (states[r] < 3 && std::abs(2*pi + q(states[r],0) - pCells[r][arrs[r]-1]) < std::abs(q(states[r],0) - pCells[r][arrs[r]-1]))
                        poisson_rate = poisson_rate/std::exp(std::pow(2*pi + q(states[r],0) - pCells[r][arrs[r]-1], 2) / std::pow(std::abs(ranges[r])/resState[r], 2));
                    else
                        poisson_rate = poisson_rate/std::exp(std::pow(q(states[r],0) - pCells[r][arrs[r]-1], 2) / std::pow(std::abs(ranges[r])/resState[r], 2));
                    
                    send_poisson(outdata, poisson_rate, tickt, cellsCounter);
                    cellsCounter++;
                }

                if (change)
                    arrs[r] = 0;

                r=r-1;
            }
        }
        
        status = false;
        cellsCounter = 0;

        // Reward Delivery
        send_poisson(outdata, 0, tickt, width[0]-1);

        // Music Communication: spikes are sent and received here
        runtime->tick();
        tickt = runtime->time();

        // Policy
        curr_pop = inhandler.GetSpikes(1);
        policy = 0;
        sumActor = 0;
        for (int i = 0; i < curr_pop->size(); ++i){
            sumActor = sumActor + dec.AlphaKernel(tickt, &(*curr_pop)[i]);
            policy = policy + dec.AlphaKernel(tickt, &(*curr_pop)[i])*(2*F_max*i/curr_pop->size() - F_max);
        }

        if (sumActor == 0)
            policy = 0;
        else
            policy = policy/sumActor;

            
        // Dopaminergi neurons activity
        curr_pop = inhandler.GetSpikes(2);
        dopaActivity = 0;
        for (int i = 0; i < curr_pop->size(); ++i)
            dopaActivity = dopaActivity + dec.ExpKernel(tickt, &(*curr_pop)[i]);

        dopaActivity = (A_dopa/curr_pop->size())*dopaActivity + b_dopa;

        // Value Function
        curr_pop = inhandler.GetSpikes(0);
        valueFunction = 0;
        TD_error = 0;
        for (int i = 0; i < curr_pop->size(); ++i){
            valueFunction = valueFunction + dec.AlphaKernel(tickt, &(*curr_pop)[i]);
            TD_error = TD_error + dec.NLKernelDev(tickt, &(*curr_pop)[i]) - dec.NLKernel(tickt, &(*curr_pop)[i])/tau_r;
        }

        valueFunction = (A_critic/curr_pop->size())*valueFunction + b_critic;
        TD_error = (A_critic/curr_pop->size())*TD_error + b_critic/tau_r + reward;

        for (int i = 0; i < valueMat.n_rows; ++i)
        {
            if (valueMat(i,0) == round(q(0,0)/vRes)*vRes)
            {
                for (int j = 0; j < valueMat.n_cols; ++j)
                {
                    if (valueMat(0,j) == round(q(3,0)/vRes)*vRes)
                        valueMat(i, j) = valueFunction;
                }
            }
        }

        // Environment (RoboBee) generates the new state and reward 
        q = bee.BeeDynamics(u);
        reward = 50*cos(q(0,0));

        rec_time(iter,0) = tickt;
        rec_state.col(iter) = q;

        network(VALUEFUN, iter) = valueFunction;
        network(POLICY, iter) = policy;
        network(DOPA, iter) = dopaActivity;

        envorinment(REWARD, iter) = reward;
        envorinment(TDERROR, iter) = TD_error;

        iter++;
    }

    // End runtime phase
    runtime->finalize();
/*========================================================================================================================*/ 


    
/*=====================================================SAVING PHASE=======================================================*/
    
    char *imagesPath;
    arma::mat toPlot;

    Gnuplot gp;
    gp << "set terminal png\n";

    // State
    dummy = asprintf(&imagesPath, "set output '%s/inputState.png'\n", fname);
    gp << imagesPath;
    gp << "set multiplot layout 2,1 title 'Input State'\n";
    gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    
    gp << "unset xlabel\nset ylabel 'theta 1 [rad]'\n";
    gp << "unset xrange\nset yrange [-pi:pi]\n";
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_time,rec_state.row(0).t());
    gp.send1d(toPlot);
    
    gp << "set xlabel 'time [s]'\nset ylabel 'omega 1 [rad/s]'\n";
    gp << "unset xrange\nset yrange [-5:5]\n";
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_time,rec_state.row(3).t());
    gp.send1d(toPlot);
    gp << "unset multiplot\n";

    // Lateral Positions
    dummy = asprintf(&imagesPath, "set output '%s/lateral_pos.png'\n", fname);
    gp << imagesPath;
    gp << "set multiplot layout 1,2 title 'Lateral Position'\n";
    gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    
    gp << "set xlabel 'x position (m)'\nset ylabel 'z position (m)'\n";
    // gp << "set xrange [-0.15:0.15]\nset yrange [0:0.15]\n";
    gp << "unset xrange\nunset yrange\n"; 
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_state.row(6).t(),rec_state.row(8).t());
    gp.send1d(toPlot);
    
    gp << "set xlabel 'y position (m)'\nunset ylabel\n";
    // gp << "set xrange [-0.15:0.15]\nset yrange [0:0.15]\n";
    gp << "unset xrange\nunset yrange\n"; 
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_state.row(7).t(),rec_state.row(8).t());
    gp.send1d(toPlot);
    gp << "unset multiplot\n";
    
    // Reward and Dopaminergic Activity
    dummy = asprintf(&imagesPath, "set output '%s/rew-dopa.png'\n", fname);
    gp << imagesPath;
    gp << "set multiplot layout 2,1 title 'Reward'\n";
    gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";

    gp << "unset xlabel\nset ylabel 'reward [reward units]'\n";
    gp << "unset xrange\nset yrange [-50:50]\n";
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_time,envorinment.row(REWARD).t());
    gp.send1d(toPlot);

    gp << "set xlabel 'time [s]'\nset ylabel 'Dopaminergic Activity [reward units]'\n";
    gp << "unset xrange\nset yrange [-50:50]\n";
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_time,network.row(DOPA).t());
    gp.send1d(toPlot);
    gp << "unset multiplot\n";

    // Value Function
    dummy = asprintf(&imagesPath, "set output '%s/vFunc.png'\n", fname);
    gp << imagesPath;
    gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";

    gp << "set xlabel 'time [s]'\nset ylabel 'Value Function'\n";
    gp << "unset xrange\nset yrange [-50:50]\n";
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_time,network.row(VALUEFUN).t());
    gp.send1d(toPlot);

    // Policy
    dummy = asprintf(&imagesPath, "set output '%s/policy.png'\n", fname);
    gp << imagesPath;
    gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
   
    gp << "set xlabel 'time [s]'\nset ylabel 'Policy'\n";
    gp << "unset xrange\nset yrange [-2e-6:2e-6]\n";  
    gp << "plot '-' with lines notitle\n";
    toPlot = arma::join_rows(rec_time,network.row(POLICY).t());
    gp.send1d(toPlot);

    // Value Function Surface
    dummy = asprintf(&imagesPath, "set output '%s/ValueFunc.png'\n", fname);
    gp << imagesPath;
    gp << "set title 'State-Value Function'\n";
    gp << "set pm3d; set palette\n";
    gp << "set hidden3d\n";
    gp << "set dgrid3d 50,50 qnorm 2\n";
    gp << "set grid ytics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set grid xtics lc rgb '#bbbbbb' lw 0.1 lt 0\n";
    gp << "set hidden3d\n";
    gp << "set xlabel 'theta 1 [rad]'\nset ylabel 'omega 1 [rad/s]'\nset zlabel 'VF [reward units]'\n";
    gp << "unset xrange\nunset yrange\nunset zrange\n";
    gp << "splot " << gp.file1d(valueMat) << "matrix nonuniform with lines notitle\n";

    
    std::string loadfolder("BeeBrain");
    Iomanager manager(loadfolder, folder);
    std::vector <double> criticWeights = manager.ReadData("weightsCritic.out"),
                         actorWeights = manager.ReadData("weightsActor.out");
    manager.PrintData("weightsCritic.dat", &criticWeights);
    manager.PrintData("weightsActor.dat", &actorWeights);

    rec_state.save(folder + "/state.mat"); // arma::raw_ascii
    rec_time.save(folder + "/simtime.mat");
    network.save(folder + "/network.mat");
    envorinment.save(folder + "/envorinment.mat");
    valueMat.save(folder + "/valueMatrix.mat", arma::raw_ascii);
    
/*========================================================================================================================*/ 



/*====================================================CLEANING PHASE======================================================*/

    // Music
    delete runtime, gp;

    // OpenGL
    if (ANIMATE)
        delete myFrame, myCam, myShader, Lamp;

/*========================================================================================================================*/ 
}