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
#include "intl.h"
#include "langhooks.h"
#include "cfghooks.h"
#include "version.h" /* for printing gcc version number in graph */

#include "vcg-plugin.h"

#include "tree.h"

#undef DEFTREESTRUCT
#define DEFTREESTRUCT(VAL, NAME) NAME,

static const char *ts_enum_names[] = {
#include "treestruct.def"
};
#undef DEFTREESTRUCT

#define TREE_DECL_NON_COMMON 1
#define TREE_DECL_WITH_VIS 2

static int title_id;
struct obstack str_obstack;

static gdl_node *create_tree_node (gdl_graph *graph, tree tn, int nested_level);

/* Create gdl edge and add it into GRAPH.  SN is source node,
   TN is target node.  */

static void
create_edge (gdl_graph *graph, gdl_node *sn, gdl_node *tn)
{
  gdl_edge *edge;

  if (tn == NULL || sn == NULL)
    return;

  edge = gdl_new_edge (gdl_get_node_title (sn), gdl_get_node_title (tn));
  gdl_add_edge (graph, edge);
}

/* Create gdl node for tree common part COMMON and add it into GRAPH.  */

static gdl_node *
create_common_node (gdl_graph *graph, void *common,
                    enum tree_node_structure_enum tns, int nested_level)
{
  gdl_node *node, *anode;
  char buf[256];
  char *title, *label;

  if (common == NULL || nested_level > 10)
    return NULL;

  sprintf (buf, "node.%d", title_id++);
  title = xstrdup (buf);
  vcg_plugin_common.tag (title);
  node = gdl_new_node (title);
  gdl_add_node (graph, node);
  gdl_set_node_label (node, ts_enum_names[tns]);

  switch (tns)
    {
    case TS_BASE:
      break;
      
    case TS_COMMON:
      break;
      
    case TS_INT_CST:
      break;
      
    case TS_REAL_CST:
      break;
      
    case TS_FIXED_CST:
      break;
      
    case TS_VECTOR:
      break;
      
    case TS_STRING:
      break;
      
    case TS_COMPLEX:
      break;
      
    case TS_IDENTIFIER:
      break;
      
    case TS_DECL_MINIMAL:
      break;
      
    case TS_DECL_COMMON:
      break;
      
    case TS_DECL_WRTL:
      break;
      
    case TS_DECL_NON_COMMON:
      #define tx (*(struct tree_decl_non_common *) common)
      anode = create_common_node (graph, &tx.common, TS_DECL_WITH_VIS,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.saved_tree, nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.arguments, nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.result, nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.vindex, nested_level + 1);
      create_edge (graph, node, anode);
      break;
      #undef tx
      
    case TS_DECL_WITH_VIS:
      #define tx (*(struct tree_decl_with_vis *) common)
      anode = create_common_node (graph, &tx.common, TS_DECL_WITH_RTL,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.saved_tree, nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.saved_tree, nested_level + 1);
      create_edge (graph, node, anode);
      break;
      #undef tx
      
    case TS_FIELD_DECL:
      break;
      
    case TS_VAR_DECL:
      break;
      
    case TS_PARM_DECL:
      break;
      
    case TS_LABEL_DECL:
      break;
      
    case TS_RESULT_DECL:
      break;
      
    case TS_CONST_DECL:
      break;
      
    case TS_TYPE_DECL:
      break;
      
    case TS_FUNCTION_DECL:
      break;
      
    case TS_TYPE:
      break;
      
    case TS_LIST:
      break;
      
    case TS_VEC:
      break;
      
    case TS_EXP:
      break;
      
    case TS_SSA_NAME:
      break;
      
    case TS_BLOCK:
      break;
      
    case TS_BINFO:
      break;
      
    case TS_STATEMENT_LIST:
      break;
      
    case TS_CONSTRUCTOR:
      break;
      
    case TS_OMP_CLAUSE:
      break;
      
    case TS_OPTIMIZATION:
      break;
      
    case TS_TARGET_OPTION:
      break;
    }

  return node;
}

/* Create a gdl node for tree TN and add it into GRAPH.  NESTED_LEVEL stands
   for the current nested level for gcc tree structure data.  */

