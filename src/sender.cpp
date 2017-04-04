/*
 *  sender.cpp
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

#include "include/sender.h"

Sender::Sender(MUSIC::EventOutputPort *outport, double TICK)
{
    outputPort = outport;
    psgRate = 0;
    dist = 0;
    pi = 3.1415926535897;
    window = TICK;
    spikeGen = new Encoder(window);
}

Sender::Sender () {}

Sender::~Sender ()
{
  delete idState;
  delete resState;
  delete angle;
  delete rangeState;
  delete arrs;
  delete spikeGen;
}

void Sender::CreatePlaceCells (int numState, int *stateId, int *stateRes, bool *stateType, double *stateRange, double maxRate)
{
  max_psg = maxRate;
  numPlaceCells=1;
  for (int i = 0; i < numState; i++)
    numPlaceCells = numPlaceCells*stateRes[i];

  // Initialize State Variables
  idState = new int[numState];
  resState = new int[numState];
  angle = new bool[numState];
  rangeState = new double[numState];

  for (int i = 0; i < numState; i++) {
    idState[i] = stateId[i];
    resState[i] = stateRes[i];
  	angle[i] = stateType[i];
  	rangeState[i] = stateRange[i];
  }

  // Place Cells Centers Creation
  std::vector <double> currDim;
  for (int i = 0; i < numState; ++i)
  {
      if (rangeState[i] > 0)
      {
          for (int j = 0; j < resState[i]; ++j)
              currDim.push_back(std::abs(rangeState[i])*j/resState[i]);
      }
      else
      {
          for (int j = -(resState[i]-1)/2; j <= (resState[i]-1)/2; ++j)
              currDim.push_back(std::abs(2*rangeState[i])*j/(resState[i]-1));
      }
      pCells.push_back(currDim);
      currDim.clear();
  }

  // Initialize Dynamic Nested Loop Variables
  MAXROWS = pCells.size();
  arrs = new int[MAXROWS] {0};
  exit_cond = 0;
  total = 0;
  cellsCounter = 0;
  status = false;
  change = true;

  for (int i = 0; i < MAXROWS; ++i)
      exit_cond = exit_cond + (pCells[i].size()-1);
}

void Sender::SendState (arma::vec& q, double tickt)
{
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
              psgRate = max_psg;
              for (int i = 0; i < r; ++i)
              {
                  if (angle[i]){ // Calculate angular distance
                    dist = atan2(sin(q(idState[i]) - pCells[i][arrs[i]]), cos(q(idState[i]) - pCells[i][arrs[i]]));
                    psgRate = psgRate/std::exp(std::pow(dist, 2) / std::pow(std::abs(rangeState[i])/resState[i], 2));
                  }
                  else{ // Calculate linear distance
                    dist = q(idState[i]) - pCells[i][arrs[i]];
                    psgRate = psgRate/std::exp(std::pow(dist, 2) / std::pow(std::abs(rangeState[i])/resState[i], 2));
                  }
              }
              if (angle[r]){ // Calculate angular distance
                dist = atan2(sin(q(idState[r]) - pCells[r][arrs[r]-1]), cos(q(idState[r]) - pCells[r][arrs[r]-1]));
                psgRate = psgRate/std::exp(std::pow(dist, 2) / std::pow(std::abs(rangeState[r])/resState[r], 2));
              }
              else{ // Calculate linear distance
                dist = q(idState[r]) - pCells[r][arrs[r]-1];
                psgRate = psgRate/std::exp(std::pow(dist, 2) / std::pow(std::abs(rangeState[r])/resState[r], 2));
              }

              spikeGen->PoissonSpikeGenerator(outputPort, psgRate, tickt, cellsCounter);
              cellsCounter++;
          }

          if (change)
              arrs[r] = 0;

          r=r-1;
      }
  }

  status = false;
  cellsCounter = 0;
}

void Sender::SendReward(double reward, double tickt)
{
  if (reward >= 0){
    inputRew = InputRate(std::abs(reward), 0, 2000, 0, 300);
    spikeGen->PoissonSpikeGenerator(outputPort, inputRew, tickt, numPlaceCells);
  }
  else if (reward < 0){
    inputRew = InputRate(std::abs(reward), 0, 500, 0, 300);
    spikeGen->PoissonSpikeGenerator(outputPort, inputRew, tickt, numPlaceCells+1);
  }
}

double Sender::InputRate(
  double reward,
  double minRate, double maxRate,
  double minRew, double maxRew)
{
  Q = std::abs(maxRew-minRew)/std::abs(maxRate-minRate);

  return std::abs(reward-minRew)/Q + minRate;
}

// && std::abs(2*pi + q(idState[i]) - pCells[i][arrs[i]]) < std::abs(q(idState[i]) - pCells[i][arrs[i]])
// psgRate = psgRate/std::exp(std::pow(2*pi + q(idState[i]) - pCells[i][arrs[i]], 2) / std::pow(std::abs(rangeState[i])/resState[i], 2));
//
//  && std::abs(2*pi + q(idState[r]) - pCells[r][arrs[r]-1]) < std::abs(q(idState[r]) - pCells[r][arrs[r]-1])
//  psgRate = psgRate/std::exp(std::pow(2*pi + q(idState[r]) - pCells[r][arrs[r]-1], 2) / std::pow(std::abs(rangeState[r])/resState[r], 2));
