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


static void dump_tree_base (struct tree_base *s, int id, char *name);

static void dump_tree_common (struct tree_common *s, int id, char *name);

static void dump_tree_int_cst (struct tree_int_cst *s, int id, char *name);

static void dump_tree_real_cst (struct tree_real_cst *s, int id, char *name);

static void dump_tree_fixed_cst (struct tree_fixed_cst *s, int id, char *name);

static void dump_tree_vector (struct tree_vector *s, int id, char *name);

static void dump_tree_string (struct tree_string *s, int id, char *name);

static void dump_tree_complex (struct tree_complex *s, int id, char *name);

static void dump_tree_identifier (struct tree_identifier *s, int id, char *name);

static void dump_tree_decl_minimal (struct tree_decl_minimal *s, int id, char *name);

static void dump_tree_decl_common (struct tree_decl_common *s, int id, char *name);

static void dump_tree_decl_with_rtl (struct tree_decl_with_rtl *s, int id, char *name);

static void dump_tree_decl_non_common (struct tree_decl_non_common *s, int id, char *name);

static void dump_tree_parm_decl (struct tree_parm_decl *s, int id, char *name);

static void dump_tree_decl_with_vis (struct tree_decl_with_vis *s, int id, char *name);

static void dump_tree_var_decl (struct tree_var_decl *s, int id, char *name);

static void dump_tree_field_decl (struct tree_field_decl *s, int id, char *name);

static void dump_tree_label_decl (struct tree_label_decl *s, int id, char *name);

static void dump_tree_result_decl (struct tree_result_decl *s, int id, char *name);

static void dump_tree_const_decl (struct tree_const_decl *s, int id, char *name);

static void dump_tree_type_decl (struct tree_type_decl *s, int id, char *name);

static void dump_tree_function_decl (struct tree_function_decl *s, int id, char *name);

static void dump_tree_type (struct tree_type *s, int id, char *name);

static void dump_tree_list (struct tree_list *s, int id, char *name);

static void dump_tree_vec (struct tree_vec *s, int id, char *name);

static void dump_tree_exp (struct tree_exp *s, int id, char *name);

static void dump_tree_ssa_name (struct tree_ssa_name *s, int id, char *name);

static void dump_tree_block (struct tree_block *s, int id, char *name);

static void dump_tree_binfo (struct tree_binfo *s, int id, char *name);

static void dump_tree_statement_list (struct tree_statement_list *s, int id, char *name);

static void dump_tree_constructor (struct tree_constructor *s, int id, char *name);

static void dump_tree_omp_clause (struct tree_omp_clause *s, int id, char *name);

static void dump_tree_optimization_option (struct tree_optimization_option *s, int id, char *name);

static void dump_tree_target_option (struct tree_target_option *s, int id, char *name);

static void dump_tree (tree t, int id, char *name);

