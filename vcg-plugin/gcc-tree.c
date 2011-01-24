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

static int title_id;
struct obstack str_obstack;

static htab_t tree_table;

static gdl_node *create_tree_node (gdl_graph *graph, tree tn, char *name,
                                   int nested_level);

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

  if (common == NULL)
    return NULL;

  sprintf (buf, "node.%d", title_id++);
  title = xstrdup (buf);
  vcg_plugin_common.tag (title);
  node = gdl_new_node (title);
  gdl_add_node (graph, node);

  /* Avoid nested level is too deep.  */
  if (nested_level > 10)
    {
      gdl_set_node_label (node, "...");
      return node;
    }

  sprintf (buf, "common\n");
  obstack_grow (&str_obstack, buf, strlen (buf));
  sprintf (buf, "----------\n");
  obstack_grow (&str_obstack, buf, strlen (buf));
  sprintf (buf, "%s", ts_enum_names[tns]);
  obstack_grow (&str_obstack, buf, strlen (buf));
  obstack_1grow (&str_obstack, '\0');
  label = obstack_finish (&str_obstack);
  gdl_set_node_label (node, label);

  switch (tns)
    {
    case TS_BASE:
      break;
      
    case TS_COMMON:
      #define tx (*(struct tree_common *) common)
      //anode = create_common_node (graph, &tx.base, TS_BASE, nested_level + 1);
      //create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.chain, "chain", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.type, "type", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_INT_CST:
    case TS_REAL_CST:
    case TS_FIXED_CST:
    case TS_VECTOR:
    case TS_STRING:
    case TS_COMPLEX:
    case TS_IDENTIFIER:
      abort ();
      break;
      
    case TS_DECL_MINIMAL:
      #define tx (*(struct tree_decl_minimal *) common)
      anode = create_common_node (graph, &tx.common, TS_COMMON,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.name, "name", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.context, "context", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_DECL_COMMON:
      #define tx (*(struct tree_decl_common *) common)
      anode = create_common_node (graph, &tx.common, TS_DECL_MINIMAL,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.size, "size", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.size_unit, "size_unit", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.initial, "initial", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.attributes, "attributes", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.abstract_origin, "abstract_origin", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_DECL_WRTL:
      #define tx (*(struct tree_decl_with_rtl *) common)
      anode = create_common_node (graph, &tx.common, TS_DECL_COMMON,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_DECL_NON_COMMON:
      #define tx (*(struct tree_decl_non_common *) common)
      anode = create_common_node (graph, &tx.common, TS_DECL_WITH_VIS,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.saved_tree, "saved_tree", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.arguments, "arguments", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.result, "result", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.vindex, "vindex", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_DECL_WITH_VIS:
      #define tx (*(struct tree_decl_with_vis *) common)
      anode = create_common_node (graph, &tx.common, TS_DECL_WRTL,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.assembler_name, "assembler_name", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.section_name, "section_name", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.comdat_group, "comdat_group", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_FIELD_DECL:
    case TS_VAR_DECL:
    case TS_PARM_DECL:
    case TS_LABEL_DECL:
    case TS_RESULT_DECL:
    case TS_CONST_DECL:
    case TS_TYPE_DECL:
    case TS_FUNCTION_DECL:
    case TS_TRANSLATION_UNIT_DECL:
    case TS_TYPE:
    case TS_LIST:
    case TS_VEC:
    case TS_EXP:
    case TS_SSA_NAME:
    case TS_BLOCK:
    case TS_BINFO:
    case TS_STATEMENT_LIST:
    case TS_CONSTRUCTOR:
    case TS_OMP_CLAUSE:
    case TS_OPTIMIZATION:
    case TS_TARGET_OPTION:
      abort ();
      break;
    }

  return node;
}

/* Create a gdl node for tree TN and add it into GRAPH.  NESTED_LEVEL stands
   for the current nested level for gcc tree structure data.  */

static gdl_node *
create_tree_node (gdl_graph *graph, tree tn, char *name, int nested_level)
{
  gdl_node *node, *anode;
  enum tree_node_structure_enum tns;
  char buf[256];
  char *title, *label;
  void **slot;

  if (tn == 0)
    return NULL;

  sprintf (buf, "node.%d", title_id++);
  title = xstrdup (buf);
  vcg_plugin_common.tag (title);
  node = gdl_new_node (title);
  gdl_add_node (graph, node);

  /* Avoid nested level is too deep.  */
  if (nested_level > 10)
    {
      gdl_set_node_label (node, "...");
      return node;
    }

  tns = tree_node_structure (tn);
  
  sprintf (buf, "%s\n", name);
  obstack_grow (&str_obstack, buf, strlen (buf));
  sprintf (buf, "----------\n");
  obstack_grow (&str_obstack, buf, strlen (buf));
  sprintf (buf, "addr: 0x%x\n", tn);
  obstack_grow (&str_obstack, buf, strlen (buf));
  sprintf (buf, "type: %s", ts_enum_names[tns]);
  obstack_grow (&str_obstack, buf, strlen (buf));

  switch (tns)
    {
    case TS_BASE:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_COMMON:
      #define tx tn->common
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      anode = create_common_node (graph, &tx.base, TS_BASE,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.chain, "chain", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.type, "type", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_INT_CST:
      #define tx tn->int_cst
      sprintf (buf, "\nvalue: %ld", tx.int_cst);
      obstack_grow (&str_obstack, buf, strlen (buf));
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      #undef tx
      break;
      
    case TS_REAL_CST:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_FIXED_CST:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_VECTOR:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_STRING:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_COMPLEX:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_IDENTIFIER:
      #define tx tn->identifier
      sprintf (buf, "\nvalue: %s", tx.id.str);
      obstack_grow (&str_obstack, buf, strlen (buf));
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      anode = create_common_node (graph, &tx.common, TS_COMMON,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_DECL_MINIMAL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_DECL_COMMON:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_DECL_WRTL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_DECL_NON_COMMON:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_DECL_WITH_VIS:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_FIELD_DECL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_VAR_DECL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_PARM_DECL:
      #define tx tn->parm_decl
      sprintf (buf, "\nann = 0x%x", tx.ann);
      obstack_grow (&str_obstack, buf, strlen (buf));
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      anode = create_common_node (graph, &tx.common, TS_DECL_WRTL,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_LABEL_DECL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_RESULT_DECL:
      #define tx tn->result_decl
      sprintf (buf, "\nann = 0x%x", tx.ann);
      obstack_grow (&str_obstack, buf, strlen (buf));
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      anode = create_common_node (graph, &tx.common, TS_DECL_WRTL,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_CONST_DECL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_TYPE_DECL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_FUNCTION_DECL:
      #define tx tn->function_decl
      if (htab_find (tree_table, tn))
        {
          sprintf (buf, "\nduplicated");
          obstack_grow (&str_obstack, buf, strlen (buf));
          obstack_1grow (&str_obstack, '\0');
          label = obstack_finish (&str_obstack);
          gdl_set_node_label (node, label);

          return node;
        }
      slot = htab_find_slot (tree_table, tn, INSERT);
      *slot = tn;
      sprintf (buf, "\nf = 0x%x", tx.f);
      obstack_grow (&str_obstack, buf, strlen (buf));
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      anode = create_common_node (graph, &tx.common, TS_DECL_NON_COMMON,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.personality, "personality", nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.function_specific_target, "function_specific_target",
                                nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.function_specific_optimization, "function_specific_optimization",
                                nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_TRANSLATION_UNIT_DECL:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_TYPE:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_LIST:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_VEC:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_EXP:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_SSA_NAME:
      #define tx tn->ssa_name
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      anode = create_common_node (graph, &tx.common, TS_COMMON,
                                  nested_level + 1);
      create_edge (graph, node, anode);
      anode = create_tree_node (graph, tx.var, "var", nested_level + 1);
      create_edge (graph, node, anode);
      #undef tx
      break;
      
    case TS_BLOCK:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_BINFO:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_STATEMENT_LIST:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_CONSTRUCTOR:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_OMP_CLAUSE:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_OPTIMIZATION:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
      
    case TS_TARGET_OPTION:
      obstack_1grow (&str_obstack, '\0');
      label = obstack_finish (&str_obstack);
      gdl_set_node_label (node, label);
      break;
    }
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

  create_tree_node (graph, tn, "tree", 0);

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
  tree_table = htab_create (32, htab_hash_pointer, htab_eq_pointer, NULL);

  graph = create_tree_graph (node);
  gdl_dump_graph (fp, graph);
  gdl_free_graph (graph);

  fclose (fp);
  obstack_free (&str_obstack, NULL);
  htab_delete (tree_table);
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

