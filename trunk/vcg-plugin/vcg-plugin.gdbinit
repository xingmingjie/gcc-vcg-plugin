# Define vcg plugin commands used in gdb. #

define dump-cgraph
  call vcg_plugin_dump_cgraph ()
end
document dump-cgraph
Usage: dump-cgraph
Dump the call graph.
end

define dump-function
  call vcg_plugin_dump_function ($arg0)
end
document dump-function
Usage: dump-function <function decl tree>
Dump the control flow graph of FUNCTION_DECL_TREE.
end

define dump-gimple-hierarchy
  call vcg_plugin_dump_gimple_hierarchy ()
end
document dump-gimple-hierarchy
Usage: dump-gimple-hierarchy
Dump the graph of gcc gimple statement structure hierarchy.
end

define dump-passes
  call vcg_plugin_dump_passes ()
end
document dump-passes
Usage: dump-passes
Dump the graph of gcc passes.
end

define dump-tree
  call vcg_plugin_dump_tree ($arg0)
end
document dump-tree
Usage: dump-tree TREE
Dump the graph of TREE.
end

define dump-tree-hierarchy
  call vcg_plugin_dump_tree_hierarchy ()
end
document dump-tree-hierarchy
Usage: dump-tree-hierarchy
Dump the graph of gcc tree structure hierarchy.
end

define view-cgraph
  call vcg_plugin_view_cgraph ()
end
document view-cgraph
Usage: view-cgraph
View the call graph.
end

define view-function
  call vcg_plugin_view_function ($arg0)
end
document view-function
Usage: view-function FUNCTION_DECL_TREE
View the control flow graph of FUNCTION_DECL_TREE.
end

define view-gimple-hierarchy
  call vcg_plugin_view_gimple_hierarchy ()
end
document view-gimple-hierarchy
Usage: view-gimple-hierarchy
View the graph of gcc gimple statement structure hierarchy.
end

define view-passes
  call vcg_plugin_view_passes ()
end
document view-passes
Usage: view-passes
View the graph of gcc passes.
end

define view-tree
  call vcg_plugin_view_tree ($arg0)
end
document view-tree
Usage: view-tree TREE
View the graph of TREE.
end

define view-tree-hierarchy
  call vcg_plugin_view_tree_hierarchy ()
end
document view-tree-hierarchy
Usage: view-tree-hierarchy
View the graph of gcc tree structure hierarchy.
end

