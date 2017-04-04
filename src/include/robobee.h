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
public:
	Robobee(arma::vec& q0, double frequency); 											// Constructor
	Robobee();																										// Default
	~Robobee(); 																									// Destroyer
	void InitRobot(arma::vec& q0);													// Set State
	arma::vec& BeeDynamics(arma::vec& u);												// Bee Dynamic

protected:
	void Body2World(); 				// Get Rotation Matrix
	void Omega2ThetaDot(); 	// Angular velocity -> Rate of Euler Angles
	void Quat2QuatDot();
	void GetEulerAngles();
	void GetAeroForces();


private:
	// Constant Parameters
	double winglength, 		 // Robot Wing Length
				 l, 				 		 // Robot height
				 h, 				 		 // Robot width
				 Ks_xy,			 		 //
			 	 Ks_z,			 		 //
			 	 J_xy,			 		 //
			 	 J_z,				 		 //
				 m, 				 		 // mass
				 bw,	 			 		 // aero drag on wings from wind tunnel tests, Ns/m
				 cw, 				 		 // rot drag coeff around z [Nsm]
				 rw, 				 		 // z-dist from ctr of wings to ctr of mass
				 g, 				 		 // gravity acceleration
		   	 freq, 			 		 // Integration frequency
				 dt;				 		 // Integration step

	arma::mat J, 			 		 // Inertial Tensor
						Ks, 				 // Cable Stiffness Matrix
						R, 					 // Rotation Tensor parametrized with Euler Angles (world->body)
						W, 					 // Rotation Tensor parametrized with Euler Angles (omega->theta derivatives)
						T; 					 // Rotation Tensor parametrized with Quaternions (omega->quaternions derivatives)

	arma::vec q, 					 // Robot state
						theta,			 // Attitude expressed in Euler Angles
						omega, 			 // Angular velocities
						pos, 				 // World Position
						vel, 				 // Velocities
						f, tau, 		 // Total forces & torques
						f_g, 				 // Gravity force
						rw_vec,
						vw_vec, 				 // distance vec
						f_d,
						tau_d, 			 // Aerodynamic forces & torques
						f_disturb, 	 //
						tau_disturb, // Disturb forces & torques
						quat; 			 // Quaternions
};

#endif
