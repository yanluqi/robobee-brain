/*
 *  decoder.cpp
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

#include "include/decoder.h"

Decoder::Decoder(double window)
{
	delta_t = window;
	counter = 0;
}

Decoder::Decoder() : Decoder(1.0) {}

Decoder::~Decoder() {}

double Decoder::BoxKernel(double tickt, std::vector <double> *spikes)
{
    if (spikes->empty())
        counter = 0;
    else
    {
        for (int i=0; i!=spikes->size(); ++i) {
            if (spikes->at(i)<=tickt && spikes->at(i)>=tickt - delta_t)
                counter++;
        }
    }

    curr_rate = counter;
    counter = 0;

    return (curr_rate);
}

double Decoder::AlphaKernel(double tickt, std::vector <double> *spikes)
{
    double alpha = 10,
					 max_k = std::pow(alpha,2)*(1/alpha)*std::exp(-1);

    if (spikes->empty())
        counter = 0;
    else
    {
        for (int i=0; i!=spikes->size(); ++i) {
            if (spikes->at(i)<=tickt && spikes->at(i)>=tickt - delta_t)
                counter = counter + std::pow(alpha,2)*(tickt-spikes->at(i))*std::exp(-alpha*(tickt-spikes->at(i)))/max_k;
        }
    }

    curr_rate = counter;
    counter = 0;

    return (curr_rate);
}

double Decoder::ExpKernel(double tickt, std::vector <double> *spikes)
{
    double tau = 0.1;

    if (spikes->empty())
        counter = 0;
    else
    {
        for (int i=0; i!=spikes->size(); ++i) {
            if (spikes->at(i)<=tickt && spikes->at(i)>=tickt - delta_t)
                counter = counter + std::exp(-(tickt-spikes->at(i))/tau);
        }
    }

    curr_rate = counter;
    counter = 0;

    return (curr_rate);
}

double Decoder::NLKernel(double tickt, std::vector <double> *spikes)
{
    double tau_d = 0.1,
					 tau_r = 0.025,
					 max_x = tickt - log(tau_d/tau_r)/(1/tau_r - 1/tau_d),
					 max_k = (std::exp(-(tickt-max_x)/tau_d) - std::exp(-(tickt-max_x)/tau_r))/(tau_d - tau_r);

    if (spikes->empty())
        counter = 0;
    else
    {
        for (int i=0; i!=spikes->size(); ++i) {
            if (spikes->at(i)<=tickt && spikes->at(i)>=tickt - delta_t)
                counter = counter + (std::exp(-(tickt-spikes->at(i))/tau_d) - std::exp(-(tickt-spikes->at(i))/tau_r))/(tau_d - tau_r)/max_k;
        }
    }

    curr_rate = counter;
    counter = 0;

    return (curr_rate);
}

double Decoder::NLKernelDev(double tickt, std::vector <double> *spikes)
{
	double tau_d = 0.1,
				 tau_r = 0.025,
				 max_x = tickt - log(tau_d/tau_r)/(1/tau_r - 1/tau_d),
				 max_k = (std::exp(-(tickt-max_x)/tau_d) - std::exp(-(tickt-max_x)/tau_r))/(tau_d - tau_r);

    if (spikes->empty())
        counter = 0;
    else
    {
        for (int i=0; i!=spikes->size(); ++i) {
            if (spikes->at(i)<=tickt && spikes->at(i)>=tickt - delta_t)
                counter = counter + (-(1/tau_d)*std::exp(-(tickt-spikes->at(i))/tau_d) + (1/tau_r)*std::exp(-(tickt-spikes->at(i))/tau_r))/(tau_d - tau_r)/max_k;
        }
    }

    curr_rate = counter;
    counter = 0;

    return (curr_rate);
}
