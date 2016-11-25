/*
 *  decoder.h
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

#ifndef DECODER_H
#define DECODER_H

#include <vector>
#include <cmath>

class Decoder
{
public:
	Decoder(double window);
	
	Decoder();

	~Decoder();

	double BoxKernel(double tickt, std::vector <double> *spikes);

	double AlphaKernel(double tickt, std::vector <double> *spikes);

	double ExpKernel(double tickt, std::vector <double> *spikes);

	double NLKernel(double tickt, std::vector <double> *spikes);
	
	double NLKernelDev(double tickt, std::vector <double> *spikes);

private:
	double delta_t,
		   counter = 0,
		   curr_rate = 0;
};

#endif