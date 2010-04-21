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
#include "rtl.h"
#include "intl.h"
#include "langhooks.h"
#include "cfghooks.h"
#include "version.h" /* for printing gcc version number in graph */

#ifdef TMP_VCG
  #undef TMP_VCG
#endif

#define TMP_VCG "tmp-vcg-plugin-cfg.vcg"

#include "tree.h"

#undef DEFTREESTRUCT
#define DEFTREESTRUCT(ENUM, NAME) NAME,
static char *tsname[] =
{
  #include "treestruct.def"
  NULL
};

static FILE *fout;

static int title_id = 0;

#define PRINT_GRAPH_BEGIN(name, value, id) \
do {\
  fprintf (fout, "\n\
graph: {\n\
  title: \"g%d\"\n\
  label: \"%s: %s\"\n\
  folding: 1\n\
\n", (id), (name), (value));\
} while (0); 

#define PRINT_NODE(name, value, id) \
do {\
  fprintf (fout, "\n\
node: {\n\
  title: \"%d\"\n\
  label: \"%s: %s\"\n\
}\n", (id), (name), (value));\
} while (0); 


#include "view-tree.h"

static void
dump_tree_base (struct tree_base *s, int id, char *name)
{
  PRINT_GRAPH_BEGIN(name, "tree_base", id)
  PRINT_NODE(name, "tree_base", id)

  fprintf (fout, "}\n");
}

static void
dump_tree_common (struct tree_common *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_common", id)
  PRINT_NODE(name, "tree_common", id)

  dump_tree_base (&s->base, id1, "base");
  dump_tree (s->chain, id2, "chain");
  dump_tree (s->type, id3, "type");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);

  fprintf (fout, "}\n");
}

#define PRINT_INT_VALUE(v, id) \
do {\
  fprintf (fout, "\n\
node: {\n\
  title: \"%d\"\n\
  label: \"value: %d\"\n\
}\n", (id), (v));\
} while (0); 

static void
dump_tree_int_cst (struct tree_int_cst *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_int_cst", id)
  PRINT_NODE(name, "tree_int_cst", id)

  dump_tree_common (&s->common, id1, "common");
  PRINT_INT_VALUE (s->int_cst, id2);

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"%d\"}\n", id, id2);

  fprintf (fout, "}\n");
}

#define PRINT_REAL_VALUE(v, id) \
do {\
  fprintf (fout, "\n\
node: {\n\
  title: \"%d\"\n\
  label: \"value: %d\"\n\
}\n", (id), (v));\
} while (0); 

static void
dump_tree_real_cst (struct tree_real_cst *s, int id, char *name)
{
  int id1 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_real_cst", id)
  PRINT_NODE(name, "tree_real_cst", id)

  dump_tree_common (&s->common, id1, "common");
  //PRINT_REAL_VALUE (s.real_cst_ptr, id2, "real_cst_ptr");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  //fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"%d\"}\n", id, id2);

  fprintf (fout, "}\n");
}

