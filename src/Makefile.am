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

bin_PROGRAMS = main analyze

main_SOURCES = \
	main.cpp \
	sender.cpp \
	receiver.cpp \
	encoder.cpp \
	decoder.cpp \
	robobee.cpp \
	controller.cpp \
	iomanager.cpp \
	plotter.cpp

main_LDADD = \
	-ldynplot \
	-larmadillo \
	-lmusic \
  $(BOOST_IOSTREAMS_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_FILESYSTEM_LIB)

analyze_SOURCES = \
	analyze.cpp \
	plotter.cpp

analyze_LDADD = \
	-ldynplot \
	-larmadillo \
	$(BOOST_IOSTREAMS_LIB) \
  $(BOOST_SYSTEM_LIB) \
  $(BOOST_FILESYSTEM_LIB)
