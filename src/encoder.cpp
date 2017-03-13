/*
 *  encoder.cpp
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

#include "include/encoder.h"

Encoder::Encoder(double window)
{
  winLength = window;
  t = 0;

  distribution = new distType(0,0.98);
  generator = new genType2(42);
  numberGenerator = new numGen(*generator, *distribution);
}

Encoder::Encoder () {}

Encoder::~Encoder()
{
  delete distribution;
  delete generator;
  delete numberGenerator;
}

void Encoder::PoissonSpikeGenerator(MUSIC::EventOutputPort* outport, double rate, double tickt, int index)
{
  t = -log((*numberGenerator)())/rate;

  while (t<winLength) {
      outport -> insertEvent(tickt+t, MUSIC::GlobalIndex(index));
      t = t - log((*numberGenerator)())/rate;
  }
}
