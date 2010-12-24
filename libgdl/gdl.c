/* Graph description languange.

   Copyright (C) 2009, 2010 Mingjie Xing, mingjie.xing@gmail.com. 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <error.h>
#include <libiberty.h>

#include "gdl.h"

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

gdl_graph *vcg_plugin_top_graph;

gdl_graph *
gdl_new_graph (const char *title)
{
  gdl_graph *graph;

  graph = XNEW(gdl_graph);
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

gdl_node *
gdl_new_node (const char *title)
{
  gdl_node *node;

  node = (gdl_node *) xmalloc (sizeof (gdl_node));
  gdl_set_node_title (node, title);
  gdl_set_node_label (node, NULL);
  gdl_set_node_vertical_order (node, -1);
  gdl_set_node_color (node, GDL_COLOR_DEFAULT);
  gdl_set_node_shape (node, GDL_SHAPE_DEFAULT);

  node->next = NULL;
  
  return node;
}

gdl_edge *
gdl_new_edge (const char *source, const char *target)
{
  gdl_edge *edge;

  edge = (gdl_edge *) xmalloc (sizeof (gdl_edge));
  gdl_set_edge_source (edge, source);
  gdl_set_edge_target (edge, target);
  gdl_set_edge_label (edge, NULL);
  gdl_set_edge_linestyle (edge, GDL_LINESTYLE_DEFAULT);

  edge->next = NULL;
  
  return edge;
}

void 
gdl_add_subgraph (gdl_graph *graph, gdl_graph *subgraph)
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

void 
gdl_add_node (gdl_graph *graph, gdl_node *node)
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

void 
gdl_add_edge (gdl_graph *graph, gdl_edge *edge)
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

gdl_graph *
gdl_find_subgraph (gdl_graph *graph, char *title)
{
  gdl_graph *subgraphs, *subgraph;
  
  subgraphs = gdl_get_graph_subgraph (graph);
  for (subgraph = subgraphs; subgraph != NULL; subgraph = subgraph->next)
    if (!strcmp (gdl_get_graph_title (subgraph), title))
      return subgraph;

  return NULL;
}

void
gdl_dump_node (FILE *fout, gdl_node *node)
{
  const char *str;
  int val;
  int i;

  fputs ("node: {\n", fout);

  /* title */
  str = gdl_get_node_title (node);
  if (str != NULL)
    fprintf (fout, "title: \"%s\"\n", str);
  
  /* label */
  str = gdl_get_node_label (node);
  if (str != NULL)
    {
      fprintf (fout, "label: \"");
      for (i = 0; i < strlen (str); i++)
        {
          if (str[i] == '"')
            fprintf (fout,  "\\");
          fprintf (fout, "%c", str[i]);
        }
      fprintf (fout, "\"\n");
    }
  
  /* vertical_order */
  val = gdl_get_node_vertical_order (node);
  if (val != -1)
    fprintf (fout, "vertical_order: %d\n", val);

  /* shape */
  str = gdl_get_node_shape_s (node);
  if (str != NULL)
    fprintf (fout, "shape: %s\n", str); 

  /* color */
  str = gdl_get_node_color_s (node);
  if (str != NULL)
    fprintf (fout, "color: %s\n", str); 

  fputs ("}\n", fout);
}

void
gdl_dump_edge (FILE *fout, gdl_edge *edge)
{
  const char *str;
  int i;

  fputs ("edge: {\n", fout);

  /* sourcename */
  str = gdl_get_edge_source (edge);
  if (str != NULL)
    fprintf (fout, "sourcename: \"%s\"\n", str);
  
  /* targetname */
  str = gdl_get_edge_target (edge);
  if (str != NULL)
    fprintf (fout, "targetname: \"%s\"\n", str);
  
  /* label */
  str = gdl_get_edge_label (edge);
  if (str != NULL)
    {
      fprintf (fout, "label: \"");
      for (i = 0; i < strlen (str); i++)
        {
          if (str[i] == '"')
            fprintf (fout,  "\\");
          fprintf (fout, "%c", str[i]);
        }
      fprintf (fout, "\"\n");
    }
  
  /* linestyle */
  str = gdl_get_edge_linestyle_s (edge);
  if (str != NULL)
    fprintf (fout, "linestyle: %s\n", str);

  fputs ("}\n", fout);
}

