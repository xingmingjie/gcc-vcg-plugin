/* Graph description languange.

   Copyright (C) 2009, 2010, 2011 Mingjie Xing, mingjie.xing@gmail.com. 

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

/* default attributes */

#define DEF_ATTR(id, name, type, code, value) \
  { code, name, (union gdl_attr_value) value },

static gdl_attr node_default_attr[] =
{
  #include "node-attr.def"
};
static gdl_attr edge_default_attr[] =
{
  #include "edge-attr.def"
};
static gdl_attr graph_default_attr[] =
{
  #include "graph-attr.def"
};
#undef DEF_ATTR

/* Create a node for a given TITLE.  */

gdl_node *
gdl_new_node (const char *title)
{
  gdl_node *node;

  node = (gdl_node *) xmalloc (sizeof (gdl_node));
  memcpy (node->attr, node_default_attr, sizeof (node_default_attr));

  /* Duplicate the string.  */
  node->title = strdup (title);

  node->next = NULL;
  node->parent = NULL;
  
  return node;
}

/* Create an edge for a given SOURCE and TARGET.  */

gdl_edge *
gdl_new_edge (const char *source, const char *target)
{
  gdl_edge *edge;

  edge = (gdl_edge *) xmalloc (sizeof (gdl_edge));
  memcpy (edge->attr, edge_default_attr, sizeof (edge_default_attr));

  /* Duplicate the string.  */
  edge->source = strdup (source);
  edge->target = strdup (target);

  edge->next = NULL;
  
  return edge;
}

/* Create a graph for a given TITLE.  */

gdl_graph *
gdl_new_graph (const char *title)
{
  gdl_graph *graph;

  graph = XNEW(gdl_graph);
  memcpy (graph->attr, graph_default_attr, sizeof (graph_default_attr));

  /* Duplicate the string.  */
  graph->title = strdup (title);

  graph->node = NULL;
  graph->last_node = NULL;
  graph->subgraph = NULL;
  graph->last_subgraph = NULL;
  graph->edge = NULL;
  graph->last_edge = NULL;
  graph->next = NULL;
  graph->parent = NULL;

  return graph;
}

/* Free the NODE.  */

void
gdl_free_node (gdl_node *node)
{
  free (node->title);
  free (node);
}

/* Free the EDGE.  */

void
gdl_free_edge (gdl_edge *edge)
{
  free (edge->source);
  free (edge->target);
  free (edge);
}

/* Free the GRAPH.  */

void
gdl_free_graph (gdl_graph *graph)
{
  gdl_graph *subgraphs, *subgraph, *next_subgraph;
  gdl_node *nodes, *node, *next_node;
  gdl_edge *edges, *edge, *next_edge;

  /* Free the nodes.  */
  nodes = gdl_get_graph_node (graph);
  for (node = nodes; node != NULL;)
    {
      next_node = node->next;
      gdl_free_node (node);
      node = next_node;
    }

  /* Free the edges.  */
  edges = gdl_get_graph_edge (graph);
  for (edge = edges; edge != NULL;)
    {
      next_edge = edge->next;
      gdl_free_edge (edge);
      edge = next_edge;
    }

  /* Free the subgraphs.  */
  subgraphs = gdl_get_graph_subgraph (graph);
  for (subgraph = subgraphs; subgraph != NULL;)
    {
      next_subgraph = subgraph->next;
      gdl_free_graph (subgraph);
      subgraph = next_subgraph;
    }

  /* Free the graph.  */
  free (graph->title);
  free (graph);
}

/* Add NODE into GRAPH.  */

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
  node->parent = graph;
}

/* Add EDGE into GRAPH.  */

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

/* Add SUBGRAPH into GRAPH.  */

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
  subgraph->parent = graph;
}

/* Find the node in GRAPH for a given TITLE.  */

gdl_node *
gdl_find_node (gdl_graph *graph, char *title)
{
  gdl_edge *nodes, *node;

  nodes = gdl_get_graph_node (graph); 
  for (node = nodes; node; node = node->next)
    if (!strcmp (gdl_get_node_title (node), title))
      return node;

  return NULL;
}

/* Find the edge in GRAPH for a given SOURCE and TARGET.  */

gdl_edge *
gdl_find_edge (gdl_graph *graph, char *source, char *target)
{
  gdl_edge *edges, *edge;
  char *srcname, *destname;

  edges = gdl_get_graph_edge (graph); 
  for (edge = edges; edge; edge = edge->next)
    {
      srcname = gdl_get_edge_source (edge);
      destname = gdl_get_edge_target (edge);
      if (!strcmp (srcname, source) && !strcmp (destname, target))
        return edge;
    }

  return NULL;
}