static gdl_node *
create_tree_node (gdl_graph *graph, tree tn, int nested_level)
{
  gdl_node *node, *anode;
  enum tree_node_structure_enum tns;
  char buf[256];
  char *title, *label;

  if (tn == 0 || nested_level > 10)
    return NULL;

  sprintf (buf, "node.%d", title_id++);
  title = xstrdup (buf);
  vcg_plugin_common.tag (title);
  node = gdl_new_node (title);
  gdl_add_node (graph, node);

  tns = tree_node_structure (tn);
  
  obstack_grow (&str_obstack, ts_enum_names[tns], strlen (ts_enum_names[tns]));
  obstack_1grow (&str_obstack, '\n');
  switch (tns)
    {
    case TS_BASE:
      sprintf (buf, "TS_BASE");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_COMMON:
      sprintf (buf, "TS_COMMON");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_INT_CST:
      sprintf (buf, "TS_INT_CST");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_REAL_CST:
      sprintf (buf, "TS_REAL_CST");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_FIXED_CST:
      sprintf (buf, "TS_FIXED_CST");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_VECTOR:
      sprintf (buf, "TS_VECTOR");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_STRING:
      sprintf (buf, "TS_STRING");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_COMPLEX:
      sprintf (buf, "TS_COMPLEX");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_IDENTIFIER:
      sprintf (buf, "TS_IDENTIFIER");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_DECL_MINIMAL:
      sprintf (buf, "TS_DECL_MINIMAL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_DECL_COMMON:
      sprintf (buf, "TS_DECL_COMMON");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_DECL_WRTL:
      sprintf (buf, "TS_DECL_WRTL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_DECL_NON_COMMON:
      sprintf (buf, "TS_DECL_NON_COMMON");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_DECL_WITH_VIS:
      sprintf (buf, "TS_DECL_WITH_VIS");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_FIELD_DECL:
      sprintf (buf, "TS_FIELD_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_VAR_DECL:
      sprintf (buf, "TS_VAR_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_PARM_DECL:
      sprintf (buf, "TS_PARM_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_LABEL_DECL:
      sprintf (buf, "TS_LABEL_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_RESULT_DECL:
      sprintf (buf, "TS_RESULT_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_CONST_DECL:
      sprintf (buf, "TS_CONST_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_TYPE_DECL:
      sprintf (buf, "TS_TYPE_DECL");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_FUNCTION_DECL:
      #define tx tn->function_decl
      sprintf (buf, "f = 0x%x\n", tx.f);
      obstack_grow (&str_obstack, buf, strlen (buf));
      anode = create_common_node (graph, &tx.common, TREE_DECL_NON_COMMON,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.personality, nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.function_specific_target,
                                nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.function_specific_optimization,
                                nested_level + 1);
      create_edge (graph, node, anode);
      break;
      #undef tx
      
    case TS_TYPE:
      sprintf (buf, "TS_TYPE");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_LIST:
      sprintf (buf, "TS_LIST");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_VEC:
      sprintf (buf, "TS_VEC");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_EXP:
      sprintf (buf, "TS_EXP");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_SSA_NAME:
      sprintf (buf, "TS_SSA_NAME");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_BLOCK:
      sprintf (buf, "TS_BLOCK");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_BINFO:
      sprintf (buf, "TS_BINFO");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_STATEMENT_LIST:
      sprintf (buf, "TS_STATEMENT_LIST");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_CONSTRUCTOR:
      sprintf (buf, "TS_CONSTRUCTOR");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_OMP_CLAUSE:
      sprintf (buf, "TS_OMP_CLAUSE");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_OPTIMIZATION:
      sprintf (buf, "TS_OPTIMIZATION");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
      
    case TS_TARGET_OPTION:
      sprintf (buf, "TS_TARGET_OPTION");
      obstack_grow (&str_obstack, buf, strlen (buf));
      break;
    }
  label = obstack_finish (&str_obstack);
  vcg_plugin_common.tag (label);
  gdl_set_node_label (node, label);
  return node;
}

/* Create a graph from the tree TN.  */

static gdl_graph *
create_tree_graph (tree tn)
{
  gdl_graph *graph;

  graph = gdl_new_graph ("tree");
  gdl_set_graph_node_borderwidth (graph, 1);
  gdl_set_graph_edge_thickness (graph, 1);
  gdl_set_graph_splines (graph, "yes");

  create_tree_node (graph, tn, 0);

  return graph;
}

/* Dump tree NODE into the file FNAME.  */

static void
dump_tree_to_file (char *fname, tree node)
{
  FILE *fp;
  gdl_graph *graph;

  if ((fp = fopen (fname, "w")) == NULL)
    {
      vcg_plugin_common.error ("failed to open file %s.", fname);
      return;
    }

  obstack_init (&str_obstack);
  title_id = 1;

  graph = create_tree_graph (node);
  gdl_dump_graph (fp, graph);
  gdl_free_graph (graph);

  fclose (fp);
}

/* Public function to dump a gcc tree NODE.  */

void
vcg_plugin_dump_tree (tree node)
{
  char *fname;

  vcg_plugin_common.init ();

  /* Create the dump file name.  */
  asprintf (&fname, "dump-tree-%#x.vcg", node);
  vcg_plugin_common.tag (fname);
  dump_tree_to_file (fname, node);

  vcg_plugin_common.finish ();
}

/* Public function to view a gcc tree NODE.  */

void
vcg_plugin_view_tree (tree node)
{
  char *fname;

  vcg_plugin_common.init ();

  /* Get the temp file name.  */
  fname = vcg_plugin_common.temp_file_name;
  dump_tree_to_file (fname, node);
  vcg_plugin_common.show (fname);

  vcg_plugin_common.finish ();
}

