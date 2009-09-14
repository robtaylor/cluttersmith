/* ClutterScript viewer, a viewer for displaying clutter scripts or fragments
 * of clutterscript.
 *
 * Copyright 2007 OpenedHand Ltd
 * Authored by Øyvind Kolås <pippin@o-hand.com>
 * Licensed under the GPL v2 or greater.
 *
 */

#include <clutter/clutter.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>


/* Global structure containing information parsed from commandline parameters */
static struct
{
  gboolean  fullscreen;
  gchar    *bg_color;
  gint      width, height;
  gchar    *path;
  gchar    *id;
  gchar    *timeline;
}
args =
{
  FALSE,
  "gray",
  1024, 600,
  "json/foo.json",
  "actor",
  NULL
};

/* using global variables, this is needed at least for the ClutterScript to avoid
 * possible behaviours to be destroyed when the script is destroyed.
 */
static ClutterActor    *stage;
static ClutterActor    *actor    = NULL;
static ClutterTimeline *timeline = NULL;
static ClutterScript   *script   = NULL;

gboolean
parse_args (gchar **argv)
{
  gchar **arg = argv + 1;

  while (*arg)
    {
      if (g_str_equal (*arg, "-h") ||
          g_str_equal (*arg, "--help"))
        {
usage:
          g_print ("\nUsage: %s [options] %s\n\n",
                   argv[0], args.path ? args.path : "<clutterscript>");
          g_print ("  -s <widthXheight>       stage size                  (%ix%i)\n",
                   args.width, args.height);
          g_print ("  -fs                     run fullscreen              (%s)\n",
                   args.fullscreen ? "TRUE" : "FALSE");
          g_print ("  -bg <color>             stage color                 (%s)\n",
                   args.bg_color);
          g_print ("  -id <actor id>          which actor id to show      (%s)\n",
                   args.id ? args.id : "NULL");
          g_print ("  -timeline <timeline id> a timeline to play          (%s)\n",
                   args.timeline ? args.timeline : "NULL");
          g_print ("  -h                      this help\n\n");
          return FALSE;
        }
      else if (g_str_equal (*arg, "-s"))
        {
          arg++; g_assert (*arg);
          args.width = atoi (*arg);
          if (strstr (*arg, "x"))
            args.height = atoi (strstr (*arg, "x") + 1);
        }
      else if (g_str_equal (*arg, "-bg"))
        {
          arg++; g_assert (*arg);
          args.bg_color = *arg;
        }
      else if (g_str_equal (*arg, "-id"))
        {
          arg++; g_assert (*arg);
          args.id = *arg;
        }
      else if (g_str_equal (*arg, "-timeline"))
        {
          arg++; g_assert (*arg);
          args.timeline = *arg;
        }
      else if (g_str_equal (*arg, "-fs"))
        {
          args.fullscreen = TRUE;
        }
      else
        {
          args.path = *arg;
        }
      arg++;
    }
  if (args.path == NULL)
    {
      g_print ("Error parsing commandline: no clutterscript provided\n");
      goto usage;
    }
  return TRUE;
}

static ClutterActor *
initialize_stage ()
{
  ClutterActor *stage;
  ClutterColor  color;

  stage = clutter_stage_get_default ();
  clutter_color_from_string (&color, args.bg_color);
  clutter_stage_set_color (CLUTTER_STAGE (stage), &color);
  clutter_actor_set_size (stage, args.width, args.height);
  clutter_actor_show (stage);

  if (args.fullscreen)
    clutter_stage_set_fullscreen (CLUTTER_STAGE (stage), TRUE);

  return stage;
}


static void
load_script (const gchar *path)
{
  GError        *error = NULL;

  g_assert (CLUTTER_IS_SCRIPT (script));
  clutter_script_load_from_file (script, path, &error);

  if (error)
    {
      ClutterColor error_color = { 0xff, 0, 0, 0xff };
      actor = clutter_text_new_with_text ("Sans 20px", error->message);
      clutter_actor_set_size (actor, clutter_actor_get_width (stage),
                                     200);
      g_print ("!%s\n", error->message);
      clutter_group_add (CLUTTER_GROUP (stage), actor);
      clutter_actor_show_all (stage);
      clutter_text_set_color (CLUTTER_TEXT (actor), &error_color);
      g_clear_error (&error);
      return;
    }

  actor = CLUTTER_ACTOR (clutter_script_get_object (script, args.id));

  if (actor == NULL)
    {
      ClutterColor error_color = { 0xff, 0, 0, 0xff };
      gchar        message[256];

      g_sprintf (message, "No actor with \"id\"=\"%s\" found", args.id);
      actor = clutter_text_new_with_text ("Sans 30px", message);
      clutter_text_set_color (CLUTTER_TEXT (actor), &error_color);
    }
  else 
    {
      clutter_group_add (CLUTTER_GROUP (stage), actor);
      clutter_actor_show_all (stage);

      if (args.timeline != NULL)
        {
          timeline = CLUTTER_TIMELINE (clutter_script_get_object (
                                       script, args.timeline));
          if (timeline)
            clutter_timeline_start (timeline);
        }
      clutter_script_connect_signals (script, script);
      g_object_set_data_full (G_OBJECT (actor), "clutter-script", script, NULL);
    }
}

void gst_init (gpointer, gpointer);
gboolean idle_add_stage (gpointer stage);

#ifndef COMPILEMODULE
gint
main (gint    argc,
      gchar **argv)
{
  clutter_init (&argc, &argv);
  gst_init (&argc, &argv);

  if (!parse_args (argv))
    return -1;

  stage = initialize_stage ();

  script = clutter_script_new ();
  load_script (args.path);

  g_timeout_add (100, idle_add_stage, stage);

  clutter_main ();
  return 0;
}
#endif
