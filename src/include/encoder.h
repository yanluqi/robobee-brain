/*
 *  encoder.h
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

#ifndef ENCODER_H
#define ENCODER_H

#include <music.hh>
#include <boost/random.hpp>
#include <boost/tuple/tuple.hpp>
#include <math.h>

class Encoder {

public:
  // Distributions
  typedef boost::uniform_real<> distType;

  // Numbers Generators
  typedef boost::minstd_rand genType1;    // 16%
  typedef boost::mt19937 genType2;        // 93%
  typedef boost::hellekalek1995 genType3; // 2%
  typedef boost::mt11213b genType4;       // 100%
  typedef boost::taus88 genType5;         // 100%

  // Generator
  typedef boost::variate_generator<genType2&, distType> numGen;

  Encoder (double window);
  Encoder ();
  virtual ~Encoder ();

  void PoissonSpikeGenerator(MUSIC::EventOutputPort* outport, double rate, double tickt, int index);

private:
  double winLength, t;

  //Random Number Generator
  distType *distribution;
  genType2 *generator;
  numGen *numberGenerator;

};

#endif // ENCODER_H
