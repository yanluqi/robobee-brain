/*
 *  robobee.cpp
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

#include "include/robobee.h"

Robobee::Robobee(arma::vec& q0, double frequency)
{
	// Constant Paramaters
	winglength = 0.6 * 2.54 / 100;
	l = 0.013;
	h = 0.0025;
	Ks_xy = 0;
	Ks_z = 0;
	J_xy = 1.5e-9;
	J_z = 0.5e-9;
	bw = 2.0e-4;
	cw = std::pow(h/2 + winglength * 2./3, 2) * bw;
	rw = .007;
	m = 111e-6;
	g = 9.81;
	/*-----------------------------------------------*/

	// User Parameters
	freq = frequency;
	dt = 1/freq;
	InitRobot(q0);

	// Dependent Parameters
	rw_vec = {0, 0, rw};
	vw_vec.zeros(3);
	Ks = Ks_xy * arma::eye<arma::mat>(3,3);
	Ks(2,2) = Ks_z;
	J = J_xy * arma::eye<arma::mat>(3,3);
	J(2,2) = J_z;

	R.zeros(3);
	W.zeros(3);
	T.zeros(4);

	f.zeros(3);
	tau.zeros(3);
	f_g = { 0, 0, -g*m };
	f_d.zeros(3);
	tau_d.zeros(3);
	f_disturb.zeros(3);
	tau_disturb.zeros(3);
}

Robobee::Robobee() {}

Robobee::~Robobee()
{

}

void Robobee::InitRobot(arma::vec& q0)
{
	q = q0;
	theta = q.rows(0,2);
	omega = q.rows(3,5);
	pos = q.rows(6,8);
	vel = q.rows(9,11);

	arma::vec c = cos(theta*0.5),
			  s = sin(theta*0.5);

	quat = {c(0)*c(1)*c(2)+s(0)*s(1)*s(2),
            s(0)*c(1)*c(2)-c(0)*s(1)*s(2),
            c(0)*s(1)*c(2)+s(0)*c(1)*s(2),
            c(0)*c(1)*s(2)-s(0)*s(1)*c(2)};
}

arma::vec& Robobee::BeeDynamics(arma::vec& u)
{
	f.zeros();
	f(2) = u(0,0);
	tau = u.rows(1,3);
	Body2World();
	Omega2ThetaDot();
	Quat2QuatDot();

	// Calculate Forces & Torques
	GetAeroForces();
	f = f + R.t() * f_g + f_disturb + f_d;
	tau = tau + tau_d + tau_disturb - Ks*theta;

	// Calculate next state
	// theta = theta + dt * (W * omega);
	quat = quat + dt*(T*quat);
	GetEulerAngles();
	omega = omega + dt * arma::solve(J, tau - arma::cross(omega, J * omega));
	pos = pos + dt * (R * vel);
	vel = vel + dt * (1/m * (f - m * arma::cross(omega, vel)));
	q.rows(0,2) = theta;
	q.rows(3,5) = omega;
	q.rows(6,8) = pos;
	q.rows(9,11) = vel;

	return q;
}

void Robobee::GetAeroForces()
{
	vw_vec = vel + arma::cross(omega, rw_vec);
	f_d = -bw * vw_vec;
	tau_d = arma::cross(rw_vec, f_d);
}

void Robobee::Body2World()
{
	// R matrix to convert 3-vectors in body coords to world coords
	// v = Rv' where v is in world frame and v' is in body frame
	double sx = sin(theta(0,0)),
				 sy = sin(theta(1,0)),
				 sz = sin(theta(2,0)),
				 cx = cos(theta(0,0)),
				 cy = cos(theta(1,0)),
				 cz = cos(theta(2,0));

	R = { {cz*cy, cz*sy*sx - cx*sz, sz*sx + cz*cx*sy},
    		{cy*sz, cz*cx + sz*sy*sx, cx*sz*sy - cz*sx},
    		{-sy, cy*sx, cy*cx} };
}

void Robobee::Omega2ThetaDot()
{
	double st1 = sin(theta(0,0)),
				 ct1 = cos(theta(0,0)),
				 tt2 = tan(theta(1,0)),
				 ct2 = cos(theta(1,0));

	W = { {1, st1*tt2, ct1*tt2},
    		{0, ct1, -st1},
    		{0, st1/ct2, ct1/ct2} };
}

void  Robobee::Quat2QuatDot()
{
	T = { {0, -omega(0), -omega(1), -omega(2)},
    	  {omega(0),  0,  omega(2), -omega(1)},
    	  {omega(1), -omega(2),  0,  omega(0)},
    	  {omega(2),  omega(1), -omega(0),  0} };
  T = 0.5*T;
}

void Robobee::GetEulerAngles()
{
	double qr = quat(0),
		   qi = quat(1),
		   qj = quat(2),
		   qk = quat(3);

	theta(0) = atan2( 2*(qr*qi + qj*qk), 1 - 2*(std::pow(qi,2) + std::pow(qj,2)) );
	theta(1) = asin(2*(qr*qj - qk*qi));
	theta(2) = atan2( 2*(qr*qk + qi*qj), 1 - 2*(std::pow(qj,2) + std::pow(qk,2)) );
}
