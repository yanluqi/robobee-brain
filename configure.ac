# configure.ac
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

AC_PREREQ([2.59])
AC_INIT([RoboBee], [1.0.2])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE

AM_CONDITIONAL([DARWIN], [test $(uname) = Darwin])

AM_PROG_AR

AC_LANG([C++])
AC_PROG_CXX(mpicxx)

AC_CHECK_LIB([music], [_init])
AC_CHECK_HEADER([music.hh])

AC_CHECK_LIB([armadillo], [_init])
AC_CHECK_HEADER([armadillo])

AC_CONFIG_MACRO_DIR([m4])
# Check Boost C++ libraries
AX_BOOST_BASE([1.53],,[AC_MSG_ERROR([projectX needs Boost, but it was not found in your system])])
# Calls AC_SUBST(BOOST_ * _LIB)
AX_BOOST_IOSTREAMS
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