void
gdl_dump_graph (FILE *fout, gdl_graph *graph)
{
  gdl_node *nodes, *node;
  gdl_edge *edges, *edge;
  gdl_graph *subgraphs, *subgraph;

  const char *str;
  int val;
  int i;

  fputs ("graph: {\n", fout);

  /* Dump the general graph attributes.  */

  /* title */
  str = gdl_get_graph_title (graph);
  if (str != NULL)
    fprintf (fout, "title: \"%s\"\n", str);

  /* label */
  str = gdl_get_graph_label (graph);
  if (str != NULL)
    {
      fprintf (fout, "label: \"");
      for (i = 0; i < strlen (str); i++)
        {
          if (str[i] == '"')
            fprintf (fout,  "\\");
          fprintf (fout, "%c", str[i]);
        }
      fprintf (fout, "\"\n");
    }

  /* color */
  str = gdl_get_graph_color_s (graph);
  if (str != NULL)
    fprintf (fout, "color: %s\n", str);

  /* node.color */
  str = gdl_get_graph_node_color_s (graph);
  if (str != NULL)
    fprintf (fout, "node.color: %s\n", str);

  /* node.borderwidth */
  val = gdl_get_graph_node_borderwidth (graph);
  if (val != -1)
    fprintf (fout, "node.borderwidth: %d\n", val);

  /* node.margin */
  val = gdl_get_graph_node_margin (graph);
  if (val != -1)
    fprintf (fout, "//node.margin: %d\n", val);

  /* edge.thickness */
  val = gdl_get_graph_edge_thickness (graph);
  if (val != -1)
    fprintf (fout, "edge.thickness: %d\n", val);

  /* folding */
  val = gdl_get_graph_folding (graph);
  if (val != -1)
    fprintf (fout, "folding: %d\n", val);

  /* vertical order */
  val = gdl_get_graph_vertical_order (graph);
  if (val != -1)
    fprintf (fout, "vertical_order: %d\n", val);

  /* splines */
  str = gdl_get_graph_splines (graph);
  if (str != NULL)
    fprintf (fout, "splines: %s\n", str);

  /* shape */
  str = gdl_get_graph_shape_s (graph);
  if (str != NULL)
    fprintf (fout, "shape: %s\n", str);

  /* layoutalgorithm */
  str = gdl_get_graph_layout_algorithm_s (graph);
  if (str != NULL)
    fprintf (fout, "//layoutalgorithm: %s\n", str);

  /* near_edges */
  val = gdl_get_graph_near_edges (graph);
  if (val != -1)
    fprintf (fout, "near_edges: %s\n", val ? "yes" : "no");

  /* port_sharing */
  val = gdl_get_graph_port_sharing (graph);
  if (val != -1)
    fprintf (fout, "port_sharing: %s\n", val ? "yes" : "no");

  /* Dump the nodes.  */
  nodes = gdl_get_graph_node (graph);
  for (node = nodes; node != NULL; node = node->next)
    gdl_dump_node (fout, node);

  /* Dump the subgraphs.  */
  subgraphs = gdl_get_graph_subgraph (graph);
  for (subgraph = subgraphs; subgraph != NULL; subgraph = subgraph->next)
    gdl_dump_graph (fout, subgraph);

  /* Dump the edges.  */
  edges = gdl_get_graph_edge (graph);
  for (edge = edges; edge != NULL; edge = edge->next)
    gdl_dump_edge (fout, edge);

  fputs ("}\n", fout);
}

