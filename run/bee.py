#!/usr/bin/env python

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

ms = 1000.0;
# Active MONITOR vatiable if you want to recor weights mean change during the simulation
MONITOR = False
# Active LOAD variable if you want to initialize the network with weights got in previous simulations
LOAD = False
# Simulation Time (if you modify this parameter remember to adjust it into the config file too)
simt = 10*ms;
# Load network's parameters
box = ParametersBox()
# Create an instance of the class BeeBrain
net = BeeBrain()
# Create nodes inside BeeBrain's instance with paramters chosen
net.build(box)

# Create connections among nodes
if LOAD:
	net.connect(box, np.loadtxt('BeeBrain/connToCritic.dat'), np.loadtxt('BeeBrain/connToActor.dat'))
else:
	net.connect(box)

# Get Actor and Critic connections and initial weights
connCritic = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.critic)[0])
connActor = nest.GetConnections(nest.GetNodes(net.cortex)[0], nest.GetNodes(net.actor)[0])
weightsCriticStart = np.array(nest.GetStatus(connCritic, 'weight'))
weightsActorStart = np.array(nest.GetStatus(connActor, 'weight'))

# Run Simulation
if MONITOR:
	epoch = 0
	weight_mean = []
	dopa_rate = []
	tempo = []
	weight_mean.append(np.mean(nest.GetStatus(connCritic, 'weight')))
	dopa_rate.append(nest.GetStatus(connCritic, 'n')[100])
	tempo.append(epoch)

	while epoch < simt:
		net.run(box,1.0)
		epoch += 1.0
		weight_mean.append(np.mean(nest.GetStatus(connCritic, 'weight')))
		dopa_rate.append(nest.GetStatus(connCritic, 'n')[100])
		tempo.append(epoch)

	plt.figure()
	plt.plot(tempo, weight_mean)
	plt.grid(True)
	plt.savefig('BeeBrain/weightsMean.png', bbox_inches='tight')
	np.savetxt('BeeBrain/weight_mean.dat', weight_mean)

	plt.figure()
	plt.plot(tempo, dopa_rate)
	plt.grid(True)
	plt.savefig('BeeBrain/dopaRate.png', bbox_inches='tight')
	np.savetxt('BeeBrain/dopa_rate.dat', dopa_rate)
else:
	net.run(box, simt)

# Record Network Status
connToCritic = np.column_stack((np.array(nest.GetStatus(connCritic, 'source'), dtype=int),
							    np.array(nest.GetStatus(connCritic, 'target'), dtype=int),
								weightsCriticStart,
								np.array(nest.GetStatus(connCritic, 'weight'))))
connToActor = np.column_stack((np.array(nest.GetStatus(connActor, 'source'), dtype=int),
							   np.array(nest.GetStatus(connActor, 'target'), dtype=int),
							   weightsActorStart,
							   np.array(nest.GetStatus(connActor, 'weight'))))
wr = nest.GetStatus(net.wr, 'events')[0]

# Plot & Save weights change
matCriticStart = weightMatrix(connToCritic[:,0], connToCritic[:,1], connToCritic[:,2])
matCriticEnd = weightMatrix(connToCritic[:,0], connToCritic[:,1], connToCritic[:,3])
matActorStart = weightMatrix(connToActor[:,0], connToActor[:,1], connToActor[:,2])
matActorEnd = weightMatrix(connToActor[:,0], connToActor[:,1], connToActor[:,3])
plot3Dweights(matCriticEnd - matCriticStart,'Critic Weights','Critic Neurons','Place Cells')
plt.savefig('BeeBrain/wCriticChange.png', bbox_inches='tight')
plot3Dweights(matActorEnd - matActorStart,'Actor Weights','Actor Neurons','Place Cells')
plt.savefig('BeeBrain/wActorChange.png', bbox_inches='tight')

# Save Network Configuration
np.savetxt('BeeBrain/pCellsIDs.dat', nest.GetNodes(net.cortex)[0])
np.savetxt('BeeBrain/criticIDs.dat', nest.GetNodes(net.critic)[0])
np.savetxt('BeeBrain/actorIDs.dat', nest.GetNodes(net.actor)[0])
np.savetxt('BeeBrain/connToCritic.dat', connToCritic)
np.savetxt('BeeBrain/connToActor.dat', connToActor)
# np.save('BeeBrain/weightStory', np.column_stack((wr['senders'],wr['targets'], wr['times'], wr['weights'])))

# Plot & Save Cortex activity
# cortexSpikes = nest.GetStatus(net.sdetector, 'events')[0]
# np.save('BeeBrain/cortexSpikes.dat', np.column_stack((cortexSpikes['senders'],cortexSpikes['times'])))
# criticSpikes = nest.GetStatus(net.sdetector, 'events')[1]
# np.save('BeeBrain/criticSpikes.dat', np.column_stack((criticSpikes['senders'], criticSpikes['times'])))
# actorSpikes = nest.GetStatus(net.sdetector, 'events')[2]
# np.savetxt('BeeBrain/actorSpikes.dat', np.column_stack((actorSpikes['senders'], actorSpikes['times'])))
# dopaSpikes = nest.GetStatus(net.sdetector, 'events')[3]
# np.save('BeeBrain/dopaSpikes.dat', np.column_stack((dopaSpikes['senders'], dopaSpikes['times'])))

# nest.raster_plot.from_device([net.sdetector[0]], hist=True)
# plt.savefig('BeeBrain/cortexSpikes.png', bbox_inches='tight')
# nest.raster_plot.from_device([net.sdetector[1]], hist=True)
# plt.savefig('BeeBrain/criticSpikes.png', bbox_inches='tight')
# nest.raster_plot.from_device([net.sdetector[2]], hist=True)
# plt.savefig('BeeBrain/actorSpikes.png', bbox_inches='tight')