static void
dump_tree_fixed_cst (struct tree_fixed_cst *s, int id, char *name)
{
  int id1 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_fixed_cst", id)
  PRINT_NODE(name, "tree_fixed_cst", id)

  dump_tree_common (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_vector (struct tree_vector *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_vector", id)
  PRINT_NODE(name, "tree_vector", id)

  dump_tree_common (&s->common, id1, "common");
  dump_tree (s->elements, id2, "elements");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);

  fprintf (fout, "}\n");
}

static void
dump_tree_string (struct tree_string *s, int id, char *name)
{
  int id1 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_string", id)
  PRINT_NODE(name, "tree_string", id)

  dump_tree_common (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_complex (struct tree_complex *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_complex", id)
  PRINT_NODE(name, "tree_complex", id)

  dump_tree_common (&s->common, id1, "common");
  dump_tree (s->real, id2, "real");
  dump_tree (s->imag, id3, "imag");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);

  fprintf (fout, "}\n");
}

#define PRINT_IDENTIFIER(v, id) \
do {\
  fprintf (fout, "\n\
node: {\n\
  title: \"%d\"\n\
  label: \"value: %s\"\n\
}\n", (id), (v));\
} while (0); 

static void
dump_tree_identifier (struct tree_identifier *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_identifier", id)
  PRINT_NODE(name, "tree_identifier", id)

  dump_tree_common (&s->common, id1, "common");
  PRINT_IDENTIFIER(s->id.str, id2)

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"%d\"}\n", id, id2);

  fprintf (fout, "}\n");
}

static void
dump_tree_decl_minimal (struct tree_decl_minimal *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_decl_minimal", id)
  PRINT_NODE(name, "tree_decl_minimal", id)

  dump_tree_common (&s->common, id1, "common");
  dump_tree (s->name, id2, "name");
  //dump_tree (s.context, id3, "context");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  //fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);

  fprintf (fout, "}\n");
}

static void
dump_tree_decl_common (struct tree_decl_common *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;
  int id4 = title_id++;
  int id5 = title_id++;
  int id6 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_decl_common", id)
  PRINT_NODE(name, "tree_decl_common", id)

  dump_tree_decl_minimal (&s->common, id1, "common");
  dump_tree (s->size, id2, "size");
  dump_tree (s->size_unit, id3, "size_unit");
  dump_tree (s->initial, id4, "initial");
  dump_tree (s->attributes, id5, "attributes");
  dump_tree (s->abstract_origin, id6, "abstract_origin");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id4);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id5);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id6);


  fprintf (fout, "}\n");
}

static void
dump_tree_decl_with_rtl (struct tree_decl_with_rtl *s, int id, char *name)
{
  int id1 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_decl_with_rtl", id)
  PRINT_NODE(name, "tree_decl_with_rtl", id)

  dump_tree_decl_common (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_decl_non_common (struct tree_decl_non_common *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;
  int id4 = title_id++;
  int id5 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_decl_non_common", id)
  PRINT_NODE(name, "tree_decl_non_common", id)

  dump_tree_decl_with_vis (&s->common, id1, "common");
  dump_tree (s->saved_tree, id2, "saved_tree");
  dump_tree (s->arguments, id3, "arguments");
  dump_tree (s->result, id4, "result");
  dump_tree (s->vindex, id5, "vindex");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id4);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id5);

  fprintf (fout, "}\n");
}

static void
dump_tree_parm_decl (struct tree_parm_decl *s, int id, char *name)
{
  int id1 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_parm_decl", id)
  PRINT_NODE(name, "tree_parm_decl", id)

  dump_tree_decl_with_rtl (&s->common, id1, "common");
  
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_decl_with_vis (struct tree_decl_with_vis *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;
  int id4 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_decl_with_vis", id)
  PRINT_NODE(name, "tree_decl_with_vis", id)

  dump_tree_decl_with_rtl (&s->common, id1, "common");
  dump_tree (s->assembler_name, id2, "assembler_name");
  dump_tree (s->section_name, id3, "section_name");
  dump_tree (s->comdat_group, id4, "comdat_group");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id4);

  fprintf (fout, "}\n");
}

