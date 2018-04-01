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

Controller::Controller(arma::vec& q_desired) // : b {0.01, 0.01, 0.02}, a {0.001, 1, 0}
{
	g = 9.81;
	m = 111e-6;
	max_f_l = 1.5 * m * g;
	max_torque = 2e-6;
	T = 0.01;

	q_d = q_desired;

	f_l.zeros(1);
	tauc_e.zeros(3);
	tau_c.zeros(3);

	fl_k = new double[3] {0.003, 0, 0};
	fl_e = new double[3] {0};
	prev_q = -10;
	dt = 0.001;

	tauc_k = new double[3] {0.0, -2e-7, 0.0}; // -1e-6

	init = 0;
}

Controller::~Controller()
{
	delete fl_k;
	delete fl_e;
	delete tauc_k;
}

arma::vec Controller::AltitudeControl(arma::vec& q)
{
	fl_e[0] = q_d(8,0) - q(8,0);
	fl_e[1] = q_d(11,0) - q(11,0);

	if (!init){
		int dim = 2;
		A.zeros(dim,dim);
		B.zeros(dim,1);
		C.zeros(1,dim);
		D.zeros(1,1);
		x.zeros(dim);

		// 1000Hz
		A(0,0) = 1.367879441171442;
		A(1,0) = 0.5;
		A(0,1) = -0.7357588823428847;
		B(0,0) = 4;
		C(0,0) = -0.9979390591140898;
		C(0,1) = 1.995884439433767;
		D(0,0) = 6.324887025108467;

		// 10000Hz
		// A(0,0) = 1.904837418035960;
		// A(1,0) = 1;
		// A(0,1) = -0.904837418035960;
		// B(0,0) = 1;
		// C(0,0) = -0.904685920089069;
		// C(0,1) = 0.904686110414232;
		// D(0,0) = 9.516741970724031;

		init = 1;
	}
	else{
		x = A*x + B*fl_e[0];
		f_l = C*x + D*fl_e[0];
	}

	f_l = f_l + 0.8*m*g;

	if (f_l(0) > max_f_l)
		f_l(0) = max_f_l;
	else if (f_l(0) < -max_f_l)
		f_l(0) = -max_f_l;

	// if (prev_q != -10)
	// 	fl_e[2] = fl_e[2] + ((q_d(8,0) - prev_q) + fl_e[0])*dt/2;
	//
	// f_l(0,0) = fl_k[0]*fl_e[0] + fl_k[1]*fl_e[1] + fl_k[2]*fl_e[2] + m*g;
	// prev_q = q(8,0);

	return f_l;
}


arma::vec Controller::DampingControl(arma::vec& q)
{
	// Discrete Controller for the deerivative term (q.rows(3,5) -> omegabody)
	// tau_c = tau_c + 2/T*tauc_k[1]*(q.rows(3,5) - tauc_e);
	// tauc_e = q.rows(3,5);

	tau_c = tauc_k[0] * q.rows(0,2) + tauc_k[1] * q.rows(3,5); // thetabody and omegabody

	for (int i; i!=3; ++i){
		if (tau_c(i) > max_torque)
			tau_c(i) = max_torque;
		else if (tau_c(i) < -max_torque)
			tau_c(i) = -max_torque;
	};

	// tau_c(2) = 0;

	return tau_c;
}

// double a [] = {0.001, 1, 0},
// 			 b [] = {0.01, 0.01, 0.02};
//
// int dim = sizeof(a)/sizeof(a[0]) - 1;
//
// for (int i = 0; i < sizeof(a)/sizeof(a[0]); i++)
// 	a[i] = a[i]/a[0];
// for (int i = 0; i < sizeof(b)/sizeof(b[0]); i++)
// 	b[i] = b[i]/a[0];

// Observable Canonical Form
// for (int i = 0; i < dim; i++) {
// 	A(i,i+1) = 1;
// 	A(i,0) = -a[i+1];
// 	B(i,0) = b[i+1] - a[i+1]*b[0];
// }
// C(0,0) = 1;
// D(0,0) = b[0];

// General second-order biquad transformation
// double k = 2/0.001;
//
// b[0] = (b[0]*std::pow(k,2) + b[1]*k + b[2]) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
// b[1] = (2*b[2] - 2*b[0]*std::pow(k,2)) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
// b[2] = (b[0]*std::pow(k,2) - b[1]*k + b[2]) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
//
// a[0] = 1;
// a[1] = (2*a[2] - 2*a[0]*std::pow(k,2)) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
// a[2] = (a[0]*std::pow(k,2) - a[1]*k + a[2]) / (a[0]*std::pow(k,2) + a[1]*k + a[2]);
