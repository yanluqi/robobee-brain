/*
 *  robobee.h
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

#ifndef ROBOBEE_H
#define ROBOBEE_H

#include <cmath>
#include <vector>
#include <armadillo>

class Robobee
{
	// Internal Constant Parameters
	double winglength, l, h, J, b_w, c_w, r_w, m, g, ks, force_bias_x,
		   torque_bias_y, gyro_bias_y, force_bias_y, torque_bias_x, gyro_bias_x,
		   freq, dt;

	arma::mat Jmat;

public:
	arma::mat q;

// Methods

public:
	// Constructor
	Robobee(arma::mat& q0, double frequency);
	
	// Destroyer
	~Robobee();
	
	// Bee Dynamic
	arma::mat BeeDynamics(arma::mat& u);

	// // Get Rotation Matrix
	arma::mat RotMatrix(arma::mat& eulerAngles);

	// // Angular velocity -> Rate of Euler Angles
	arma::mat Omega2Thetadot(arma::mat& euler_theta);

	// // Get Aerodynamic force
	void BeeAerodynamics(arma::mat& v, arma::mat& omega, arma::mat *f_d, arma::mat *tau_d);
};

#endif