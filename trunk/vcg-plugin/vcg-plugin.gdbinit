# Define vcg plugin commands used in gdb. #

define view_function
  call vcg_plugin_view_function($arg0)
end
document view_function
Usage: view_function <function_decl_tree>
Draws the control flow graph of the function.
end

define dump_function
  call vcg_plugin_dump_function($arg0)
end
document dump_function
Usage: dump_function <function_decl_tree>
Dump the control flow graph of the function.
end

define view_tree
  call vcg_plugin_view_tree($arg0)
end
document view_tree
Usage: view_tree <tree>
Draws the graph of tree.
end

