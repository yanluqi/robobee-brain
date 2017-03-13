/*
 *  sender.h
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

#ifndef SENDER_H
#define SENDER_H

#include <armadillo>
#include <music.hh>
#include "include/encoder.h"

class Sender
{
public:
  // Constructor
  Sender(MUSIC::EventOutputPort *outport, double TICK);

  // Default Constructor
  Sender();

  // Destructor
  virtual ~Sender();

  void CreatePlaceCells (int numState, int *idState, int *resState, bool *typeState, double *rangeState, double maxRate);
  void SendState (arma::mat& q, double tickt);
  void SendReward(double reward, double tickt);
  double InputRate(
    double reward,
    double minRate, double maxRate,
    double minRew, double maxRew);

protected:

private:
  MUSIC::EventOutputPort *outputPort;
  double pi;

  std::vector < std::vector<double> > pCells;
  int numPlaceCells;

  // State Data
  int *idState,
      *resState;

	bool *angle;

  double *rangeState;

  // Dynamic Nested Loop Variables
  int *arrs,          // Array to move inside a level
      MAXROWS,        // Max depth
      exit_cond,      // Variable for exit condition
      total,          // Variable used to check exit condition
      cellsCounter;   // Holds the current channel

  bool status, // bool variable for external loop
       change; // bool variable for internal loop

  // Dopaminergic Neurons Reward Delivery
  double eta,
         inputRew,
         Q;

  // Encoder Variables
  Encoder *spikeGen;
  double psgRate,
         max_psg,
         window;
};

#endif // SENDER_H
