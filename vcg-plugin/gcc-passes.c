/* Dump or view gcc passes.

   Copyright (C) 2010, 2011 Mingjie Xing, mingjie.xing@gmail.com.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */ 

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "vcg-plugin.h"

static int id;

static gdl_graph *
create_passes_graph_1 (gdl_graph *graph, struct opt_pass *passes, char *name)
{
  gdl_graph *subgraph, *g;
  gdl_node *node;
  gdl_edge *edge;
  char *title, *prev_title = NULL;
  struct opt_pass *pass;

  asprintf (&title, "%d", id++);
  subgraph = gdl_new_graph (title);
  gdl_set_graph_label (subgraph, name);
  gdl_add_subgraph (graph, subgraph);

  for (pass = passes; pass; pass = pass->next)
    {
      if (pass->sub)
        {
          g = create_passes_graph_1 (subgraph, pass->sub, pass->name);
          title = gdl_get_graph_title (g);
        }
      else
        {
          asprintf (&title, "%d", id++);
          node = gdl_new_node (title);
          gdl_set_node_label (node, pass->name);
          gdl_add_node (subgraph, node);
        }
      if (prev_title)
        {
          edge = gdl_new_edge (prev_title, title);
          gdl_add_edge (subgraph, edge);
        }
      prev_title = title;
    }
  return subgraph;
}

/* Create the passes graph.  */

static gdl_graph *
create_passes_graph (struct opt_pass *passes)
{
  gdl_graph *graph;
  gdl_graph *subgraph;

  graph = gdl_new_graph ("passes");
  gdl_set_graph_node_borderwidth (graph, 1);
  gdl_set_graph_edge_thickness (graph, 1);

  create_passes_graph_1 (graph, passes, "passes");

  return graph;
}

static void
dump_passes_to_file (char *fname, struct opt_pass *passes)
{
  FILE *fp;
  gdl_graph *graph;

  if ((fp = fopen (fname, "w")) == NULL)
    {
      vcg_plugin_common.error ("failed to open file %s.", fname);
      return;
    }

  id = 0;

  graph = create_passes_graph (passes);
  gdl_dump_graph (fp, graph);
  gdl_free_graph (graph);

  fclose (fp);
}

/* Public function to dump the gcc passes.  */

void
vcg_plugin_dump_passes (struct opt_pass *passes)
{
  char *fname = "dump-passes.vcg";

  dump_passes_to_file (fname, passes);
}

/* Public function to view the gcc passes.  */

void
vcg_plugin_view_passes (struct opt_pass *passes)
{
  char *fname;

  /* Get the temp file name.  */
  fname = vcg_plugin_common.temp_file_name;
  dump_passes_to_file (fname, passes);
  vcg_plugin_common.show (fname);
}

