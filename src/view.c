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

extern char *vcg_viewer;

void
vcg_plugin_view (char *filename)
{
  char *cmd;
  pid_t pid;

  cmd = concat (vcg_viewer, " ", filename, NULL);
  pid = fork ();
  if (pid == 0)
    {
      system (cmd);
      exit (0);
    }
}
