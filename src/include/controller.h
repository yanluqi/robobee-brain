/*
 *  controller.h
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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cmath>
#include <vector>
#include <armadillo>

class Controller
{
	// Contreller Bounds
public:
	double max_f_l, max_torque, m, g;

	// Altitude controller variables
	arma::mat A, B, C, D, x;
	int initialize;

public:
	arma::mat q_d;

public:
	// Constructor
	Controller(arma::mat& q_desired);
	
	// Destructor
	~Controller();
	
	// Altitude Controller
	arma::mat AltitudeControl(arma::mat& q);
	
	// Damping Controller (Attitude)
	arma::mat DampingControl(arma::mat& q);
};

#endif