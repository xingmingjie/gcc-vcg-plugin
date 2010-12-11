/* Vcg plugin internal common routines.

   Copyright (C) 2009, 2010 Mingjie Xing, mingjie.xing@gmail.com. 

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

/* vcg viewer tool, default is vcgview */
static char *vcg_viewer = "vcgview";

static void
vcg_init (int argc, char *argv[])
{
  int i;

  for (i = 0; i < argc; i++)
    {
      printf ("key: %s\n", argv[i].key);
      printf ("value: %s\n", argv[i].value);
      /* Get the vcg viewer tool, default is "vcgview". */
      if (strcmp (argv[i].key, "viewer") == 0)
        {
          vcg_viewer = argv[i].value;
        }
    }
}

static void
vcg_dump (gdl_graph *graph)
{
  gdl_dump_graph (fout, graph);
}

static void
vcg_show (gdl_graph *graph)
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

vcg_plugin_common_t vcg_plugin_common =
{
  "vcg_plugin",
  "0.1",
  "vcgview",
  vcg_init,
  vcg_dump,
  vcg_show
};
