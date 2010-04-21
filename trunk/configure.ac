#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Copyright (C) 2009, 2010 Eric Fisher, joefoxreal@gmail.com.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ(2.59)
AC_INIT(gcc-vcg-plugin, 0.4, joefoxreal@gmail.com)
AC_CONFIG_HEADERS(config.h)

# m4
AC_CONFIG_MACRO_DIR([m4])
#ACLOCAL_FLAGS="-I m4"

# automake
AM_INIT_AUTOMAKE

# add the m4 dir
# AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_YACC
AC_PROG_CC
AC_PROG_LEX

# libtool
LT_INIT
AC_PROG_LIBTOOL

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_FUNC_OBSTACK

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
