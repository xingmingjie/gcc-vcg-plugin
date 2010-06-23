/* <one line to give the program's name and a bief idea of what it does.>

   Copyright (C) 2010 Eric Fisher, joefoxreal@gmail.com.

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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "gcc-plugin.h"
#include "plugin.h"
#include "plugin-version.h"

#include "vcg-plugin.h"

#ifdef TMP_VCG
  #undef TMP_VCG
#endif

#define TMP_VCG "tmp-vcg-plugin-cfg.vcg"

extern int graph_dump_format;
extern int dump_for_graph;

static const char *const graph_ext[] =
{
  /* no_graph */ "",
  /* vcg */      ".vcg",
};

static void start_fct (FILE *);
static void start_bb (FILE *, int);
static void node_data (FILE *, rtx);
static void draw_edge (FILE *, int, int, int, int);
static void end_fct (FILE *);
static void end_bb (FILE *);

/* debug: no basic blocks in graph if set, skipping start_bb() and end_bb() */
static int nobb = 0;

/* debug: no nodes in graph if set */
static int nonodes = 0;

/* debug: no edges in graph if set */
static int noedges = 0;

/* Output text for new basic block.  */
static void
start_fct (FILE *fp)
{
  fprintf (fp, "  /* start function %s */\n", current_function_name ());
  fprintf (fp, "\
  graph: {\n\
    title: \"%s\"\n\
    folding: 1\n\
    hidden: 2\n\
    node: { title: \"%s.0\" }\n",
           current_function_name (), current_function_name ());
}

static void
start_bb (FILE *fp, int bb)
{
  fprintf (fp, "    /* start basic block %d */\n",bb);
  fprintf (fp, "\
    graph: {\n\
      title: \"%s.BB%d\"\n\
      folding: 1\n\
      color: lightblue\n\
      label: \"basic block %d",
           current_function_name (), bb, bb);

  /* terminate label: statement */
  fputs ("\"\n\n", fp);
}

static void
node_data (FILE *fp, rtx tmp_rtx)
{
  if (PREV_INSN (tmp_rtx) == 0)
    {
      /* This is the first instruction.  Add an edge from the starting
	 block.  */
      if (noedges)
        {
          /* no node data */
        }
      else
        {
          fprintf (fp, "\
      edge: { sourcename: \"%s.0\" targetname: \"%s.%d\" }\n",
                   current_function_name (),
                   current_function_name (), XINT (tmp_rtx, 0));
        }
    }

  if (nonodes)
    {
      /* no node data */
    }
  else 
    {
      fprintf (fp, "      node: {\n\
        title: \"%s.%d\"\n\
        color: %s\n\
        label: \"%s %d\n",
           current_function_name (), XINT (tmp_rtx, 0),
           NOTE_P (tmp_rtx) ? "lightgrey"
           : NONJUMP_INSN_P (tmp_rtx) ? "green"
           : JUMP_P (tmp_rtx) ? "darkgreen"
           : CALL_P (tmp_rtx) ? "darkgreen"
           : LABEL_P (tmp_rtx) ?  "darkgrey\n\
        shape: ellipse" : "white",
           GET_RTX_NAME (GET_CODE (tmp_rtx)), XINT (tmp_rtx, 0));
        }

  if (nonodes)
    {
      /* no node data */
    }
  else 
    {
      /* Print the RTL.  */
      if (NOTE_P (tmp_rtx))
        {
          const char *name;
          name =  GET_NOTE_INSN_NAME (NOTE_KIND (tmp_rtx));
          fprintf (fp, " %s", name);
        }
      else if (INSN_P (tmp_rtx))
        print_rtl_single (fp, PATTERN (tmp_rtx));
      else
        print_rtl_single (fp, tmp_rtx);
    }
  
  /* finish label: statement */
  if (nonodes)
    {
      /* */
    }
  else
    {
      fputs ("\"\n      }\n", fp);
    }
}

static void
draw_edge (FILE *fp, int from, int to, int bb_edge, int color_class)
{
  const char * color;
  if (noedges)
    {
      /* no edge data */
      return;
    }
  color = "";
  if (color_class == 2)
    color = "color: red ";
  else if (bb_edge)
    color = "color: blue ";
  else if (color_class == 3)
    color = "color: green ";
  else
    color = "";

  fprintf (fp, "\
      edge: { sourcename: \"%s.%d\" targetname: \"%s.%d\" %s",
           current_function_name (), from,
           current_function_name (), to, color);
  if (color_class)
    fprintf (fp, "class: %d ", color_class);
  fputs ("}\n", fp);
}

static void
end_bb (FILE *fp)
{
  fputs ("    } /* end of basic block */\n", fp);
}

static void
end_fct (FILE *fp)
{
      if (nonodes)
        {
          /* no node data */
          fprintf (fp, "  } /* end of function */\n");
        }
      else
        {
          fprintf (fp, "\
      node: { title: \"%s.999999\" label: \"END\" }\n\
  } /* end of function */\n",
	           current_function_name ());
        }
}

