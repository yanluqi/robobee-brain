/*
 *  receiver.cpp
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

#include "include/receiver.h"

Receiver::Receiver (int *neurons, int num_pops)
{
	storage.resize(num_pops);
	for (int i = 0; i < num_pops; ++i)
	{
		storage[i].resize(neurons[i]);
		if (bound.empty())
			bound.push_back(neurons[i]);
		else
			bound.push_back(bound[i-1] + neurons[i]);
	}

	spikeFilter = new Decoder(1.0);
}

Receiver::Receiver () {}

Receiver::~Receiver ()
{
	delete spikeFilter;
	delete value;
}

void Receiver::operator () (double t, MUSIC::GlobalIndex id)
{
	for (int i = 0; i < storage.size(); ++i)
	{
		if (id >= bound[i] - storage[i].size() && id < bound[i])
		{
			pop = i;
			new_id = id - (bound[i] - storage[i].size());
		}
	}

	storage[pop][new_id].push_back(t);
}

std::vector <std::vector <double> >* Receiver::GetSpikes(int pop)
{
	return &storage[pop];
}

void Receiver::SetCritic(int idPop, double *param)
{
	value = new double[2];

	idCritic = idPop;
	sizeCritic = storage[idCritic].size();
	A_critic = param[0];
	b_critic = param[1];
	tau_r = param[2];
}

void Receiver::SetActor(int idPop, double *param)
{
	idActor = idPop;
	sizeActor = storage[idActor].size();
	F_max = param[0];
	F_min = param[1];
	minID = 0;
	maxID = sizeActor - 1;
	Q = std::abs(maxID-minID)/std::abs(F_max-F_min);
}

void Receiver::SetDopa(int idPop, double *param)
{
	idDopa = idPop;
	sizeDopa = storage[idDopa].size();
	A_dopa = param[0];
	b_dopa = param[1];
}

double* Receiver::GetValue(double tickt, double reward)
{
	value[0] = 0;
	value[1] = 0;
	for (int i = 0; i < storage[idCritic].size(); ++i){
			value[0] = value[0] + spikeFilter->NLKernel(tickt, &storage[idCritic][i]);
			value[1] = value[1] + spikeFilter->NLKernelDev(tickt, &storage[idCritic][i])
								 - spikeFilter->NLKernel(tickt, &storage[idCritic][i])/tau_r;
	}

	value[0] = (A_critic/sizeCritic)*value[0] + b_critic;
	value[1] = (A_critic/sizeCritic)*value[1] - b_critic/tau_r + reward;

	return value;
}

double Receiver::GetAction(double tickt)
{
	policy = 0;
	sumActor = 0;
	for (int i = 0; i < storage[idActor].size(); ++i){
			sumActor = sumActor + spikeFilter->NLKernel(tickt, &storage[idActor][i]);
			policy = policy + spikeFilter->NLKernel(tickt, &storage[idActor][i])*GetForce(i);
	}

	if (sumActor == 0)
			policy = 0;
	else
			policy = policy/sumActor;

	return policy;
}

double Receiver::GetDopa(double tickt)
{
	dopaActivity = 0;
	for (int i = 0; i < storage[idDopa].size(); ++i)
			dopaActivity = dopaActivity + spikeFilter->ExpKernel(tickt, &storage[idDopa][i]);

	dopaActivity = (A_dopa/sizeDopa)*dopaActivity + b_dopa;

	return dopaActivity;
}

double Receiver::GetForce(int k)
{
  return std::abs(double(k)-minID)/Q + F_min;
}