static void
dump_tree_var_decl (struct tree_var_decl *s, int id, char *name)
{
  int id1 = title_id++;
  
  PRINT_GRAPH_BEGIN(name, "tree_var_decl", id)
  PRINT_NODE(name, "tree_var_decl", id)

  dump_tree_decl_with_vis (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_field_decl (struct tree_field_decl *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_field_decl", id)
  PRINT_NODE(name, "tree_field_decl", id)

  fprintf (fout, "}\n");
}

static void
dump_tree_label_decl (struct tree_label_decl *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_label_decl", id)
  PRINT_NODE(name, "tree_label_decl", id)

  fprintf (fout, "}\n");
}

static void
dump_tree_result_decl (struct tree_result_decl *s, int id, char *name)
{
  int id1 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_result_decl", id)
  PRINT_NODE(name, "tree_result_decl", id)

  dump_tree_decl_with_rtl (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_const_decl (struct tree_const_decl *s, int id, char *name)
{
  PRINT_GRAPH_BEGIN(name, "tree_const_decl", id)
  PRINT_NODE(name, "tree_const_decl", id)

  fprintf (fout, "}\n");
}

static void
dump_tree_type_decl (struct tree_type_decl *s, int id, char *name)
{
  PRINT_GRAPH_BEGIN(name, "tree_type_decl", id)
  PRINT_NODE(name, "tree_type_decl", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_function_decl (struct tree_function_decl *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;

  char *value;

  value = tree_code_name[((struct tree_base *)s)->code];

  PRINT_GRAPH_BEGIN(name, value, id)
  PRINT_NODE(name, value, id)

  dump_tree_decl_non_common (&s->common, id1, "common");
  dump_tree (s->function_specific_target, id2, "function_specific_target");
  dump_tree (s->function_specific_optimization, id3, "function_specific_optimization");
  
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);

  fprintf (fout, "}\n");
}

static void
dump_tree_type (struct tree_type *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_type", id)
  PRINT_NODE(name, "tree_type", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_list (struct tree_list *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;
  int id3 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_list", id)
  PRINT_NODE(name, "tree_list", id)

  dump_tree_common (&s->common, id1, "common");
  dump_tree (s->purpose, id2, "purpose");
  dump_tree (s->value, id3, "value");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id3);

  fprintf (fout, "}\n");
}

static void
dump_tree_vec (struct tree_vec *s, int id, char *name)
{
  int id1 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_vec", id)
  PRINT_NODE(name, "tree_vec", id)

  dump_tree_common (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_exp (struct tree_exp *s, int id, char *name)
{
  int id1 = title_id++;
  int id2 = title_id++;

  int i;
  int idx;
  int len;

  char *value;
  char buf[16];  

  value = tree_code_name[((struct tree_base *)s)->code];
  len = (int) tree_code_length[((struct tree_base *)s)->code];

  PRINT_GRAPH_BEGIN(name, value, id)
  PRINT_NODE(name, value, id)

  dump_tree_common (&s->common, id1, "common");
  dump_tree (s->block, id2, "block");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);
  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id2);

  for (i = 0; i < len; i++)
    {
      idx = title_id++;
      sprintf (buf, "operand[%d]", i);
      dump_tree (s->operands[i], idx, buf);
      fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, idx);
    }

  fprintf (fout, "}\n");
}

static void
dump_tree_ssa_name (struct tree_ssa_name *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_ssa_name", id)
  PRINT_NODE(name, "tree_ssa_name", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_block (struct tree_block *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_block", id)
  PRINT_NODE(name, "tree_block", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_binfo (struct tree_binfo *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_binfo", id)
  PRINT_NODE(name, "tree_binfo", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_statement_list (struct tree_statement_list *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_statement_list", id)
  PRINT_NODE(name, "tree_statement_list", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_constructor (struct tree_constructor *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_constructor", id)
  PRINT_NODE(name, "tree_constructor", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_omp_clause (struct tree_omp_clause *s, int id, char *name)
{

  PRINT_GRAPH_BEGIN(name, "tree_omp_clause", id)
  PRINT_NODE(name, "tree_omp_clause", id)


  fprintf (fout, "}\n");
}

static void
dump_tree_optimization_option (struct tree_optimization_option *s, int id, char *name)
{
  int id1 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_optimization_option", id)
  PRINT_NODE(name, "tree_optimization_option", id)

  dump_tree_common (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree_target_option (struct tree_target_option *s, int id, char *name)
{
  int id1 = title_id++;

  PRINT_GRAPH_BEGIN(name, "tree_target_option", id)
  PRINT_NODE(name, "tree_target_option", id)

  dump_tree_common (&s->common, id1, "common");

  fprintf (fout, "edge: {sourcename: \"%d\" targetname: \"g%d\"}\n", id, id1);

  fprintf (fout, "}\n");
}

static void
dump_tree (tree t, int id, char *name)
{  
  enum tree_node_structure_enum tns;

  if (t == 0)
    {
      PRINT_GRAPH_BEGIN(name, "NULL", id)
      
      fprintf (fout, "}\n");
      return;
    }

  tns = tree_node_structure (t);

  switch (tns)
    {
    case TS_BASE:
      dump_tree_base (&t->base, id, name);
      break;
      
    case TS_COMMON:
      dump_tree_common (&t->common, id, name);
      break;
      
    case TS_INT_CST:
      dump_tree_int_cst (&t->int_cst, id, name);
      break;
      
    case TS_REAL_CST:
      dump_tree_real_cst (&t->real_cst, id, name);
      break;
      
    case TS_FIXED_CST:
      dump_tree_fixed_cst (&t->fixed_cst, id, name);
      break;
      
    case TS_VECTOR:
      dump_tree_vector (&t->vector, id, name);
      break;
      
    case TS_STRING:
      dump_tree_string (&t->string, id, name);
      break;
      
    case TS_COMPLEX:
      dump_tree_complex (&t->complex, id, name);
      break;
      
    case TS_IDENTIFIER:
      dump_tree_identifier (&t->identifier, id, name);
      break;
      
    case TS_DECL_MINIMAL:
      dump_tree_decl_minimal (&t->decl_minimal, id, name);
      break;
      
    case TS_DECL_COMMON:
      dump_tree_decl_common (&t->decl_common, id, name);
      break;
      
    case TS_DECL_WRTL:
      dump_tree_decl_with_rtl (&t->decl_with_rtl, id, name);
      break;
      
    case TS_DECL_NON_COMMON:
      dump_tree_decl_non_common (&t->decl_non_common, id, name);
      break;
      
    case TS_DECL_WITH_VIS:
      dump_tree_decl_with_vis (&t->decl_with_vis, id, name);
      break;
      
    case TS_FIELD_DECL:
      dump_tree_field_decl (&t->field_decl, id, name);
      break;
      
    case TS_VAR_DECL:
      dump_tree_var_decl (&t->var_decl, id, name);
      break;
      
    case TS_PARM_DECL:
      dump_tree_parm_decl (&t->parm_decl, id, name);
      break;
      
    case TS_LABEL_DECL:
      dump_tree_label_decl (&t->label_decl, id, name);
      break;
      
    case TS_RESULT_DECL:
      dump_tree_result_decl (&t->result_decl, id, name);
      break;
      
    case TS_CONST_DECL:
      dump_tree_const_decl (&t->const_decl, id, name);
      break;
      
    case TS_TYPE_DECL:
      dump_tree_type_decl (&t->type_decl, id, name);
      break;
      
    case TS_FUNCTION_DECL:
      dump_tree_function_decl (&t->function_decl, id, name);
      break;
      
    case TS_TYPE:
      dump_tree_type (&t->type, id, name);
      break;
      
    case TS_LIST:
      dump_tree_list (&t->list, id, name);
      break;
      
    case TS_VEC:
      dump_tree_vec (&t->vec, id, name);
      break;
      
    case TS_EXP:
      dump_tree_exp (&t->exp, id, name);
      break;
      
    case TS_SSA_NAME:
      dump_tree_ssa_name (&t->ssa_name, id, name);
      break;
      
    case TS_BLOCK:
      dump_tree_block (&t->block, id, name);
      break;
      
    case TS_BINFO:
      dump_tree_binfo (&t->binfo, id, name);
      break;
      
    case TS_STATEMENT_LIST:
      dump_tree_statement_list (&t->stmt_list, id, name);
      break;
      
    case TS_CONSTRUCTOR:
      dump_tree_constructor (&t->constructor, id, name);
      break;
      
    case TS_OMP_CLAUSE:
      dump_tree_omp_clause (&t->omp_clause, id, name);
      break;
      
    case TS_OPTIMIZATION:
      dump_tree_optimization_option (&t->optimization, id, name);
      break;
      
    case TS_TARGET_OPTION:
      dump_tree_target_option (&t->target_option, id, name);
      break;
    }
}

void
vcg_plugin_view_tree (tree t)
{
  int id = title_id++;

  fout = fopen (TMP_VCG, "w");

  //PRINT_NODE(name, tsname[tns], id)
  do {
  fprintf (fout, "\n\
graph: {\n\
  title: \"g%d\"\n\
  label: \"%s: %s\"\n\
\n", (id), ("tree"), ("tree"));\
} while (0); 



  dump_tree (t, id, "tree");

  fprintf (fout, "}\n");

  fclose (fout);

  vcg_plugin_view (TMP_VCG);
}

