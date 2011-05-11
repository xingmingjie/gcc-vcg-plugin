/* Dump or view gcc rtx.

   Copyright (C) 2011 Mingjie Xing, mingjie.xing@gmail.com.

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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "vcg-plugin.h"

static struct obstack str_obstack;

/* Temp file stream, used to get the rtx dump. */
static FILE *tmp_stream;
static char *tmp_buf;
static size_t tmp_buf_size;

static void
create_rtx_node (gdl_graph *graph, const_rtx x, int nested_level)
{
  gdl_node *node, *node_x;
  char buf[256];
  char *title, *label;
  enum rtx_code code;
  const char *format_ptr;
  int i;
  const char *str;
  int value;

  if (x == 0)
    return;

  node = gdl_new_node (title);
  gdl_add_node (graph, node);

  /* Avoid nested level is too deep.  */
  if (nested_level > 10)
    {
      gdl_set_node_label (node, "...");
      return;
    }

  code = GET_CODE (x);
  //sprintf (buf, "%s\n", GET_RTX_NAME (code));
  
  rewind (tmp_stream);
  print_rtl_single (tmp_stream, x);
  fflush(tmp_stream);
  obstack_grow (&str_obstack, tmp_buf, tmp_buf_size);
  sprintf (buf, "----------\n");
  obstack_grow (&str_obstack, buf, strlen (buf));
  sprintf (buf, "addr: 0x%x\n", (unsigned) x);
  obstack_grow (&str_obstack, buf, strlen (buf));

  format_ptr = GET_RTX_FORMAT (code);
  sprintf (buf, "format: %s", format_ptr);
  obstack_grow (&str_obstack, buf, strlen (buf));

  obstack_1grow (&str_obstack, '\0');
  label = obstack_finish (&str_obstack);
  gdl_set_node_label (node, label);
  
  for (i = 0; i < GET_RTX_LENGTH (code); i++)
    switch (*format_ptr++)
      {
      case 'T':
        str = XTMPL (x, i);
        goto string;
      case 'S':
      case 's':
        str = XSTR (x, i);
      string:
        if (str == 0)
          str = "";
        node_x = gdl_new_graph_node (graph, NULL);
        gdl_set_node_label (node_x, str);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));
        break;

      case '0':
        

      case 'i':
        value = XINT (x, i);
        node_x = gdl_new_graph_node (graph, NULL);
        sprintf (buf, "i\n");
        obstack_grow (&str_obstack, buf, strlen (buf));
        sprintf (buf, "----------\n");
        obstack_grow (&str_obstack, buf, strlen (buf));
        sprintf (buf, "%d", value);
        obstack_grow (&str_obstack, buf, strlen (buf));
        label = obstack_finish (&str_obstack);
        gdl_set_node_label (node_x, label);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));

        break;
      }
}

/* Dump rtx X into the file FNAME.  */

static void
dump_rtx_to_file (char *fname, const_rtx x)
{
  gdl_graph *graph;

  obstack_init (&str_obstack);
  tmp_stream = open_memstream (&tmp_buf, &tmp_buf_size);

  graph = vcg_plugin_common.top_graph;
  create_rtx_node (graph, x, 1);
  vcg_plugin_common.dump (fname, graph);

  obstack_free (&str_obstack, NULL);
  fclose (tmp_stream);
  free (tmp_buf);
}

/* Public function to dump a gcc rtx X.  */

void
vcg_plugin_dump_rtx (const_rtx x)
{
  char *fname;

  vcg_plugin_common.init ();

  /* Create the dump file name.  */
  asprintf (&fname, "dump-rtx-%#x.vcg", x);
  vcg_plugin_common.tag (fname);
  dump_rtx_to_file (fname, x);

  vcg_plugin_common.finish ();
}

/* Public function to view a gcc rtx X.  */

void
vcg_plugin_view_rtx (const_rtx x)
{
  char *fname;

  vcg_plugin_common.init ();

  /* Get the temp file name.  */
  fname = vcg_plugin_common.temp_file_name;
  dump_rtx_to_file (fname, x);
  vcg_plugin_common.show (fname);

  vcg_plugin_common.finish ();
}

