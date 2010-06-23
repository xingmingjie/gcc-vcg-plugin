/* Graph description languange.

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
#include <assert.h>

#include <error.h>
#include <libiberty.h>

#include "gdl.h"
#include "cfg.h"
#include "tree2vcg.h"

/*
 Define global variables:
  gdl_shape_s
  gdl_color_s
  gdl_linestyle_s
  gdl_layout_algorithm_s
  vcg_plugin_top_graph

 Use global variable:
  vcg_plugin_top_graph
  vcg_plugin_current_function 
  vcg_plugin_first_function

 Define extern functions:
  vcg_plugin_cfg_to_vcg

 Use extern functions:
  <none>
*/

char *gdl_shape_s[GDL_SHAPE_DEFAULT + 1] =
{
  "box",
  "rhomb",
  "ellipse",
  "triangle",
  NULL
};

char *gdl_color_s[GDL_COLOR_DEFAULT + 1] =
{
  "black",
  "blue",
  "lightblue",
  "red",
  "green",
  "yellow",
  "white",
  "lightgrey",
  NULL
};

char *gdl_linestyle_s[GDL_LINESTYLE_DEFAULT + 1] =
{
  "continuous",
  "dashed",
  "dotted",
  "invisible",
  NULL
};

char *gdl_layout_algorithm_s[GDL_LAYOUT_ALGORITHM_DEFAULT + 1] =
{
  "max_depth",
  "tree",
  NULL
};

struct gdl_graph *vcg_plugin_top_graph;

static struct gdl_graph *
gdl_new_graph (char *title)
{
  struct gdl_graph *graph;

  graph = (struct gdl_graph *) xmalloc (sizeof (struct gdl_graph));
  gdl_set_graph_title (graph, title);
  gdl_set_graph_label (graph, NULL);
  gdl_set_graph_color (graph, GDL_COLOR_DEFAULT);
  gdl_set_graph_node_color (graph, GDL_COLOR_DEFAULT);
  gdl_set_graph_folding (graph, -1);
  gdl_set_graph_shape (graph, GDL_SHAPE_DEFAULT);
  gdl_set_graph_splines (graph, NULL);
  gdl_set_graph_layout_algorithm (graph, GDL_LAYOUT_ALGORITHM_DEFAULT);
  gdl_set_graph_vertical_order (graph, -1);
  gdl_set_graph_near_edges (graph, -1);
  gdl_set_graph_port_sharing (graph, -1);
  gdl_set_graph_node_borderwidth (graph, -1);
  gdl_set_graph_node_margin (graph, -1);
  gdl_set_graph_edge_thickness (graph, -1);

  graph->node = NULL;
  graph->last_node = NULL;
  graph->subgraph = NULL;
  graph->last_subgraph = NULL;
  graph->edge = NULL;
  graph->last_edge = NULL;
  graph->next = NULL;

  return graph;
}

static struct gdl_node *
gdl_new_node (char *title)
{
  struct gdl_node *node;

  node = (struct gdl_node *) xmalloc (sizeof (struct gdl_node));
  gdl_set_node_title (node, title);
  gdl_set_node_label (node, NULL);
  gdl_set_node_vertical_order (node, -1);
  gdl_set_node_color (node, GDL_COLOR_DEFAULT);
  gdl_set_node_shape (node, GDL_SHAPE_DEFAULT);

  node->next = NULL;
  
  return node;
}

static struct gdl_edge *
gdl_new_edge (char *source, char *target)
{
  struct gdl_edge *edge;

  edge = (struct gdl_edge *) xmalloc (sizeof (struct gdl_edge));
  gdl_set_edge_source (edge, source);
  gdl_set_edge_target (edge, target);
  gdl_set_edge_label (edge, NULL);
  gdl_set_edge_linestyle (edge, GDL_LINESTYLE_DEFAULT);

  edge->next = NULL;
  
  return edge;
}

static void 
gdl_add_subgraph (struct gdl_graph *graph, struct gdl_graph *subgraph)
{
  if (graph->subgraph == NULL)
    {
      graph->subgraph = subgraph;
      graph->last_subgraph = subgraph;
    }
  else
    {
      graph->last_subgraph->next = subgraph;
      graph->last_subgraph = subgraph;
    }
}

