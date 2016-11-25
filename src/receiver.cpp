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
}

Receiver::Receiver ()
{

}

Receiver::~Receiver ()
{
	// Put something here
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