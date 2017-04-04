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
public:
	// Constructor
	Controller(arma::vec& q_desired);

	// Destructor
	~Controller();

	// Altitude Controller
	arma::vec AltitudeControl(arma::vec& q);

	// Damping Controller (Attitude)
	arma::vec DampingControl(arma::vec& q);

	inline void Reset() { init = 0; };

protected:

private:

	int init;

	double g, m, T,
		     *fl_e, // e, ed, ei
		   	 *fl_k, // k, kd, ki
		   	 *tauc_k,
		     prev_q, dt,
		     max_f_l,
		     max_torque;

	arma::mat A, B, C, D;

	arma::vec q_d, x, f_l, omega, tau_c, tauc_e;
};

#endif
