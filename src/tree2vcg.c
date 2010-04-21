/* Output vcg description file according to the tree dump file.

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

#include <stdio.h>
#include <stdlib.h>

#include <libiberty.h>
#include <obstack.h>

#include "tree2vcg.h"

extern FILE *yyin;
extern int yyparse (void);
extern void vcg_plugin_set_yy_debug (void);

/* char *program_name; */

#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free

/*
 Define global variables:
  vcg_plugin_insn_obstack
  vcg_plugin_first_function
  vcg_plugin_last_function
  vcg_plugin_current_function

 Use global variables: 
  vcg_plugin_insn_obstack
  vcg_plugin_first_function
  vcg_plugin_last_function
  vcg_plugin_current_function

 Define extern functions:
  vcg_plugin_tree2vcg

 Use extern functions:
  vcg_plugin_set_yy_debug
  yyparse
  vcg_plugin_finalize_last_bb
  vcg_plugin_fine_tune_cfg
  vcg_plugin_cfg_to_vcg
  vcg_plugin_output_vcg
*/

struct obstack vcg_plugin_insn_obstack;

struct vcg_plugin_function *vcg_plugin_first_function; 
struct vcg_plugin_function *vcg_plugin_last_function; 
struct vcg_plugin_function *vcg_plugin_current_function; 

static void
general_init (void)
{
  obstack_init (&vcg_plugin_insn_obstack);

  vcg_plugin_first_function = NULL;
  vcg_plugin_last_function = NULL;
  vcg_plugin_current_function = NULL;
}

int
vcg_plugin_tree2vcg (FILE *dump_file, FILE *vcg_file)
{
  general_init ();

  yyin = dump_file;
  /* vcg_plugin_set_yy_debug (); */
  yyparse ();
  vcg_plugin_finalize_last_bb ();

  vcg_plugin_fine_tune_cfg ();

  vcg_plugin_cfg_to_vcg ();

  vcg_plugin_output_vcg (vcg_file);

  return 0;
}
