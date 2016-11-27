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
: b {0.01, 0.01, 0.02}, a {0.001, 1, 0}
{
	g = 9.81;
	m = 111e-6;
	max_f_l = 1.5 * m * g; 
	max_torque = 2e-6;

	q_d = q_desired;

	init = 0;

	int dim = sizeof(a)/sizeof(a[0]) - 1;
	A.zeros(dim,dim);
	B.zeros(dim,1);
	C.zeros(1,dim);
	D.zeros(1,1);
	x.zeros(dim,1);
	f_l.zeros(1,1);
	e.zeros(1,1);

	// General second-order biquad transformation
	double k = 2/0.001;

	b[0] = (b[0]*std::pow(k,2) + b[1]*k + b[2]) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
	b[1] = (2*b[2] - 2*b[0]*std::pow(k,2)) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
	b[2] = (b[0]*std::pow(k,2) - b[1]*k + b[2]) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);

	a[0] = 1;
	a[1] = (2*a[2] - 2*a[0]*std::pow(k,2)) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
	a[2] = (a[0]*std::pow(k,2) - a[1]*k + a[2]) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);

}

Controller::~Controller() {}

arma::mat Controller::AltitudeControl(arma::mat& q)
{
	e = q_d(8,0) - q(8,0);

	if (!init){

		// Observable Canonical Form
		// A(0,0) = -a[1];
		// A(1,0) = -a[2];
		// A(0,1) = 1;

		// B(0,0) = b[1] - a[1]*b[0];
		// B(1,0) = b[2] - a[2]*b[0];

		// C(0,0) = 1;

		// D(0,0) = b[0];

		// 10000Hz
		// A(0,0) = 1.904837418035960;
		// A(1,0) = 1; // 0.5;
		// A(0,1) = -0.904837418035960;

		// B(0,0) = 1; // 4;

		// C(0,0) = -0.904685920089069;
		// C(0,1) = 0.904686110414232;

		// D(0,0) = 9.516741970724031;

		// 1000Hz
		A(0,0) = 1.367879441171442;
		A(1,0) = 0.5;
		A(0,1) = -0.7357588823428847;

		B(0,0) = 4;

		C(0,0) = -0.9979390591140898;
		C(0,1) = 1.995884439433767;

		D(0,0) = 6.324887025108467;

		init = 1;
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