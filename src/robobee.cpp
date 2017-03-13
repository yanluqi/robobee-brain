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

Robobee::Robobee(arma::mat& q0, double frequency)
{
	// Internal Paramaters
	winglength = 0.6 * 2.54 / 100;
	l = 0.013;
	h = 0.0025;
	J = 1.5e-9;
	b_w = 2.0e-4;
	c_w = std::pow(h/2 + winglength * 2./3, 2) * b_w;
	r_w = .007;
	m = 111e-6;
	g = 9.81;
	ks = 0;
	force_bias_x = 0;
	torque_bias_y = 0;
	gyro_bias_y = 0;
	force_bias_y = 0;
	torque_bias_x = 0;
	gyro_bias_x = 0;
	Jmat.zeros(3,3);
	Jmat(0,0) = J;
	Jmat(1,1) = J;
	Jmat(2,2) = 0.5e-9;

	freq = frequency;
	dt = 1/freq;

	// State
	q = q0;
}

Robobee::Robobee() {}

Robobee::~Robobee()
{

}

arma::mat Robobee::BeeDynamics(arma::mat& u)
{
	arma::mat theta = q.rows(0,2),
						omegabody = q.rows(3,5),
						posworld = q.rows(6,8),
						vbody = q.rows(9,11),
						f_l,
						tau_c = u.rows(1,3),
						R, W,
						f_disturb, tau_disturb,
						f_d, tau_d,
						g_world, f_g,
						theta_ks,
						f, tau,
						fictitious_f, fictitious_tau,
						xdotworld, vdotbody, thetadot, omegadotbody,
						qdot;


	R = RotMatrix(theta);
	W = Omega2Thetadot(theta);

	f_disturb << force_bias_x << arma::endr << force_bias_y << arma::endr << 0;
	tau_disturb << torque_bias_x << arma::endr << torque_bias_y << arma::endr << 0;

	BeeAerodynamics(vbody, omegabody, &f_d, &tau_d); // f_d, tau_d

	g_world << 0 << arma::endr << 0 << arma::endr << -g*m;
	f_g = R.t() * g_world;
	f_l << 0 << arma::endr << 0 << arma::endr << u(0,0);
	f = f_l + f_g + f_disturb + f_d;

	theta_ks << theta(0,0) << arma::endr << theta(1,0) << arma::endr << 0;
	tau = tau_c + tau_d + tau_disturb - ks*theta_ks;

	fictitious_f = m * arma::cross(omegabody, vbody);
	fictitious_tau = arma::cross(omegabody, Jmat * omegabody);

	xdotworld = R * vbody;
	vdotbody = 1/m * (f - fictitious_f);
	thetadot = W * omegabody;
	omegadotbody = arma::solve(Jmat, tau-fictitious_tau);

	qdot = arma::join_vert(thetadot, omegadotbody);
	qdot = arma::join_vert(qdot, xdotworld);
	qdot = arma::join_vert(qdot, vdotbody);

	q = q + dt*qdot;

	return q;
}

arma::mat Robobee::RotMatrix(arma::mat& eulerAngles)
{
	// R matrix to convert 3-vectors in body coords to world coords
	// v = Rv' where v is in world frame and v' is in body frame
	double sx, sy, sz, cx, cy, cz;
	arma::mat R;

	cz = cos(eulerAngles(2,0));
	cy = cos(eulerAngles(1,0));
	cx = cos(eulerAngles(0,0));
	sz = sin(eulerAngles(2,0));
 	sy = sin(eulerAngles(1,0));
	sx = sin(eulerAngles(0,0));

	R << cz*cy << cz*sy*sx - cx*sz << sz*sx + cz*cx*sy << arma::endr
	  << cy*sz << cz*cx + sz*sy*sx << cx*sz*sy - cz*sx << arma::endr
	  << -sy << cy*sx << cy*cx;

	return R;
}

arma::mat Robobee::Omega2Thetadot(arma::mat& euler_theta)
{
	double st1, ct1, tt2, ct2;
	arma::mat W;

	st1 = sin(euler_theta(0,0));
	ct1 = cos(euler_theta(0,0));
	tt2 = tan(euler_theta(1,0));
	ct2 = cos(euler_theta(1,0));

	W << 1 << st1*tt2 << ct1*tt2 << arma::endr
      << 0 << ct1 << -st1 << arma::endr
      << 0 << st1/ct2 << ct1/ct2;

    return W;
  }

void Robobee::BeeAerodynamics(arma::mat& v, arma::mat& omega, arma::mat *f_d, arma::mat *tau_d)
{
	arma::mat r_vect, v_w;

	r_vect << 0 << arma::endr << 0 << arma::endr << r_w;
	v_w = v + arma::cross(omega, r_vect);
	*f_d = -b_w * v_w;
	*tau_d = arma::cross(r_vect, *f_d);
}