/* Like print_rtl, but also print out live information for the start of each
   basic block.  */
static void
print_rtl_graph_with_bb (const char *base, rtx rtx_first)
{
  rtx tmp_rtx;
  size_t namelen = strlen (base);
  size_t extlen = strlen (graph_ext[graph_dump_format]) + 1;
  char *buf = XALLOCAVEC (char, namelen + extlen);
  FILE *fp;
  int inbb = 0; /* indicator if inside a basic block */

  if (basic_block_info == NULL)
    return;

  memcpy (buf, base, namelen);
  memcpy (buf + namelen, graph_ext[graph_dump_format], extlen);

  fp = fopen (buf, "a");
  if (fp == NULL)
    fatal_error ("can't open %s: %m", buf);

  if (rtx_first == 0)
    fprintf (fp, "(nil)\n");
  else
    {
      enum bb_state { NOT_IN_BB, IN_ONE_BB, IN_MULTIPLE_BB };
      int max_uid = get_max_uid ();
      int *start = XNEWVEC (int, max_uid);
      int *end = XNEWVEC (int, max_uid);
      enum bb_state *in_bb_p = XNEWVEC (enum bb_state, max_uid);
      basic_block bb;
      int i;

      for (i = 0; i < max_uid; ++i)
	{
	  start[i] = end[i] = -1;
	  in_bb_p[i] = NOT_IN_BB;
	}

      FOR_EACH_BB_REVERSE (bb)
	{
	  rtx x;
	  start[INSN_UID (BB_HEAD (bb))] = bb->index;
	  end[INSN_UID (BB_END (bb))] = bb->index;
	  for (x = BB_HEAD (bb); x != NULL_RTX; x = NEXT_INSN (x))
	    {
	      in_bb_p[INSN_UID (x)]
		= (in_bb_p[INSN_UID (x)] == NOT_IN_BB)
		 ? IN_ONE_BB : IN_MULTIPLE_BB;
	      if (x == BB_END (bb))
		break;
	    }
	}

      /*  Tell print-rtl that we want graph output.  */
      dump_for_graph = 1;

      /* Start new function.  */
      start_fct (fp);

      for (tmp_rtx = NEXT_INSN (rtx_first); NULL != tmp_rtx;
	   tmp_rtx = NEXT_INSN (tmp_rtx))
	{
	  int edge_printed = 0;
	  rtx next_insn;

          next_insn = 0;

	  if (start[INSN_UID (tmp_rtx)] < 0 && end[INSN_UID (tmp_rtx)] < 0)
	    {
	      if (BARRIER_P (tmp_rtx))
		continue;
	      if (NOTE_P (tmp_rtx)
		  && (1 || in_bb_p[INSN_UID (tmp_rtx)] == NOT_IN_BB))
		continue;
	    }
          /* start basic block */
          i = start[INSN_UID (tmp_rtx)];
	  if (i >= 0)
	    {
	      /* We start a subgraph for each basic block.  */
	      if (nobb)
                {
                  fprintf(fp,"\
   /* skipped start basic block %d start_bb() */\n", i);
                }
              else
                {
	          inbb = 1; /* mark inside a basic block */
	          start_bb (fp, i);
                }
	      if (i == 0)
		draw_edge (fp, 0, INSN_UID (tmp_rtx), 1, 0);
	    }

	  /* Print the data for this node.  */
	  node_data (fp, tmp_rtx);
	  next_insn = next_nonnote_insn (tmp_rtx);
          i = end[INSN_UID (tmp_rtx)];
          if (i >= 0)
	    {
	      edge e;
	      edge_iterator ei;

	      bb = BASIC_BLOCK (i);

	      /* End of the basic block.  */
              if (nobb)
                {
                  fprintf (fp,"\
   /* skipped end basic block %d end_bb() */\n", i);
                }
              else
                {
                  if (inbb)
                    {
                      end_bb (fp);
                      inbb = 0; /* not in basic block anymore */
                    }
                  else
                    {
                      fprintf (fp, "\
/* end_bb() in graph.c called without start_bb() */\n");
                    }
                }
	      /* Now specify the edges to all the successors of this
		 basic block.  */
	      FOR_EACH_EDGE (e, ei, bb->succs)
		{
		  if (e->dest != EXIT_BLOCK_PTR)
		    {
		      rtx block_head = BB_HEAD (e->dest);

		      draw_edge (fp, INSN_UID (tmp_rtx),
				 INSN_UID (block_head),
				 next_insn != block_head,
				 (e->flags & EDGE_ABNORMAL ? 2 : 0));

		      if (block_head == next_insn)
			edge_printed = 1;
		    }
		  else
		    {
		      draw_edge (fp, INSN_UID (tmp_rtx), 999999,
				 next_insn != 0,
				 (e->flags & EDGE_ABNORMAL ? 2 : 0));

		      if (next_insn == 0)
			edge_printed = 1;
		    }
		}
	    }

	  if (!edge_printed)
	    {
	      /* Don't print edges to barriers.  */
	      if (next_insn == 0
		  || !BARRIER_P (next_insn))
		draw_edge (fp, XINT (tmp_rtx, 0),
			   next_insn ? INSN_UID (next_insn) : 999999, 0, 0);
	      else
		{
		  /* We draw the remaining edges in class 3.  We have
		     to skip over the barrier since these nodes are
		     not printed at all.  */
		  do
		    next_insn = NEXT_INSN (next_insn);
		  while (next_insn
			 && (NOTE_P (next_insn)
			     || BARRIER_P (next_insn)));

		  draw_edge (fp, XINT (tmp_rtx, 0),
			     next_insn ? INSN_UID (next_insn) : 999999, 0, 3);
		}
	    }
	}

      dump_for_graph = 0;

      end_fct (fp);

      /* Clean up.  */
      free (start);
      free (end);
      free (in_bb_p);
    }

  fclose (fp);
}


