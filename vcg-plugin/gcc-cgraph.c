/* Dump or view gcc call graph.

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
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */ 

#include "vcg-plugin.h"

static htab_t title_table;
static char buf[1024];

/* For the hash tables.  */

static int
string_hash_eq (const void *y1, const void *y2)
{
  return strcmp ((const char *) y1, (const char *) y2) == 0;
}


static char*
get_label (struct cgraph_node *node)
{
  return (char *) cgraph_node_name (node);
}

static char*
get_title (struct cgraph_node *node, char *prefix)
{
  char **slot;
  char *title;

  if (prefix == NULL)
    return get_label (node);

  sprintf (buf, "%s.%s", prefix, (char *) cgraph_node_name (node));
  title = htab_find (title_table, buf);
  if (title == NULL)
    {
      vcg_plugin_common.buf_print ("%s", buf);
      title = vcg_plugin_common.buf_finish ();
      slot = (char **) htab_find_slot (title_table, buf, INSERT);
      *slot = title;
    }

  return title;
}

static void
create_edge (gdl_graph *graph, struct cgraph_edge *edge, char *prefix)
{
  char *source_title, *target_title;
  struct cgraph_node *source, *target;
  
  source = edge->caller;
  target = edge->callee;

  source_title = get_title (source, prefix);
  target_title = get_title (target, prefix);
  gdl_new_graph_edge (graph, source_title, target_title);
}

static void
create_node (gdl_graph *graph, struct cgraph_node *node, char *prefix)
{
  char *title, *label;
  gdl_node *gnode;

  title = get_title (node, prefix);
  label = get_label (node);
  gnode = gdl_new_graph_node (graph, title);
  gdl_set_node_label (gnode, label);
}

static void
create_node_and_edges_callee (gdl_graph *graph, struct cgraph_node *node)
{
  struct cgraph_node *source, *target;
  struct cgraph_edge *edge;
  struct cgraph_edge **stack;
  char *prefix;
  int sp;

  title_table = htab_create (10, htab_hash_string, string_hash_eq, NULL);

  prefix = get_label (node);
  create_node (graph, node, prefix);

  stack = XNEWVEC (struct cgraph_edge *, cgraph_n_nodes + 1);
  sp = 0;

  edge = node->callees;
  while (1)
    {
      while (edge)
        {
          source = edge->caller;
          target = edge->callee;

          if (gdl_find_edge (graph, get_title (source, prefix),
                             get_title (target, prefix)) == NULL)
            create_edge (graph, edge, prefix);

          if (gdl_find_node (graph, get_title (target, prefix)) == NULL)
            {
              create_node (graph, target, prefix);

              stack[++sp] = edge;
              edge = target->callees;

              continue;
            }

          edge = edge->next_callee;
        }

      if (sp == 0)
        break;

      edge = stack[sp--];
      edge = edge->next_callee;
    }
  free (stack);
  htab_delete (title_table);
}

/* Dump callee graph into the file FNAME.  */

static void
dump_cgraph_callee_to_file (char *fname)
{
  gdl_graph *graph;
  struct cgraph_node *node;

  graph = vcg_plugin_common.top_graph;
  gdl_set_graph_orientation (graph, "left_to_right");

  for (node = cgraph_nodes; node; node = node->next)
    create_node_and_edges_callee (graph, node);

  vcg_plugin_common.dump (fname);
}

/* Public function to dump callee graph.  */

void
vcg_plugin_dump_cgraph_callee (void)
{
  char *fname;

  vcg_plugin_common.init ();

  fname = concat (dump_base_name, ".cgraph-callee.vcg", NULL);
  dump_cgraph_callee_to_file (fname);
  free (fname);

  vcg_plugin_common.finish ();
}

/* Public function to view callee graph.  */

void
vcg_plugin_view_cgraph_callee (void)
{
  vcg_plugin_common.init ();

  dump_cgraph_callee_to_file (vcg_plugin_common.temp_file_name);
  vcg_plugin_common.show (vcg_plugin_common.temp_file_name);

  vcg_plugin_common.finish ();
}

static void
create_node_and_edges (gdl_graph *graph, struct cgraph_node *node)
{
  struct cgraph_edge *edge;
  char *title, *title_a;

  title = (char *) cgraph_node_name (node);
  gdl_new_graph_node (graph, title);

  for (edge = node->callees; edge; edge = edge->next_callee)
    {
      title_a = (char *) cgraph_node_name (edge->callee);
      if (gdl_find_edge (graph, title, title_a))
        continue;
      gdl_new_graph_edge (graph, title, title_a);
    }
}

/* Dump call graph into the file FNAME.  */

static void
dump_cgraph_to_file (char *fname)
{
  gdl_graph *graph;
  struct cgraph_node *node;

  graph = vcg_plugin_common.top_graph;
  gdl_set_graph_orientation (graph, "left_to_right");

  for (node = cgraph_nodes; node; node = node->next)
    create_node_and_edges (graph, node);

  vcg_plugin_common.dump (fname);
}

/* Public function to dump call graph.  */

void
vcg_plugin_dump_cgraph (void)
{
  char *fname;

  vcg_plugin_common.init ();

  fname = concat (dump_base_name, ".cgraph.vcg", NULL);
  dump_cgraph_to_file (fname);
  free (fname);

  vcg_plugin_common.finish ();
}

/* Public function to view call graph.  */

void
vcg_plugin_view_cgraph (void)
{
  vcg_plugin_common.init ();

  dump_cgraph_to_file (vcg_plugin_common.temp_file_name);
  vcg_plugin_common.show (vcg_plugin_common.temp_file_name);

  vcg_plugin_common.finish ();
}


