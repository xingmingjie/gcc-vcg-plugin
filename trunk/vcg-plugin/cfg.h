/* Control flow graph

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

#ifndef CFG_H
#define CFG_H

enum vcg_plugin_edge_type
{
  VCG_PLUGIN_UNKNOWN_EDGE,
  VCG_PLUGIN_DFST_EDGE,
  VCG_PLUGIN_ADVANCING_EDGE,
  VCG_PLUGIN_RETREATING_EDGE,
  VCG_PLUGIN_CROSS_EDGE
};

struct vcg_plugin_edge
{
  enum vcg_plugin_edge_type type;

  struct vcg_plugin_basic_block *source;
  struct vcg_plugin_basic_block *target;

  struct vcg_plugin_edge *next;
};

struct vcg_plugin_vec_edge
{
  struct vcg_plugin_edge *edge;
  struct vcg_plugin_vec_edge *next;
};

struct vcg_plugin_basic_block
{
  char *name;
  char *text;

  int pred_num;
  int succ_num;
  int visited;
  int dfs_order;
  int max_distance;

  struct vcg_plugin_vec_edge *pred;
  struct vcg_plugin_vec_edge *last_pred;
  struct vcg_plugin_vec_edge *succ;
  struct vcg_plugin_vec_edge *last_succ;

  struct vcg_plugin_basic_block *next;
};

struct vcg_plugin_control_flow_graph
{
  int bb_num;
  int edge_num;
  struct vcg_plugin_basic_block *entry; /* also the first bb */
  struct vcg_plugin_basic_block *exit; /* also  the last bb */
  struct vcg_plugin_basic_block *bb;
  struct vcg_plugin_basic_block *last_bb;
  struct vcg_plugin_edge *edge;
  struct vcg_plugin_edge *last_edge;
};

struct vcg_plugin_function
{
  char *name;
  struct vcg_plugin_control_flow_graph *cfg;
  struct vcg_plugin_function *next;
};

extern struct vcg_plugin_function *vcg_plugin_new_function (char *name);
extern struct vcg_plugin_basic_block *vcg_plugin_lookup_and_add_bb (struct vcg_plugin_function *func,
                                                      char *name);
extern struct vcg_plugin_edge *vcg_plugin_lookup_and_add_edge (struct vcg_plugin_function *func,
                                                 char *source_name,
                                                 char *target_name);

#endif
