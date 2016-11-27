#!/usr/local/bin/python

# -*- coding: utf-8 -*-
#
# tools.py
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

from __future__ import division
import nest
import nest.topology as tp
import numpy as np
from scipy.interpolate import interp1d

import matplotlib
import matplotlib.patches as patches
import matplotlib.pyplot as plt
import mpl_toolkits
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter

import matplotlib.gridspec as gridspec
from mpl_toolkits.axes_grid1 import make_axes_locatable

def plotNet(net):
  plt.ion()
  plt.clf()
  fig = plt.gcf()
  ax = fig.add_subplot(111)

  # Plot Layers
  tp.PlotLayer(net.in_cortex, nodesize=50, fig=fig)
  tp.PlotLayer(net.cortex, nodesize=30, fig=fig)

  tp.PlotLayer(net.critic, nodesize=30, fig=fig)
  tp.PlotLayer(net.actor, nodesize=30, fig=fig)

  tp.PlotLayer(net.in_rew, nodesize=50, fig=fig)
  tp.PlotLayer(net.dopa, nodesize=30, fig=fig)
  tp.PlotLayer(net.vt_dopa, nodesize=50, fig=fig)

  tp.PlotLayer(net.output, nodesize=50, fig=fig)
  

  plt.axis([-15.0, 15.0, -9.0, 9.0])
  plt.axes().set_aspect('equal', 'box')
  plt.axes().set_xticks(np.arange(-15.0, 15.1, 1.0))
  plt.axes().set_yticks(np.arange(-9.0, 9.1, 1.0))
  plt.grid(True)
  plt.xlabel('Workspace Columns: 25')
  plt.ylabel('Workspace Rows: 30')
  plt.title('RoboBee Brain')

def weightMatrix(conn):
  min_first = min(conn)[0]
  max_first = max(conn)[0]
  
  min_second = min(conn)[1]
  max_second = max(conn)[1]
  
  
  n_first = max_first - min_first + 1;
  n_second = max_second - min_second + 1;
    
  W = np.zeros([n_first, n_second])
  
  w_conn = nest.GetStatus(conn, keys='weight')
  
  for idx,n in enumerate(conn):
    W[n[0]-min_first, n[1]-min_second] += w_conn[idx]

  return W

def plot3Dweights(W):
  fig = plt.figure()
  ax = fig.gca(projection='3d')
  X = np.arange(0, len(W), 1)
  Y = np.arange(0, len(W[0]), 1)
  X, Y = np.meshgrid(Y, X)
  
  surf = ax.plot_surface(X, Y, W, rstride=1, cstride=1, cmap=cm.jet,
                         linewidth=0, antialiased=False)
  
  ax.zaxis.set_major_locator(LinearLocator(10))
  ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
  
  fig.colorbar(surf, shrink=0.5, aspect=5)
  
  # plt.show()

def plot2Dweights(W):
  plt.figure()
  plt.pcolor(W,cmap=cm.jet)
  plt.colorbar()

  plt.show()