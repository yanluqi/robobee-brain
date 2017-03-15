#!/usr/local/bin/python

# -*- coding: utf-8 -*-
#
# shortcut.py
#
# This file is part of RoboBrain.
# Copyright (C) 2016 Bernardo Fichera
#
# RoboBrain is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# RoboBrain is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with RoboBrain.  If not, see <http://www.gnu.org/licenses/>.

from tools import *
from bee_classes import *
from param_classes import*

nest.ResetKernel()

box = ParametersBox()
net = BeeBrain()

net.build(box)

net.connect(box)

connCritic = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.critic)[0])
connActor = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.actor)[0])

plotNet(net)
W1 = weightMatrix(connCritic)
plot3Dweights(W1, 'Critic Weights','Critic Neurons','Place Cells')
plot2Dweights(W1, 'Critic Weights','Critic Neurons','Place Cells')

W2 = weightMatrix(connActor)
plot3Dweights(W2, 'Actor Weights','Actor Neurons','Place Cells')
plot2Dweights(W2, 'Actor Weights','Actor Neurons','Place Cells')
# plt.show()