static void 
gdl_add_node (struct gdl_graph *graph, struct gdl_node *node)
{
  if (graph->node == NULL)
    {
      graph->node = node;
      graph->last_node = node;
    }
  else
    {
      graph->last_node->next = node;
      graph->last_node = node;
    }
}

static void 
gdl_add_edge (struct gdl_graph *graph, struct gdl_edge *edge)
{
  if (graph->edge == NULL)
    {
      graph->edge = edge;
      graph->last_edge = edge;
    }
  else
    {
      graph->last_edge->next = edge;
      graph->last_edge = edge;
    }
}

/* Transform cfg to vcg. */
void
vcg_plugin_cfg_to_vcg (void)
{
  struct gdl_graph *fun_graph;
  struct gdl_graph *bb_graph;
  struct gdl_node *node;
  struct gdl_edge *edge;

  struct vcg_plugin_control_flow_graph *cfg;
  struct vcg_plugin_basic_block *bb;
  struct vcg_plugin_edge *e;

  char *str_a, *str_b;

  /* top graph */
  vcg_plugin_top_graph = gdl_new_graph (NULL);
  gdl_set_graph_node_shape (vcg_plugin_top_graph, GDL_BOX);
  gdl_set_graph_node_borderwidth (vcg_plugin_top_graph, 1);
  gdl_set_graph_node_margin (vcg_plugin_top_graph, 1);
  gdl_set_graph_edge_thickness (vcg_plugin_top_graph, 1);
  gdl_set_graph_layout_algorithm (vcg_plugin_top_graph, GDL_MAX_DEPTH);
  gdl_set_graph_folding (vcg_plugin_top_graph, -1);
  gdl_set_graph_splines (vcg_plugin_top_graph, "yes");

  for (vcg_plugin_current_function = vcg_plugin_first_function;
       vcg_plugin_current_function != NULL;
       vcg_plugin_current_function = vcg_plugin_current_function->next)
    {
      cfg = vcg_plugin_current_function->cfg;

      /* function graph */
      fun_graph = gdl_new_graph (vcg_plugin_current_function->name);
      gdl_set_graph_node_color (fun_graph, GDL_WHITE);
      gdl_set_graph_folding (fun_graph, 1);
      gdl_add_subgraph (vcg_plugin_top_graph, fun_graph);

      for (bb = cfg->bb; bb != NULL; bb = bb->next)
        {
          /* bb graph */
          str_a = concat (vcg_plugin_current_function->name, ".", bb->name, NULL);
          if (strcmp (bb->name, "ENTRY") == 0
              || strcmp (bb->name, "EXIT") == 0)
            str_b = bb->name;
          else
            str_b = concat ("bb ", bb->name, NULL);

          bb_graph = gdl_new_graph (str_a);
          gdl_set_graph_label (bb_graph, str_b);
          gdl_set_graph_vertical_order (bb_graph, bb->max_distance);
          gdl_set_graph_folding (bb_graph, 1);
          gdl_set_graph_shape (bb_graph, GDL_ELLIPSE);
          gdl_add_subgraph (fun_graph, bb_graph);

          /* bb node */
          str_a = concat (vcg_plugin_current_function->name, "_", bb->name, NULL);
          node = gdl_new_node (str_a);
          if (bb->text == NULL)
            {
              gdl_set_node_label (node, bb->name);
            }
          else
            {
              str_b = concat ("<bb ", bb->name, ">:\n", bb->text, NULL);
              gdl_set_node_label (node, str_b);
            }

          gdl_set_node_vertical_order (node, bb->max_distance);
          gdl_add_node (bb_graph, node);
        }

      for (e = cfg->edge; e != NULL; e = e->next)
        {
          /* edge */
          str_a = concat (vcg_plugin_current_function->name, ".", e->source->name, NULL);
          str_b = concat (vcg_plugin_current_function->name, ".", e->target->name, NULL);
          edge = gdl_new_edge (str_a, str_b);
          if (e->type == VCG_PLUGIN_RETREATING_EDGE)
            gdl_set_edge_linestyle (edge, GDL_DASHED);

          gdl_add_edge (fun_graph, edge);
        }
    }
}
