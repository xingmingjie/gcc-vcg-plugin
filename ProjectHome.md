A gcc plugin, which can be loaded when debugging gcc, to show internal structures graphically. Available commands include,

  * view-bb ---- show the basic block
  * view-bbs ---- show the basic blocks
  * view-cgraph ---- show the current call graph
  * view-dominance ---- show the current dominance graph
  * view-function ---- show the current function in tree-level
  * view-gimple-hierarchy ---- show gimple statement structure hierarchy
  * view-loop ---- show the loop
  * view-passes ---- show passes
  * view-rtx ---- show a specified rtx
  * view-tree ---- show a specified tree
  * view-tree-hierarchy ---- show tree structure hierarchy
  * view-tree-hierarchy-4-6 ---- show tree structure hierarchy for gcc 4.6
  * view-tree-hierarchy-4-7 ---- show tree structure hierarchy for gcc 4.7

Here's an example about how to use vcg-plugin in gdb (with vcg-plugin.gdbinit):

> gdb -q -args /path/to/cc1 test.c -fplugin=/path/to/vcg\_plugin.so -O2

> gdb) b ...

> gdb) r

> gdb) view-passes

you can use -fplugin-arg-vcg\_plugin-viewer=vcgview to select vcg viewer, default is vcgview (http://code.google.com/p/vcgviewer).

The plugin also can work without gdb, using -fplugin-arg-vcg\_plugin-option. Options are,

  * cgraph ---- dump the call graph before IPA passes.
  * cgraph-callee ---- dump the callee graph for each function.
  * cgraph-caller ---- dump the caller graph for each function.
  * gimple-hierarchy ---- dump the gimple hierarchy graph.
  * help ---- show this help.
  * passes ---- dump the passes graph.
  * pass-lists ---- dump the pass lists graph.
  * tree-hierarchy ---- dump the tree hierarchy graph.
  * tree-hierarchy-4-6 ---- dump the tree hierarchy graph for gcc 4.6.
  * tree-hierarchy-4-7 ---- dump the tree hierarchy graph for gcc 4.7.
  * viewer=name ---- set the vcg viewer, default is vcgview.

Here's an example about how to use vcg-plugin without gdb:

> gcc -fplugin=/path/to/vcg\_plugin.so -fplugin-arg-vcg\_plugin-cgraph foo.c

Supported GCC:

  * gcc 4.6 or 4.7 (trunk).
  * gcc 4.5 is not guaranteed, because I don't test on it.

TODO:

  * view ...

Any suggestions are welcome. Send email to <mingjie.xing AT gmail.com>.

