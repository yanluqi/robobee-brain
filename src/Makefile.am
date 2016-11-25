# Makefile.am
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

ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = bootstrap

bin_PROGRAMS = main

main_SOURCES = main.cpp \
			receiver.cpp \
			iomanager.cpp \
			decoder.cpp \
			robobee.cpp \
			controller.cpp

if DARWIN
OPENGL = -framework OPENGL
else
OPENGL = -lGL
endif

GRAPHIC_FLAGS = -lGLEW $(GLFW_LIBS) -lfreetype

main_LDADD = ../graphic/liboglhelper.a \
			 -larmadillo \
			 -lmusic \
			 $(GRAPHIC_FLAGS) \
   			 $(BOOST_IOSTREAMS_LIB) \
   			 $(BOOST_SYSTEM_LIB) \
   			 $(BOOST_FILESYSTEM_LIB)

main_LDFLAGS = $(OPENGL)