/* Find the subgraph in GRAPH for a given TITLE.  */

gdl_graph *
gdl_find_subgraph (gdl_graph *graph, char *title)
{
  gdl_graph *subgraphs, *subgraph;
  
  subgraphs = gdl_get_graph_subgraph (graph);
  for (subgraph = subgraphs; subgraph; subgraph = subgraph->next)
    if (!strcmp (gdl_get_graph_title (subgraph), title))
      return subgraph;

  return NULL;
}

/* Print the string into the file and add a '\' before each '"'.  */

static inline void
print_string (FILE *fout, const char *str)
{
  int i;

  for (i = 0; i < strlen (str); i++)
    {
      if (str[i] == '"')
        fprintf (fout,  "\\");
      fprintf (fout, "%c", str[i]);
    }
}

/* Dump NODE into the file.  */

void
gdl_dump_node (FILE *fout, gdl_node *node)
{
  int i;
  gdl_attr *attr;

  fputs ("node: {\n", fout);

  /* Dump the attributes.  */
  for (i = 0; i < GDL_NODE_ATTR_MAX; i++)
    {
      attr = &node->attr[i];
      switch (attr->code)
        {
        case GDL_CODE_STR:
          if (attr->value.str != node_default_attr[i].value.str)
            fprintf (fout, "%s: %s\n", attr->name, attr->value.str);
          break;
        case GDL_CODE_STR_QUOTE:
          if (attr->value.str != node_default_attr[i].value.str)
            {
              fprintf (fout, "%s: \"", attr->name);
              print_string (fout, attr->value.str);
              fprintf (fout, "\"\n");
            }
          break;
        case GDL_CODE_INT:
          if (attr->value.val != node_default_attr[i].value.val)
            fprintf (fout, "%s: %d\n", attr->name, attr->value.val);
          break;
        default:
          assert (0);
        }
    }

  fputs ("}\n", fout);
}

/* Dump EDGE into the file.  */

void
gdl_dump_edge (FILE *fout, gdl_edge *edge)
{
  int i;
  gdl_attr *attr;

  fputs ("edge: {\n", fout);

  /* Dump the attributes.  */
  for (i = 0; i < GDL_EDGE_ATTR_MAX; i++)
    {
      attr = &edge->attr[i];
      switch (attr->code)
        {
        case GDL_CODE_STR:
          if (attr->value.str != edge_default_attr[i].value.str)
            fprintf (fout, "%s: %s\n", attr->name, attr->value.str);
          break;
        case GDL_CODE_STR_QUOTE:
          if (attr->value.str != edge_default_attr[i].value.str)
            {
              fprintf (fout, "%s: \"", attr->name);
              print_string (fout, attr->value.str);
              fprintf (fout, "\"\n");
            }
          break;
        case GDL_CODE_INT:
          if (attr->value.val != edge_default_attr[i].value.val)
            fprintf (fout, "%s: %d\n", attr->name, attr->value.val);
          break;
        default:
          assert (0);
        }
    }

  fputs ("}\n", fout);
}

/* Dump GRAPH into the file.  */

void
gdl_dump_graph (FILE *fout, gdl_graph *graph)
{
  int i;
  gdl_node *nodes, *node;
  gdl_edge *edges, *edge;
  gdl_graph *subgraphs, *subgraph;
  gdl_attr *attr;

  fputs ("graph: {\n", fout);

  /* Dump the attributes.  */
  for (i = 0; i < GDL_GRAPH_ATTR_MAX; i++)
    {
      attr = &graph->attr[i];
      switch (attr->code)
        {
        case GDL_CODE_STR:
          if (attr->value.str != graph_default_attr[i].value.str)
            fprintf (fout, "%s: %s\n", attr->name, attr->value.str);
          break;
        case GDL_CODE_STR_QUOTE:
          if (attr->value.str != graph_default_attr[i].value.str)
            {
              fprintf (fout, "%s: \"", attr->name);
              print_string (fout, attr->value.str);
              fprintf (fout, "\"\n");
            }
          break;
        case GDL_CODE_INT:
          if (attr->value.val != graph_default_attr[i].value.val)
            fprintf (fout, "%s: %d\n", attr->name, attr->value.val);
          break;
        default:
          assert (0);
        }
    }

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

