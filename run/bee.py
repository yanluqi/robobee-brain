# -*- coding: utf-8 -*-
#
# bee.py
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

#!/usr/local/bin/python

import sys
sys.path.insert(0,'../pynetwork/')
from bee_classes import *
from param_classes import*
from tools import *
import nest.voltage_trace
import nest.raster_plot
import pylab

# Active MONITOR vatiable if you want to recor weights mean change during the simulation
MONITOR = False
# Active LOAD variable if you want to initialize the network with weights got in previous simulations
LOAD = False
# Simulation Time (if you modify this parameter remember to adjust it into the config file too)
simt = 5000.0
# Load network's parameters
box = ParametersBox()
# Create an instance of the class BeeBrain
net = BeeBrain()
# Create nodes inside BeeBrain's instance with paramters chosen
net.build(box)

# Create connections among nodes
if LOAD:
	weightsCritic = np.genfromtxt('data/weightsCritic.out', delimiter=",")
	weightsAritic = np.genfromtxt('data/weightsActor.out', delimiter=",")
	net.connect(box, weightsCritic, weightsAritic)
else:
	net.connect(box)

# Get Actor and Critic connections
connCritic = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.critic)[0])
connActor = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.actor)[0])

# Get Actor and Critic weights BEFORE simulation
wCritic_start = weightMatrix(connCritic)
wActor_start = weightMatrix(connActor)
weightsCritic_pre = np.array(nest.GetStatus(connCritic, 'weight'))
weightsActor_pre = np.array(nest.GetStatus(connActor, 'weight'))

# Run Simulation
if MONITOR:
	epoch = 0
	weight_mean = []
	tempo = []
	weight_mean.append(np.mean(nest.GetStatus(connCritic, 'weight')))
	tempo.append(epoch)

	while epoch < simt:
		net.run(box,1.0)
		epoch += 1.0
		weight_mean.append(np.mean(nest.GetStatus(connCritic, 'weight')))
		tempo.append(epoch)

	plt.figure()
	plt.plot(tempo, weight_mean)
	plt.grid(True)
	plt.savefig('BeeBrain/weightsMean.png', bbox_inches='tight')
	np.savetxt('BeeBrain/weight_mean.out', weight_mean, delimiter=',')
else:
	net.run(box, simt)

# Get Actor and Critic weights AFTER simulation
wCritic_end = weightMatrix(connCritic)
wActor_end = weightMatrix(connActor)

# Plot & Save weights change
plot3Dweights(wCritic_end - wCritic_start)
plt.savefig('BeeBrain/weightsCritic.png', bbox_inches='tight')
plot3Dweights(wActor_end - wActor_start)
plt.savefig('BeeBrain/weightsActor.png', bbox_inches='tight')

# Save weights and connections
weightsCritic_post = np.array(nest.GetStatus(connCritic, 'weight'))
weightsActor_post = np.array(nest.GetStatus(connActor, 'weight'))
np.savetxt('BeeBrain/weightsCritic.out', weightsCritic_post, delimiter=',')
np.savetxt('BeeBrain/weightsActor.out', weightsActor_post, delimiter=',')
np.savetxt('BeeBrain/connCritic.out', connCritic, delimiter=',')
np.savetxt('BeeBrain/connActor.out', connActor, delimiter=',')

# Plot & Save Cortex activity
nest.raster_plot.from_device(net.sdetector, hist=True)
plt.savefig('BeeBrain/spikes_cortex.png', bbox_inches='tight')