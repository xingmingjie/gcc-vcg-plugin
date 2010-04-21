# Define vcg plugin commands used in gdb. #

define view_cfg
  call vcg_plugin_view_cfg()
end
document view_cfg
Usage: view_cfg
Draws the control flow graph of the current function.
end

define view_tree
  call vcg_plugin_view_tree($arg0)
end
document view_tree
Usage: view_tree <tree>
Draws the graph of tree.
end

