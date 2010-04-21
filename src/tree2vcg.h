/* Header file of tree2vcg.

   Copyright (C) 2009, 2010 Eric Fisher, joefoxreal@gmail.com. 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef TREE2VCG_H
#define TREE2VCG_H

#include "gdl.h"
#include "cfg.h"

extern struct obstack vcg_plugin_insn_obstack;

extern struct vcg_plugin_function *vcg_plugin_first_function; 
extern struct vcg_plugin_function *vcg_plugin_last_function;
extern struct vcg_plugin_function *vcg_plugin_current_function;

extern struct gdl_graph *vcg_plugin_top_graph;

/* yacc.y */
extern void vcg_plugin_finalize_last_bb (void);

/* output.c */
extern void vcg_plugin_output_vcg (FILE *fout);

/* fine_tune.c */
extern void vcg_plugin_fine_tune_vcg (void);
#endif