/* Similar as clean_dump_file, but this time for graph output files.  */
/* start of graph output file generation */
static void
clean_graph_dump_file (const char *base)
{
  size_t namelen = strlen (base);
  size_t extlen = strlen (graph_ext[graph_dump_format]) + 1;
  char *buf = XALLOCAVEC (char, namelen + extlen);
  FILE *fp;

  memcpy (buf, base, namelen);
  memcpy (buf + namelen, graph_ext[graph_dump_format], extlen);

  fp = fopen (buf, "w");

  if (fp == NULL)
    fatal_error ("can't open %s: %m", buf);

  fprintf (fp,"/* generated using gcc-%s */\n",version_string);
  if (nobb)
    {
      fprintf (fp,"\
/* nobb is set in graph.c skipping start_bb() and end_bb() */\n");
    }
  if (nonodes)
    {
      fprintf (fp,"/* nonodes is set in graph.c skipping node output */\n");
    }
  if (noedges)
    {
      fprintf (fp,"/* noedges is set in graph.c skipping edge output */\n");  
    }
  fputs ("graph: {\n  port_sharing: no\n", fp);

  fclose (fp);
}


/* Do final work on the graph output file.  */
static void
finish_graph_dump_file (const char *base)
{
  size_t namelen = strlen (base);
  size_t extlen = strlen (graph_ext[graph_dump_format]) + 1;
  char *buf = XALLOCAVEC (char, namelen + extlen);
  FILE *fp;

  memcpy (buf, base, namelen);
  memcpy (buf + namelen, graph_ext[graph_dump_format], extlen);

  fp = fopen (buf, "a");
  if (fp != NULL)
    {
      fputs ("}\n", fp);
      fclose (fp);
    }
  else 
    {
      fatal_error ("can't open %s: %m", buf);
    }
}

static void
dump_cfg_tree (char *filename)
{
  const char *dname, *aname;
  FILE *dump_file, *vcg_file;
  tree fn;

  fn = current_function_decl;
  if (fn == 0)
    {
      printf ("vcg-plugin: function decl is unavailable for now.\n");
      return;
    }

  dump_file = fopen ("tmp.dump", "w");
  vcg_file = fopen (TMP_VCG, "w");

  dname = lang_hooks.decl_printable_name (fn, 2);
  aname = (IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (fn)));
  fprintf (dump_file, "\n;; Function %s (%s)\n\n", dname, aname);
  
  dump_function_to_file (fn, dump_file, TDF_BLOCKS);

  fclose (dump_file);

  dump_file = fopen ("tmp.dump", "r");
  vcg_plugin_tree2vcg (dump_file, vcg_file);

  fclose (dump_file);
  fclose (vcg_file);
}

/* Create a graph from the function fn. */
gdl_graph *
vcg_plugin_function_graph (tree fn, int flag)
{
  gdl_graph *g;
  

  return g;
}

/* Dump the function fn into a file. */
void
vcg_plugin_dump_function (tree fn, int flag)
{
  gdl_graph *g;

  TRY
    g = vcg_plugin_function_graph (tree fn, int flag);
  ELSE
    g = 0;
  END_TRY

  init_dump ();
  gdl_dump_graph (vcg_plugin_dump_file, g);
  finish_dump ();
}

/* View the function fn in a graph. */
void
vcg_plugin_view_function (tree fn, int flag)
{
  vcg_plugin_dump_cfg (TMP_VCG);
  vcg_plugin_view (TMP_VCG);
}

#define TMP_VCG_BASE "tmp-vcg-plugin-cfg"
static void
dump_cfg_rtl (char *filename)
{
  graph_dump_format = 1;
  clean_graph_dump_file (TMP_VCG_BASE);
  print_rtl_graph_with_bb (TMP_VCG_BASE, get_insns());
  finish_graph_dump_file (TMP_VCG_BASE);
}

