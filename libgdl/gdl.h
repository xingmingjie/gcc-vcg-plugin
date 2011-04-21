/* The structures according to graph description language (gdl).

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

#ifndef GDL_H
#define GDL_H

#include <assert.h>

/* shape */

#define GDL_BOX			"box"
#define GDL_RHOMB		"rhomb"
#define GDL_ELLIPSE		"ellipse"
#define GDL_TRIANGLE		"triangle"

/* color */

#define GDL_BLACK		"black"
#define GDL_BLUE		"blue"
#define GDL_LIGHTBLUE		"lightblue"
#define GDL_RED			"red"
#define GDL_GREEN		"green"
#define GDL_YELLOW		"yellow"
#define GDL_WHITE		"white"
#define GDL_LIGHTGREY		"lightgrey"

/* line style */

#define GDL_CONTINUOUS		"continuous"
#define GDL_DASHED		"dashed"
#define GDL_DOTTED		"dotted"
#define GDL_INVISIBLE		"invisible"

/* layout algorithm */

#define GDL_MAX_DEPTH		"max_depth"
#define GDL_TREE		"tree"

#define DEF_ATTR(id, name, type, code, value) GDL_NODE_ATTR_##id,
enum gdl_node_attr
{
  #include "node-attr.def"
  GDL_NODE_ATTR_MAX
};
#undef DEF_ATTR

#define DEF_ATTR(id, name, type, code, value) GDL_EDGE_ATTR_##id,
enum
{
  #include "edge-attr.def"
  GDL_EDGE_ATTR_MAX
};
#undef DEF_ATTR

#define DEF_ATTR(id, name, type, code, value) GDL_GRAPH_ATTR_##id,
enum
{
  #include "graph-attr.def"
  GDL_GRAPH_ATTR_MAX
};
#undef DEF_ATTR

/* node, edge, graph, attribute */

enum
{
  GDL_CODE_STR,
  GDL_CODE_STR_QUOTE,
  GDL_CODE_INT
};

union gdl_attr_value
{
  int val;
  char *str;
};

typedef struct
{
  int code;
  char *name;
  union gdl_attr_value value;
} gdl_attr;


/* Make colorentry as a common global variable.  */
char *gdl_colorentry[256][3];

typedef struct gdl_node gdl_node;
typedef struct gdl_edge gdl_edge;
typedef struct gdl_graph gdl_graph;

struct gdl_node 
{
  char *bordercolor;
  int borderwidth;
  char *color;
  char *label;
  char *title;
  int vertical_order;
  /* The value is 1 if the attribute is set.  */
  int set_p[GDL_NODE_ATTR_MAX];

  gdl_node *next;
  /* The graph who it belongs to.  */
  gdl_graph *parent;
};

typedef enum
{
  GDL_EDGE,
  GDL_BACKEDGE,
  GDL_NEAREDGE,
  GDL_LEFTNEAREDGE,
  GDL_RIGHTNEAREDGE,
  GDL_BENTNEAREDGE,
  GDL_LEFTBENTNEAREDGE,
  GDL_RIGHTBENTNEAREDGE 
} gdl_edge_type;

struct gdl_edge
{
  char *label;
  char *linestyle;
  char *sourcename;
  char *targetname;
  int thickness;

  gdl_edge_type type;

  /* The value is 1 if the attribute is set.  */
  int set_p[GDL_EDGE_ATTR_MAX];

  gdl_edge *next;
};

struct gdl_graph
{
  char *color;
  int folding;
  char *label;
  char *layout_algorithm;
  char *near_edges;
  char *orientation;
  char *port_sharing;
  char *shape;
  char *splines;
  char *title;
  int vertical_order;
  int xspace;
  int yspace;

  int node_borderwidth;
  char *node_color;
  char *node_shape;

  char *edge_color;
  int edge_thickness;

  /* The value is 1 if the attribute is set.  */
  int set_p[GDL_GRAPH_ATTR_MAX];

  /* nodes or subgraphs */
  gdl_node *node;
  gdl_node *last_node;
  gdl_graph *subgraph;
  gdl_graph *last_subgraph;
  gdl_edge *edge;
  gdl_edge *last_edge;
  gdl_graph *next;
  /* The graph who it belongs to.  */
  gdl_graph *parent;
};

extern gdl_graph *gdl_new_graph (const char *title); 
extern gdl_node *gdl_new_node (const char *title);
extern gdl_edge *gdl_new_edge (const char *source, const char *target);
extern gdl_graph *gdl_new_graph (const char *title); 

extern void gdl_free_node (gdl_node *node);
extern void gdl_free_edge (gdl_edge *edge);
extern void gdl_free_graph (gdl_graph *graph);

extern void gdl_add_node (gdl_graph *graph, gdl_node *node); 
extern void gdl_add_edge (gdl_graph *graph, gdl_edge *edge);
extern void gdl_add_subgraph (gdl_graph *graph, gdl_graph *subgraph);

extern gdl_node *gdl_find_node (gdl_graph *graph, char *title);
extern gdl_edge *gdl_find_edge (gdl_graph *graph, char *source, char *target);
extern gdl_graph *gdl_find_subgraph (gdl_graph *graph, char *title);

extern void gdl_dump_node (FILE *fout, gdl_node *node);
extern void gdl_dump_edge (FILE *fout, gdl_edge *edge);
extern void gdl_dump_graph (FILE *fout, gdl_graph *graph);

#endif
