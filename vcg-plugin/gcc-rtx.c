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
#include <stdarg.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "vcg-plugin.h"

/* Temp file stream, used to get the rtx dump. */
static FILE *tmp_stream;
static char *tmp_buf;
static size_t tmp_buf_size;

static gdl_node *
create_rtx_node (gdl_graph *graph, const_rtx x)
{
  gdl_node *node, *node_x;
  char *label;
  enum rtx_code code;
  const char *format_ptr;
  int i;
  const char *str;
  int value;
  rtx sub;
  enum rtx_code subc;

  if (x == 0)
    return;

  node = gdl_new_graph_node (graph, NULL);

  code = GET_CODE (x);
  format_ptr = GET_RTX_FORMAT (code);
  
  rewind (tmp_stream);
  print_rtl_single (tmp_stream, x);
  fflush(tmp_stream);
  vcg_plugin_common.buf_print ("%s", tmp_buf);
  vcg_plugin_common.buf_print ("----------\n");
  vcg_plugin_common.buf_print ("addr: 0x%x\n", (unsigned) x);
  vcg_plugin_common.buf_print ("format: %s", format_ptr);
  label = vcg_plugin_common.buf_finish ();
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
        node_x = gdl_new_graph_node (graph, NULL);
        if (str == 0)
          str = "";
        gdl_set_node_label (node_x, str);
        gdl_set_node_horizontal_order (node_x, i + 1);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));
        break;

      case '0':
        break;

      case 'B':
        node_x = gdl_new_graph_node (graph, NULL);
        vcg_plugin_common.buf_print ("B\n");
        vcg_plugin_common.buf_print ("----------\n");
        if (XBBDEF (x, i))
          vcg_plugin_common.buf_print ("%i", XBBDEF (x, i)->index);

        label = vcg_plugin_common.buf_finish ();
        gdl_set_node_label (node_x, label);
        gdl_set_node_horizontal_order (node_x, i + 1);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));

        break;

      case 'i':
        node_x = gdl_new_graph_node (graph, NULL);
        value = XINT (x, i);
        vcg_plugin_common.buf_print ("i\n");
        vcg_plugin_common.buf_print ("----------\n");
        vcg_plugin_common.buf_print ("%d", value);
        label = vcg_plugin_common.buf_finish ();
        gdl_set_node_label (node_x, label);
        gdl_set_node_horizontal_order (node_x, i + 1);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));
        break;

      case 'e':
      do_e:
        node_x = create_rtx_node (graph, XEXP (x, i));
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));
        break;

      case 'n':
        node_x = gdl_new_graph_node (graph, NULL);
        vcg_plugin_common.buf_print ("n\n");
        vcg_plugin_common.buf_print ("----------\n");
        vcg_plugin_common.buf_print ("%s", GET_NOTE_INSN_NAME (XINT (x, i)));
        label = vcg_plugin_common.buf_finish ();
        gdl_set_node_label (node_x, label);
        gdl_set_node_horizontal_order (node_x, i + 1);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));
        
        break;

      case 'u':
        node_x = gdl_new_graph_node (graph, NULL);
        sub = XEXP (x, i);
        vcg_plugin_common.buf_print ("u\n");
        vcg_plugin_common.buf_print ("----------\n");
        vcg_plugin_common.buf_print ("addr: 0x%x\n", (unsigned) sub);
        if (sub)
          {
            subc = GET_CODE (sub);
            if (code == LABEL_REF)
              {
                if (subc == NOTE
                    && NOTE_KIND (sub) == NOTE_INSN_DELETED_LABEL)
                  {
                    vcg_plugin_common.buf_print ("[%d deleted]\n", INSN_UID (sub));

        //        if (subc != CODE_LABEL)
        //          goto do_e;
                  }
              }

            vcg_plugin_common.buf_print ("id: %d\n", INSN_UID (sub));
          }

        label = vcg_plugin_common.buf_finish ();
        gdl_set_node_label (node_x, label);
        gdl_set_node_horizontal_order (node_x, i + 1);
        gdl_new_graph_edge (graph, gdl_get_node_title (node),
                            gdl_get_node_title (node_x));
        break;

      }

  return node;
}

/* Dump rtx X into the file FNAME.  */

static void
dump_rtx_to_file (char *fname, const_rtx x)
{
  gdl_graph *graph;

  tmp_stream = open_memstream (&tmp_buf, &tmp_buf_size);

  graph = vcg_plugin_common.top_graph;
  create_rtx_node (graph, x);
  vcg_plugin_common.dump (fname, graph);

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

