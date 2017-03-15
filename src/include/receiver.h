/*
 *  receiver.h
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

#ifndef RECEIVER_H
#define RECEIVER_H

#include <music.hh>
#include <vector>
#include "include/decoder.h"

//Create a son of the class MUSIC::EventHandlerGLobalIndex to receive spikes from network
class Receiver : public MUSIC::EventHandlerGlobalIndex
{
public:

	Receiver(int *neurons, int num_pops);

	Receiver();

	virtual ~Receiver();

	void operator () (double t, MUSIC::GlobalIndex id);

	std::vector <std::vector <double> >* GetSpikes(int pop);

	void SetCritic(int idPop, double *param);
	void SetActor(int idPop, double *param);
	void SetDopa(int idPop, double *param);

	double* GetValue(double tickt, double reward);
	double GetAction(double tickt);
	double GetDopa(double tickt);
	double GetForce(int k);

protected:

private:
	// Spikes Storing
	int new_id, pop;
	std::vector <int> bound;
	std::vector < std::vector < std::vector<double> > > storage;

	// Spikes Filtering
	double *value, A_critic, b_critic, tau_r,
				 policy, sumActor, F_max, F_min, minID, maxID, Q,
				 dopaActivity, A_dopa, b_dopa;

	int idCritic, sizeCritic,
			idActor, sizeActor,
			idDopa, sizeDopa;

	Decoder *spikeFilter;
};

#endif // RECEIVER_H
