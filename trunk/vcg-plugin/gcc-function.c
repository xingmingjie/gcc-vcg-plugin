/* <one line to give the program's name and a bief idea of what it does.>
 *
 * Copyright (C) 2010 Mingjie Xing, mingjie.xing@gmail.com.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <config.h>

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "vcg-plugin.h"
#include "gdl.h"

static char *function_name;

/* Create a graph from the basic block bb. */

static gdl_graph *
create_bb_graph (basic_block bb)
{
  gdl_graph *g;
  gdl_node *n;
  int index;
  char buf[32]; /* Should be enough. */
  char *title, *label;
  gimple_stmt_iterator gsi;
  gimple stmt;

  /* bb_graph's title: fn_name.bb_index */
  index = bb->index;
  sprintf (buf, "%d", index);
  title = concat (current_function_name, ".", buf);

  g = gdl_new_graph (title);

  /* bb_graph's label: ENTRY | EXIT | bb bb_index */
  if (index == 0)
    gdl_set_graph_label (g, "ENTRY");
  else if (index == 1)
    gdl_set_graph_label (g, "EXIT");
  else
    gdl_set_graph_label (g, buf);

  /* bb_node's title: ENTRY | EXIT | bb bb_index */
  if (index == 0)
    n = gdl_new_node ("ENTRY");
  else if (index == 1)
    n = gdl_new_node ("EXIT");
  else
    n = gdl_new_node (buf);

  for (gsi = gsi_start_bb (bb); !gsi_end_p (gdi); gsi_next (&gsi))
    {
      stmt = gsi_stmt (gsi);
      init_print_file ();
      print_gimple_stmt (print_file, stmt, 2, flag);
    }
}

/* Create a graph from the function fn. */

static void
create_function_graph (tree fn)
{
  basic_block bb;
  gdl_graph *g;

  /* Get the function's name. */
  function_name = lang_hooks.decl_printable_name (fn, 2);

  top_graph = gld_new_graph (function_name);

  /* Switch CFUN to point to FN. */
  push_cfun (DECL_STRUCT_FUNCTION (fn));

  if (cfun && cfun->decl == fn && cfun->cfg && basic_block_info)
    {
      FOR_EACH_BB (bb)
        {
          g = create_bb_graph (bb);
          gdl_add_subgraph (top_graph, g);
        }
    }
}

static void
exit_if_invalid (tree fn)
{
}

void
vcg_plugin_view_function (tree fn)
{
  struct gdl_graph *g;

  exit_if_invalid (fn);

  vcg.init ();

  /* */
  g = create_function_graph (fn);

  vcg.add_graph (g);

  vcg.show ();
  vcg.cleanup ();
}

