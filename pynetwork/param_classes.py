# -*- coding: utf-8 -*-
#
# param_classes.py
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

import numpy as np

class ParametersBox(object):
	# Number of place cells per state (see n_pCells comment)
	n_CellsState = [7, 7]

	# Number of critic neurons
	n_Critic = 50
	
	# Number of actor neurons
	n_Actor = 60
	
	# Number of dopaminergic neurons
	n_Dopa = 100

	# Get the number of output channels in config file equal to [n_Critic + n_Actor + n_Dopa]

	# Bool variable for calibrate method
	calibrated = False

	def __init__(self):
		"""
    	Initialize an object of this class.
    	"""
		self.params = {}

	def calibrate(self):
		self.n_pCells =  np.prod(self.n_CellsState)
		"""
    	Compute all parameter dependent variables of the
    	model.
    	"""

    	# The number of input channels in the config file must be equal to this n_pCells + 1


											####################
											# NODES PARAMETERS #
											####################

	#==================#
	# RANDOM VARIABLES #
	#==================#
	def rand_param(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['rand'] = {}

		# Uniform Distribution
		self.params['rand']['Vm_min'] = -85.
		self.params['rand']['Vm_max'] = 50.

		self.params['rand']['Cm_min'] = 50.
		self.params['rand']['Cm_max'] = 100.

		self.params['rand']['Vth_min'] = -65.
		self.params['rand']['Vth_max'] = -55.

		self.params['rand']['Vreset_min'] = -70.
		self.params['rand']['Vreset_max'] = -80.

		# Guassian Distribution
		self.params['rand']['Vm_mean'] = -85.
		self.params['rand']['Vm_std'] = 1.

		self.params['rand']['Cm_mean'] = 100
		self.params['rand']['Cm_std'] = 5.

		self.params['rand']['Vth_mean'] = -45.
		self.params['rand']['Vth_std'] = 1.

		self.params['rand']['Vreset_mean'] = -80.
		self.params['rand']['Vreset_std'] = 1.

		return self.params['rand']

	#===========================#
	# PLACE CELLS INPUT PROXIES #
	#===========================#
	def input_sensor(self):
		
		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['input_sensor'] = {}
		self.params['input_sensor']['model'] = 'music_event_in_proxy'
		self.params['input_sensor']['center'] = [2.5, 5.0]
		self.params['input_sensor']['extent'] = [3.0, 3.0]
		self.params['input_sensor']['p_name'] = 'p_in'
		self.params['input_sensor']['latency'] = 0.2

		return self.params['input_sensor']

	#=============#
	# PLACE CELLS #
	#=============#
	def cortex(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['cortex'] = {}

		# MODEL
		self.params['cortex']['copymodel'] = 'parrot_neuron'
		self.params['cortex']['model'] = 'cortex_neuron'

		if self.params['cortex']['copymodel'] == 'iaf_neuron':
			self.params['cortex']['settings'] = {'C_m'      : 250.0,  # membrane capacitance (pF)
 									   			 'E_L'      : -70.0,  # resting membrane potential (mV)
 									   			 'I_e'      :   0.0,
 									   			 'V_m'      : -70.0,
 									   			 'V_reset'  : -70.0,  # reset potential (mV)
 									   			 'V_th'     : -55.0,  # spike threshold (mV)
 									   			 't_ref'    :   0.5,  # absolute refractory period (ms)
 									   			 'tau_m'    :  10.0,  # membrane time constant (ms)
 									   			 'tau_minus':  10.0,
 									   			 'tau_syn'  :   2.0
 									   			}

 		elif self.params['cortex']['copymodel'] == 'parrot_neuron':
 			self.params['cortex']['settings'] = {'beta_Ca': 0.001}

 		elif self.params['cortex']['copymodel'] == 'iaf_cond_alpha':
 			self.params['cortex']['settings'] = {'C_m'       :   250.0,  # membrane capacitance (pF)
 									   			 'E_L'       :   -70.0,  # resting membrane potential (mV)
 									   			 'I_e'       :     0.0,
 									   			 'V_m'       :   -70.0,
 									   			 'V_reset'   :   -70.0,  # reset potential (mV)
 									   			 'V_th'      :   -55.0,  # spike threshold (mV)
 									   			 't_ref'     :     0.5,  # absolute refractory period (ms)
 									   			 'g_L'       : 16.6667,
 									   			 'tau_minus' :    10.0,
  									   			 'E_ex'      :     0.0,   
  									   			 'E_in'      :   -85.0,
        	                     	   			 'tau_syn_ex':     0.2,  # excitatory synaptic time constant (ms)
        	                     	             'tau_syn_in':     2.0  # inhibitory synaptic time constant (ms)
        	                     	  			}

		# LAYER
		self.params['cortex']['number'] = self.n_pCells
		self.params['cortex']['center'] = [2.5, 0.0]
		self.params['cortex']['extent'] = [3.0, 3.0]

		return self.params['cortex']


	#========#
	# CRITIC #
	#========#
	def critic(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['critic'] = {}

		# MODEL
		self.params['critic']['copymodel'] = 'iaf_neuron'
		self.params['critic']['model'] = 'critic_neuron'
		
		if self.params['critic']['copymodel'] == 'iaf_neuron':
			self.params['critic']['settings'] = {'C_m'      : 250.0, # (250.0) membrane capacitance (pF)
 									   			'E_L'      : -70.0, # (-70.0) resting membrane potential (mV)
 									   			'I_e'      :   0.0, # (  0.0)
 									   			'V_m'      : -70.0, # (-70.0)
 									   			'V_reset'  : -70.0, # (-70.0) reset potential (mV)
 									   			'V_th'     : -55.0, # (-55.0) spike threshold (mV)
 									   			't_ref'    :   0.5, # (  2.0) absolute refractory period (ms)
 									   			'tau_m'    :  10.0, # ( 10.0) membrane time constant (ms)
 									   			'tau_minus':  10.0, # ( 20.0)
 									   			'tau_syn'  :   2.0  # (  2.0)
 									   			}
 		elif self.params['critic']['copymodel'] == 'iaf_cond_alpha':
 			self.params['critic']['settings'] = {'C_m'       :   250.0, # (  250.0) membrane capacitance (pF)
 									   			'E_L'       :   -70.0, # (  -70.0) resting membrane potential (mV)
 									   			'I_e'       :     0.0, # (    0.0)
 									   			'V_m'       :   -70.0, # (  -70.0)
 									   			'V_reset'   :   -70.0, # (  -60.0) reset potential (mV)
 									   			'V_th'      :   -55.0, # (  -55.0) spike threshold (mV)
 									   			't_ref'     :     0.5, # (    2.0) absolute refractory period (ms)
 									   			'g_L'       : 16.6667, # (16.6667)
 									   			'tau_minus' :    10.0, # (   20.0)
  									   			'E_ex'      :     0.0, # (    0.0)   
  									   			'E_in'      :   -85.0, # (  -85.0)
        	                     	   			'tau_syn_ex':     0.2, # (    0.2) excitatory synaptic time constant (ms)
        	                     	            'tau_syn_in':     2.0  # (    2.0) inhibitory synaptic time constant (ms)
        	                     	  			}

		# LAYER
		self.params['critic']['number'] = self.n_Critic
		self.params['critic']['center'] = [0.0, -5.0]
		self.params['critic']['extent'] = [4.0, 2.0]
		
		return self.params['critic']

	#=======#
	# ACTOR #
	#=======#
	def actor(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['actor'] = {}

		# MODEL
		self.params['actor']['copymodel'] = 'iaf_neuron'
		self.params['actor']['model'] = 'actor_neuron'
		
		if self.params['actor']['copymodel'] == 'iaf_neuron':
			self.params['actor']['settings'] = {'C_m'      : 250.0, # (250.0) membrane capacitance (pF)
 									   			'E_L'      : -70.0, # (-70.0) resting membrane potential (mV)
 									   			'I_e'      :   0.0, # (  0.0)
 									   			'V_m'      : -70.0, # (-70.0)
 									   			'V_reset'  : -70.0, # (-70.0) reset potential (mV)
 									   			'V_th'     : -55.0, # (-55.0) spike threshold (mV)
 									   			't_ref'    :   0.5, # (  2.0) absolute refractory period (ms)
 									   			'tau_m'    :  10.0, # ( 10.0) membrane time constant (ms)
 									   			'tau_minus':  40.0, # ( 20.0)
 									   			'tau_syn'  :   2.0  # (  2.0)
 									   			}
 		elif self.params['actor']['copymodel'] == 'iaf_cond_alpha':
 			self.params['actor']['settings'] = {'C_m'       :   250.0, # (  250.0) membrane capacitance (pF)
 									   			'E_L'       :   -70.0, # (  -70.0) resting membrane potential (mV)
 									   			'I_e'       :     0.0, # (    0.0)
 									   			'V_m'       :   -70.0, # (  -70.0)
 									   			'V_reset'   :   -70.0, # (  -60.0) reset potential (mV)
 									   			'V_th'      :   -55.0, # (  -55.0) spike threshold (mV)
 									   			't_ref'     :     0.5, # (    2.0) absolute refractory period (ms)
 									   			'g_L'       : 16.6667, # (16.6667)
 									   			'tau_minus' :    40.0, # (   20.0)
  									   			'E_ex'      :     0.0, # (    0.0)   
  									   			'E_in'      :   -85.0, # (  -85.0)
        	                     	   			'tau_syn_ex':     0.2, # (    0.2) excitatory synaptic time constant (ms)
        	                     	            'tau_syn_in':     2.0  # (    2.0) inhibitory synaptic time constant (ms)
        	                     	  			}

		# LAYER
		self.params['actor']['number'] = self.n_Actor
		self.params['actor']['center'] = [5.0, -5.0]
		self.params['actor']['extent'] = [4.0, 2.0]
		
		return self.params['actor']

	#======================#
	# REWARD INPUT PROXIES #
	#======================#
	def in_rew(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['in_rew'] = {}

		self.params['in_rew']['center'] = [-12.5, 0.0]
		self.params['in_rew']['channel'] = self.n_pCells
		self.params['in_rew']['p_name'] = 'p_in'
		self.params['in_rew']['model'] = 'music_event_in_proxy'

		return self.params['in_rew']

	#======================#
	# DOPAMINERGIG NEURONS #
	#======================#
	def dopa_neur(self):

		if not self.calibrated:
			self.calibrate()
			calibrated = True

		self.params['dopa_neur'] = {}

		# MODEL
		self.params['dopa_neur']['copymodel'] = 'iaf_cond_alpha'
		self.params['dopa_neur']['model'] = 'dopa_neuron'
		self.params['dopa_neur']['refrac'] = 2.0
		self.params['dopa_neur']['tau_ltd'] = 20.0
		self.params['dopa_neur']['V_reset'] = -70.0

		# LAYER
		self.params['dopa_neur']['number'] = self.n_Dopa
		self.params['dopa_neur']['center'] = [-9.5, 0.0]
		self.params['dopa_neur']['extent'] = [3.0, 2.0]

		return self.params['dopa_neur']

	#=====================#
	# VOLUME TRANSMITTERS #
	#=====================#
	def vt_dopa(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['vt_dopa'] = {}

		self.params['vt_dopa']['center'] = [-6.5, 0.0]
		self.params['vt_dopa']['model'] = 'volume_transmitter'

		return self.params['vt_dopa']

	#================#
	# OUTPUT PROXIES #
	#================#
	def output(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['output'] = {}
		self.params['output']['center'] = [11.0, 0.0]
		self.params['output']['p_name'] = 'p_out'
		self.params['output']['model'] = 'music_event_out_proxy'

		return self.params['output']


											##########################
											# CONNECTIONS PARAMETERS #
											##########################

	def plastic(self):

		if not self.calibrated:
			self.calibrate()
			self.calibrated = True

		self.params['plastic'] = {}

		# Synapse property
		self.params['plastic']['copymodel'] = 'stdp_dopamine_synapse'
		self.params['plastic']['model'] = 'plastic'
		self.params['plastic']['A_minus'] = 0.375
		self.params['plastic']['A_plus'] = 0.75
		self.params['plastic']['w_max'] = 100.
		self.params['plastic']['w_min'] = -50.
		self.params['plastic']['baseline'] = 0.0
		self.params['plastic']['tau_elegibility'] = 500.
		self.params['plastic']['tau_dopa'] = 100.
		self.params['plastic']['tau_plus'] = 20.
		self.params['plastic']['delay'] = 0.1

		return self.params['plastic']