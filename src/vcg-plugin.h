#ifndef VCG_PLUGIN_H
#define VCG_PLUGIN_H

/* gcc's header files */

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

/* dump file names */


/* These are available functins, which can be used to
   dump and view gcc internal data structures. */

extern void vcg_plugin_dump_function (tree fn, int flag); 
extern void vcg_plugin_view_function (tree fn, int flag); 

extern void vcg_plugin_view (char *filename);

#endif
