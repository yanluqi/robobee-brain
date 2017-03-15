#!/usr/local/bin/python

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
simt = 10000.0
# Load network's parameters
box = ParametersBox()
# Create an instance of the class BeeBrain
net = BeeBrain()
# Create nodes inside BeeBrain's instance with paramters chosen
net.build(box)

# Create connections among nodes
if LOAD:
	weightsCritic = np.genfromtxt('BeeBrain/weightsCritic.dat', delimiter=",")
	weightsAritic = np.genfromtxt('BeeBrain/weightsActor.dat', delimiter=",")
	net.connect(box, weightsCritic, weightsAritic)
else:
	net.connect(box)

# Get Actor and Critic connections
connCritic = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.critic)[0])
connActor = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.actor)[0])

# Get Actor and Critic weights BEFORE simulation
matCritic_start = weightMatrix(connCritic)
matActor_start = weightMatrix(connActor)
weightsCritic_start = np.array(nest.GetStatus(connCritic, 'weight'))
weightsActor_start = np.array(nest.GetStatus(connActor, 'weight'))

# Run Simulation
if MONITOR:
	epoch = 0
	weight_mean = []
	dopa_rate = []
	tempo = []
	weight_mean.append(np.mean(nest.GetStatus(connCritic, 'weight')))
	dopa_rate.append(nest.GetStatus(connCritic, 'c')[100])
	tempo.append(epoch)

	while epoch < simt:
		net.run(box,1.0)
		epoch += 1.0
		weight_mean.append(np.mean(nest.GetStatus(connCritic, 'weight')))
		dopa_rate.append(nest.GetStatus(connCritic, 'c')[100])
		tempo.append(epoch)

	plt.figure()
	plt.plot(tempo, weight_mean)
	plt.grid(True)
	plt.savefig('BeeBrain/weightsMean.png', bbox_inches='tight')
	np.savetxt('BeeBrain/weight_mean.dat', weight_mean, delimiter=',')

	plt.figure()
	plt.plot(tempo, dopa_rate)
	plt.grid(True)
	plt.savefig('BeeBrain/dopaRate.png', bbox_inches='tight')
	np.savetxt('BeeBrain/dopa_rate.dat', dopa_rate, delimiter=',')
else:
	net.run(box, simt)

# Get Actor and Critic weights AFTER simulation
weightsCritic_end = np.array(nest.GetStatus(connCritic, 'weight'))
weightsActor_end = np.array(nest.GetStatus(connActor, 'weight'))
matCritic_end = weightMatrix(connCritic)
matActor_end = weightMatrix(connActor)

# Plot & Save weights change
plot3Dweights(matCritic_end - matCritic_start,'Critic Weights','Critic Neurons','Place Cells')
plt.savefig('BeeBrain/wCriticChange.png', bbox_inches='tight')
plot3Dweights(matActor_end - matActor_start,'Actor Weights','Actor Neurons','Place Cells')
plt.savefig('BeeBrain/wActorChange.png', bbox_inches='tight')

# Save weights and connections
np.savetxt('BeeBrain/pCellsIDs.dat', nest.GetNodes(net.cortex)[0], delimiter=',')
np.savetxt('BeeBrain/criticIDs.dat', nest.GetNodes(net.critic)[0], delimiter=',')
np.savetxt('BeeBrain/actorIDs.dat', nest.GetNodes(net.actor)[0], delimiter=',')

wr = nest.GetStatus(net.wr, 'events')[0]
np.savetxt('BeeBrain/weightStory.dat', np.column_stack((wr['senders'],wr['targets'], wr['times'], wr['weights'])), delimiter=',')

np.savetxt('BeeBrain/connCritic.dat', connCritic, delimiter=',')
np.savetxt('BeeBrain/connActor.dat', connActor, delimiter=',')
np.savetxt('BeeBrain/weightsCritic_start.dat', weightsCritic_start, delimiter=',')
np.savetxt('BeeBrain/weightsActor_start.dat', weightsActor_start, delimiter=',')
np.savetxt('BeeBrain/matCritic_start.dat', matCritic_start, delimiter=',')
np.savetxt('BeeBrain/matActor_start.dat', matActor_start, delimiter=',')
np.savetxt('BeeBrain/weightsCritic_end.dat', weightsCritic_end, delimiter=',')
np.savetxt('BeeBrain/weightsActor_end.dat', weightsActor_end, delimiter=',')
np.savetxt('BeeBrain/matCritic_end.dat', matCritic_end, delimiter=',')
np.savetxt('BeeBrain/matActor_end.dat', matActor_end, delimiter=',')


# Plot & Save Cortex activity
# nest.raster_plot.from_device(net.sdetector, hist=True)
# plt.savefig('BeeBrain/spikes_cortex.png', bbox_inches='tight')
