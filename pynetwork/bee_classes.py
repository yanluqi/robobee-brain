# -*- coding: utf-8 -*-
#
# bee_classes.py
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

from __future__ import division
import nest
import nest.topology as tp
import os
import numpy as np

class BeeBrain(object):
  """
  Implementation of the RoboBee's Brain. The model builds 10 sensory 
  neurons (input) connected one-to-one to 10 motor neurons (output).
  The simulation uses MUSIC devices to stimulate the network and receive
  data from it.
  """

  # Model Variables
  msd         =  1000
  dt          =  0.01
  built       = False
  connected   = False
  debug       = False
  rec_nodes   = []

  def __init__(self):
    """
    Initialize an object of this class.
    """
    self.name=self.__class__.__name__
    self.data_path=self.name+"/"
    if not os.path.exists(self.data_path):
        os.makedirs(self.data_path)
    print("Writing data to: {0}".format(self.data_path))
    nest.SetKernelStatus({"data_path": self.data_path,
                          "local_num_threads": 4,
                          # 'resolution': self.dt,
			                    "overwrite_files": True,
			                    "print_time": True})

  def calibrate(self):
    """
    Compute all parameter dependent variables of the
    model.
    """

    # Creating feed numbers for random numbers generators
    self.n_vp = nest.GetKernelStatus('total_num_virtual_procs')
    self.msdrange1 = range(self.msd, self.msd+self.n_vp)
    self.msdrange2 = range(self.msd+self.n_vp+1, self.msd+1+2*self.n_vp)

    # Feed random numbers generatos
    self.pyrngs = [np.random.RandomState(s) for s in self.msdrange1]
    nest.SetKernelStatus({'grng_seed': self.msd+self.n_vp,
                  		  'rng_seeds': self.msdrange2})

  def build(self, box):
    """
    Create all nodes, used in the model.
    """

    if self.built==True: return
    
    self.calibrate()

    #################
    # INPUT PROXIES #
    #################
    """
    Create the input proxies to transmit the state to the cortex populations
    """
    params = box.input_sensor()

    self.in_cortex = tp.CreateLayer({'columns': box.n_CellsState[0], 'rows': box.n_CellsState[1], 'center': params['center'],
                                     'extent': params['extent'], 'elements': 'music_event_in_proxy'})
    for i in range(box.n_pCells):
      nest.SetStatus([nest.GetNodes(self.in_cortex)[0][i]], {'music_channel': i, 'port_name': params['p_name']})

    nest.SetAcceptableLatency(params['p_name'], params['latency'])

    ###############
    # PLACE CELLS #
    ###############
    """
    Create three different pouplations encoding the three angular velocities.
    """
    params = box.cortex()
    nest.CopyModel(params['copymodel'], params['model'], params['settings'])
    self.cortex = {}
    self.inhibition = nest.Create('poisson_generator', 1, params={'rate': 20000.0, 'stop':  500.0})

    x = [params['center'][0] - params['extent'][0]/2 + 0.25, params['center'][0] + params['extent'][0]/2 - 0.25]
    y = [params['center'][1] - params['extent'][1]/2 + 0.25, params['center'][1] + params['extent'][1]/2 - 0.25]
    pos = [[np.random.uniform(x[0],x[1]), np.random.uniform(y[0],y[1])] for j in range(params['number'])]
    self.cortex = tp.CreateLayer({'center': params['center'], 'extent': params['extent'], 
                                  'positions': pos, 'elements': 'parrot_neuron'})

    # self.randomize(nest.GetNodes(self.cortex)[0], box.rand_param(), 'normal')
    # nest.Connect(self.inhibition, nest.GetNodes(self.cortex[i])[0], syn_spec={'weight': -1000.0, 'delay': 0.1})

    ##########
    # CRITIC #
    ##########
    """
    Create the three motors to generate the control laws
    """
    params = box.critic()
    nest.CopyModel(params['copymodel'], params['model'], params['settings'])

    x = [params['center'][0] - params['extent'][0]/2 + 0.25, params['center'][0] + params['extent'][0]/2 - 0.25]
    y = [params['center'][1] - params['extent'][1]/2 + 0.25, params['center'][1] + params['extent'][1]/2 - 0.25]
    pos = [[np.random.uniform(x[0],x[1]), np.random.uniform(y[0],y[1])] for j in range(params['number'])]
    self.critic = tp.CreateLayer({'center': params['center'], 'extent': params['extent'], 
                                 'positions': pos, 'elements': params['model']})
    self.randomize(nest.GetNodes(self.critic)[0], box.rand_param(), 'normal')

    self.rec_nodes = self.rec_nodes + list(nest.GetNodes(self.critic)[0])

    ##########
    # ACTOR #
    ##########
    """
    Create the three motors to generate the control laws
    """
    params = box.actor()
    nest.CopyModel(params['copymodel'], params['model'], params['settings'])

    x = [params['center'][0] - params['extent'][0]/2 + 0.25, params['center'][0] + params['extent'][0]/2 - 0.25]
    y = [params['center'][1] - params['extent'][1]/2 + 0.25, params['center'][1] + params['extent'][1]/2 - 0.25]
    pos = [[np.random.uniform(x[0],x[1]), np.random.uniform(y[0],y[1])] for j in range(params['number'])]
    self.actor = tp.CreateLayer({'center': params['center'], 'extent': params['extent'], 
                                 'positions': pos, 'elements': params['model']})
    self.randomize(nest.GetNodes(self.actor)[0], box.rand_param(), 'normal')

    self.rec_nodes = self.rec_nodes + list(nest.GetNodes(self.actor)[0])

    ########################
    # REWARD INPUT PROXIES #
    ########################

    params = box.in_rew()

    self.in_rew = tp.CreateLayer({'columns': 1, 'rows': 1, 'center': params['center'],
                                  'elements': params['model']})
    nest.SetStatus(nest.GetNodes(self.in_rew)[0], {'music_channel': params['channel'], 'port_name': params['p_name']})

    ###########
    # SNc/VTA #
    ###########
    """
    Create the dopaminergic neurorns population and the poisson generator to make them firing in rest
    condition at cerating firing rate. The plasitcity law will take into account this baseline.
    The capacitance, initial and reset membrane potential, threshold of the dopaminergic neurons are normally distributed.
    """

    params = box.dopa_neur()
    nest.CopyModel(params['copymodel'], params['model'],
                   {'V_reset': params['V_reset'],
                    'tau_minus': params['tau_ltd']})

    x = [params['center'][0] - params['extent'][0]/2 + 0.25, params['center'][0] + params['extent'][0]/2 - 0.25]
    y = [params['center'][1] - params['extent'][1]/2 + 0.25, params['center'][1] + params['extent'][1]/2 - 0.25]
    pos = [[np.random.uniform(x[0],x[1]), np.random.uniform(y[0],y[1])] for j in range(params['number'])]
    self.dopa = tp.CreateLayer({'center': params['center'], 'extent': params['extent'], 
                                'positions': pos, 'elements': params['model']})
    self.randomize(nest.GetNodes(self.dopa)[0], box.rand_param(), 'uniform')

    self.rec_nodes = self.rec_nodes + list(nest.GetNodes(self.dopa)[0])

    ######################
    # VOLUME TRANSMITTER #
    ######################

    params = box.vt_dopa()

    self.vt_dopa = tp.CreateLayer({'columns': 1, 'rows': 1, 'center': params['center'],
                                   'elements': params['model']})


    #########################
    # CRITIC OUTPUT PROXIES #
    #########################
    """
    Create the outpot proxies to transmit the control to the environment
    """

    params = box.output()

    self.output = tp.CreateLayer({'columns': 1, 'rows': 1, 'center': params['center'],
                                     'elements': params['model']})

    # Set port name for MUSIC Output Proxies
    nest.SetStatus(nest.GetNodes(self.output)[0], {'port_name': params['p_name']})


    self.built = True
    del params
  
  def connect(self, box, *args):
    """
    Connect all nodes in the model.
    music_in -> input_neurons -> output_neurons -> music_out
    """
    if self.connected: return

    if not self.built:
      self.build(box)

    self.sdetector = nest.Create('spike_detector', 1)
    nest.Connect(nest.GetNodes(self.cortex)[0], self.sdetector)

    #================#
    # SYNAPSE MODELS #
    #================#

    params = box.plastic()
    nest.CopyModel(params['copymodel'], params['model'],
                  {'A_minus': params['A_minus'],
                   'A_plus': params['A_plus'],
                   'Wmax': params['w_max'],
                   'Wmin': params['w_min'],
                   'delay': params['delay'],
                   'b': params['baseline'],
                   'tau_c': params['tau_elegibility'],
                   'tau_n': params['tau_dopa'],
                   'tau_plus': params['tau_plus'],
                   'vt': nest.GetNodes(self.vt_dopa)[0][0]})

    ###################################
    # INPUT SENSORS PROXIES -> CORTEX #
    ###################################

    # self.cdict_cortex = {'connection_type'       : 'divergent',
    #                      'weights'               : 10.0,
    #                      'delays'                : 0.2,
    #                      'allow_multapses'       : False}

    # tp.ConnectLayers(self.in_cortex, self.cortex, self.cdict_cortex)

    nest.Connect(nest.GetNodes(self.in_cortex)[0], nest.GetNodes(self.cortex)[0], "one_to_one", 
                 {'weight': 1.0, 'delay': 0.2})

    ####################
    # CORTEX -> CRITIC #
    ####################

    self.cdict_plastic_critic = {'connection_type'       : 'divergent',
                                 'synapse_model'         : 'plastic',
                               # 'number_of_connections' :  50,
                                 'weights'               :  {'uniform': {'min': -50.0, 'max': 100.0}},
                                }

    tp.ConnectLayers(self.cortex, self.critic, self.cdict_plastic_critic)

    if args:
      conn = nest.GetConnections(target = nest.GetNodes(self.critic)[0])
      dictW = self.createDict(args[0])
      nest.SetStatus(conn, dictW)

    ###################
    # CORTEX -> ACTOR #
    ###################

    self.cdict_plastic_actor = {'connection_type'       : 'divergent',
                                'synapse_model'         : 'plastic',
                              # 'number_of_connections' :  50,
                                'weights'               :  {'uniform': {'min': -50.0, 'max': 100.0}},
                               }

    tp.ConnectLayers(self.cortex, self.actor, self.cdict_plastic_actor)

    if args:
      conn = nest.GetConnections(target = nest.GetNodes(self.actor)[0])
      dictW = self.createDict(args[1])
      nest.SetStatus(conn, dictW)

    #################################
    # ACTOR -> LATERAL CONNECTIVITY #
    #################################

    w_max = 30.0
    w_min = -60.0
    actors = nest.GetNodes(self.actor)[0]
    norm = 0.0
    curr_weight = 0.0
    l = 0.5

    for i, n in enumerate(actors):
      for j, k in enumerate(actors):
        if i!=j:
          norm = norm + np.exp(np.power(i-j,2)/np.power(l,2))

    for i, n in enumerate(actors):
      for j, k in enumerate(actors):
        if i!=j:
          curr_weight = w_min/len(actors) + w_max*np.exp(np.power(i-j,2)/np.power(l,2))/norm
          nest.Connect([n], [k], "one_to_one", {'weight': curr_weight})


    ###################################
    # INPUT REWARD PROXIES -> SNc/VTA #
    ###################################

    self.cdict_dopa = {'connection_type'       : 'divergent',
                       'weights'               : 10.0,
                       'delays'                : 0.2,
                       'allow_multapses'       : False}

    tp.ConnectLayers(self.in_rew, self.dopa, self.cdict_dopa)

    #################
    # SNc/VTA -> VT #
    #################

    self.cdict_vt = {'connection_type'       : 'convergent',
                     'delays'                :  0.2,
                     'allow_multapses'       :  False}

    tp.ConnectLayers(self.dopa, self.vt_dopa, self.cdict_vt)


    ##################################
    # POPULATIONS  -> OUTPUT PROXIES #
    ##################################

    for i, n in enumerate(self.rec_nodes):
      nest.Connect([n], nest.GetNodes(self.output)[0], "one_to_one", 
                   {'music_channel': i,
                    'delay': 0.1})


    self.connected = True
    del params

  def run(self, box, simtime):
    """
    Simulate the model for simtime milliseconds. 
    """
    if not self.connected:
      self.connect(box)

    nest.Simulate(simtime)

  def randomize(self, nodes, params, distribution):
    local_nodes = [(ni['global_id'], ni['vp']) for ni in nest.GetStatus(nodes) if ni['local']]
    
    if distribution == 'uniform':
      for gid,vp in local_nodes:
        nest.SetStatus([gid], {'C_m': self.pyrngs[vp].uniform(params['Cm_min'],params['Cm_max']),
                             # 'V_m': self.pyrngs[vp].uniform(params['Vm_min'],params['Vm_max']),
                             # 'V_reset': self.pyrngs[vp].uniform(params['Vreset_min'],params['Vreset_max']),
                               'V_th': self.pyrngs[vp].uniform(params['Vth_min'],params['Vth_max'])})
    elif distribution == 'normal':
      for gid,vp in local_nodes:
        nest.SetStatus([gid], {
                               'C_m': self.pyrngs[vp].normal(params['Cm_mean'],params['Cm_std']),
                               'V_m': self.pyrngs[vp].normal(params['Vm_mean'],params['Vm_std']),
                               'V_reset': self.pyrngs[vp].normal(params['Vreset_mean'],params['Vreset_std']),
                               'V_th': self.pyrngs[vp].normal(params['Vth_mean'],params['Vth_std'])
                              })

  def createDict(self, vect):
    dictionary = []
    for np in vect:
      dictionary.append({'weight': np})

    return dictionary