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
	delete value_param;
	delete policy_param;
	delete dopa_param;
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
	value_param = new double[3];
	idCritic = idPop;

	for (int i = 0; i < 3; i++)
		value_param[i] = param[i];
}

void Receiver::SetActor(int idPop, double *param)
{
	policy_param = new double[2];
	idActor = idPop;

	for (int i = 0; i < 2; i++)
		policy_param[i] = param[i];
}

void Receiver::SetDopa(int idPop, double *param)
{
	dopa_param = new double[2];
	idDopa = idPop;

	for (int i = 0; i < 2; i++)
		dopa_param[i] = param[i];
}


double* Receiver::GetValue(double tickt, double reward)
{
	value[0] = 0;
	value[1] = 0;
	for (int i = 0; i < storage[idCritic].size(); ++i){
			value[0] = value[0] + spikeFilter->NLKernel(tickt, &storage[idCritic][i]);
			value[1] = value[1] + spikeFilter->NLKernelDev(tickt, &storage[idCritic][i])
								 - spikeFilter->NLKernel(tickt, &storage[idCritic][i])/value_param[2];
	}

	value[0] = (value_param[0]/storage[idCritic].size())*value[0] + value_param[1];
	value[1] = (value_param[0]/storage[idCritic].size())*value[1] - value_param[1]/value_param[2] + reward;

	return value;
}

double Receiver::GetAction(double tickt)
{
	policy = 0;
	sumActor = 0;
	for (int i = 0; i < storage[idActor].size(); ++i){
			sumActor = sumActor + spikeFilter->NLKernel(tickt, &storage[idActor][i]);
			policy = policy + spikeFilter->NLKernel(tickt, &storage[idActor][i])*(2*policy_param[0]*i/storage[idActor].size() - policy_param[0]);
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

	dopaActivity = (dopa_param[0]/storage[idDopa].size())*dopaActivity + dopa_param[1];

	return dopaActivity;
}
