/* <one line to give the program's name and a bief idea of what it does.>

   Copyright (C) 2010 Mingjie Xing, mingjie.xing@gmail.com.

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

#include <config.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "toplev.h"
#include "gimple.h"
#include "tree-pass.h"
#include "rtl.h"
#include "intl.h"
#include "langhooks.h"
#include "cfghooks.h"
#include "version.h" /* for printing gcc version number in graph */

#include "vcg-plugin.h"
#include "tree.h"

/* Create a graph from the function fn. */

static gdl_graph *
create_tree_hierarchy_graph (void)
{
  gdl_graph *graph;
  gdl_node *node;
  gdl_edge *edge;

  graph = gdl_new_graph ("tree hierarchy");
  gdl_set_graph_node_borderwidth (graph, 1);
  gdl_set_graph_node_margin (graph, 1);
  gdl_set_graph_edge_thickness (graph, 1);
  gdl_set_graph_splines (graph, "yes");
  gdl_set_graph_port_sharing (graph, 0);

  #define NEW_NODE(name) \
  node = gdl_new_node (name); \
  gdl_set_node_label (node, name); \
  gdl_add_node (graph, node);

  NEW_NODE ("tree_base")
  NEW_NODE ("tree_common")

  #define NEW_EDGE(src, dest) \
  edge = gdl_new_edge (src, dest); \
  gdl_add_edge (graph, edge);

  NEW_EDGE ("tree_base", "tree_common")

  return graph;
}

static void
dump_tree_hierarchy_to_file (char *fname)
{
  FILE *fp;
  gdl_graph *graph;

  if ((fp = fopen (fname, "w")) == NULL)
    {
      vcg_plugin_common.error ("failed to open file %s.", fname);
      return;
    }

  graph = create_tree_hierarchy_graph ();
  gdl_dump_graph (fp, graph);
  gdl_free_graph (graph);

  fclose (fp);
}

/* Public function to dump the gcc tree structure hierarchy.  */

void
vcg_plugin_dump_tree_hierarchy (void)
{
  char *fname;

  /* Get the temp file name.  */
  fname = vcg_plugin_common.temp_file_name;
  dump_tree_hierarchy_to_file (fname);
  vcg_plugin_common.show (fname);
}

/* Public function to view the gcc tree structure hierarchy.  */

void
vcg_plugin_view_tree_hierarchy (void)
{
  char *fname;

  /* Get the temp file name.  */
  fname = vcg_plugin_common.temp_file_name;
  dump_tree_hierarchy_to_file (fname);
  vcg_plugin_common.show (fname);
}

