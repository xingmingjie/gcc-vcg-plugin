/* Dump or view gcc loop.
 
   Copyright (C) 2011 Mingjie Xing, mingjie.xing@gmail.com.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "vcg-plugin.h"

/* Should be enough.  */
static char buf[32]; 

#define LOOP_GRAPH_TITLE(index) {sprintf (buf, "loop %d", index); buf}
#define BB_GRAPH_TITLE(index) {sprintf (buf, "bb %d", index); buf}
#define BB_NODE_TITLE(index) {sprintf (buf, "bb.%d", index); buf}

/* Temp file stream, used to get the bb dump from gcc dump function.  */
FILE *tmp_stream;
char *tmp_buf;
size_t tmp_buf_size;

int
set_vertical_order_1 (gdl_graph *graph, int *distance, basic_block bb)
{
  int val, max = 0;
  edge e;
  edge_iterator ei;
  gdl_graph *subgraph;
  gdl_node *node;

  if (distance[bb->index] != 0)
    return distance[bb->index];

  FOR_EACH_EDGE (e, ei, bb->preds)
    {
      if (e->flags & EDGE_DFS_BACK)
        continue;

      val = set_vertical_order_1 (graph, distance, e->src);
      max = max > val ? max : val;
    }
  distance[bb->index] = max + 1;
  subgraph = gdl_find_subgraph (graph, bb_graph_title[bb->index]); 
  gdl_set_graph_vertical_order (subgraph, distance[bb->index]);
  node = gdl_get_graph_node (subgraph);
  gdl_set_node_vertical_order (node, distance[bb->index]);
 
  return distance[bb->index];
}

static void
set_vertical_order (gdl_graph *graph)
{
  int val, max = 0;
  int *distance;
  basic_block bb;
  gdl_graph *subgraph;
  gdl_node *node;

  calculate_dominance_info (CDI_DOMINATORS);
  mark_dfs_back_edges ();

  distance = (int *) xcalloc (n_basic_blocks, sizeof (int));

  max = set_vertical_order_1 (graph, distance, EXIT_BLOCK_PTR);
  FOR_EACH_BB (bb)
    {
      if (distance[bb->index] == 0)
        {
          val = set_vertical_order_1 (graph, distance, bb);
          max = max > val ? max : val + 1;
        }
    }
  subgraph = gdl_find_subgraph (graph,
                                bb_graph_title[EXIT_BLOCK_PTR->index]); 
  gdl_set_graph_vertical_order (subgraph, max);
  node = gdl_get_graph_node (subgraph);
  gdl_set_node_vertical_order (node, max);

  free (distance);
}

static void
add_bb_edge (gdl_graph *graph, struct loop *loop)
{
  gdl_edge *e;
  basic_block bb;

  
  FOR_EACH_BB (bb)
    {
      if (gdl_find_node (graph, BB_NODE_TITLE (bb->index)))
    }
}

/* Create a graph for BB and add it into GRAPH.  */

static void
add_bb_graph (gdl_graph *graph, basic_block bb)
{
  gdl_graph *g;
  gdl_node *n;
  char *str;
  int i;

  /* Create a graph.  */
  g = gdl_new_graph (BB_GRAPH_TITLE(bb->index));
  gdl_set_graph_label (g, gdl_get_graph_title (g));
  gdl_set_graph_folding (g, 1);
  gdl_set_graph_shape (g, "ellipse");
  gdl_add_graph (graph, g);

  /* Get the content of bb through gcc's dump function.  */
  rewind (tmp_stream);
  gimple_dump_bb (bb, tmp_stream, 0, TDF_VOPS|TDF_MEMSYMS|TDF_BLOCKS);
  i = tmp_buf_size;
  while (i > 1 && ISSPACE (tmp_buf[i - 1])) i--;
  str = xstrndup (tmp_buf, i);
  vcg_plugin_common.tag (str);

  /* Create a node.  */
  n = gdl_new_node (BB_NODE_TITLE(bb->index));
  gdl_set_node_label (n, str);
  gdl_add_node (g, n);
}

static void
add_loop_and_siblings_graph (gdl_graph *graph, struct loop *loop) 
{
  add_loop_graph (graph, loop);
  add_loop_and_siblings_graph (graph, loop->next);
}

/* Create a graph for LOOP and add it into GRAPH.  */

static void
add_loop_graph (gdl_graph *graph, struct loop *loop)
{
  basic_block bb;
  edge e;
  edge_iterator ei;

  gdl_graph *g;
  gdl_edge *v_edge;

  g = gdl_new_graph (LOOP_GRAPH_TITLE(loop->num));
  gdl_add_graph (graph, g)

  FOR_EACH_BB (bb)
    if (bb->loop_father == loop)
      {
        add_bb_graph (graph, bb);
      }

  add_loop_and_siblings_graph (graph, loop->inner); 
}

/* Create the top graph.  */

static gdl_graph *
create_top_graph (void)
{
  gdl_graph *graph;

  graph = gdl_new_graph ("top");
  gdl_set_graph_node_borderwidth (graph, 1);
  gdl_set_graph_node_margin (graph, 1);
  gdl_set_graph_edge_thickness (graph, 1);
  gdl_set_graph_splines (graph, "yes");

  return graph;
}

/* Dump LOOP into the file which name is FNAME.  */

static void
dump_loop (char *fname, struct loop *loop)
{
  FILE *fp;
  gdl_graph *graph;

  if ((fp = fopen (fname, "w")) == NULL)
    {
      vcg_plugin_common.error ("failed to open file %s.", fname);
      return;
    }

  /* Create names for graphs and nodes.  */
  create_names (n_basic_blocks);
  tmp_stream = open_memstream (&tmp_buf, &tmp_buf_size);

  graph = create_top_graph ();
  add_loop_graph (graph, loop);

  /* Optimize the graph layout.  */
  set_vertical_order (graph);

  gdl_dump_graph (fp, graph);
  gdl_free_graph (graph);

  /* Free names for graphs and nodes.  */
  free_names (n_basic_blocks);
  fclose (fp);
  fclose (tmp_stream);
  free (tmp_buf);
}

/* Public function to dump a gcc LOOP.  */

void
vcg_plugin_dump_function (struct loop *loop)
{
  char *fname;

  vcg_plugin_common.init ();
  fname = "dump-loop.vcg";
  dump_loop (fname, loop);
  vcg_plugin_common.finish ();
}

/* Public function to view a gcc LOOP.  */

void
vcg_plugin_view_loop (struct loop *loop)
{
  char *fname;

  vcg_plugin_common.init ();
  fname = vcg_plugin_common.temp_file_name;
  dump_loop (fname, loop);
  vcg_plugin_common.show (fname);
  vcg_plugin_common.finish ();
}

