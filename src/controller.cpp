/*
 *  controller.cpp
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

#include "include/controller.h"

Controller::Controller(arma::mat& q_desired)
{
	m = 111e-6;
	g = 9.81;
	max_f_l = 1.5 * m * g; 
	max_torque = 2e-6;
	initialize = 0;

	q_d = q_desired;
}

Controller::~Controller()
{
	
}

arma::mat Controller::AltitudeControl(arma::mat& q)
{
	arma::mat f_l, e;

	e = q_d(8,0) - q(8,0);

	if (!initialize){
		x.zeros(2,1);

		A << 1.904837418035960 << -0.904837418035960 << arma::endr
	  	  << 1 << 0;
		B << 1 << arma::endr << 0;
		C << -0.904685920089069 << 0.904686110414232;
		D << 9.516741970724031;

		f_l << 0;
		initialize = 1;
	}
	else{
		x = A*x + B*e;
		f_l = C*x + D*e;
	}

	f_l = f_l + 0.8*m*g;

	if (f_l(0,0) > max_f_l)
		f_l(0,0) = max_f_l;
	else if (f_l(0,0) < -max_f_l)
		f_l(0,0) = -max_f_l;

	return f_l;
}


arma::mat Controller::DampingControl(arma::mat& q)
{
	arma::mat omegabody, tau_c;

	omegabody = q.rows(3,5);
	tau_c = -2e-7 * omegabody;

	for (int i; i!=3; ++i){
		if (tau_c(i,0) > max_torque)
			tau_c(i,0) = max_torque;
		else if (tau_c(i,0) < -max_torque)
			tau_c(i,0) = -max_torque;
	};

	tau_c(2,0) = 0;

	return tau_c;
}