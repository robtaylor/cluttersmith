

#include <gdk-pixbuf/gdk-pixbuf.h>

#include <clutter/clutter.h>
#include "cluttersmith.h"

static void
free_data (guchar  *pixels,
           gpointer  data)
{
  g_free (pixels);
}

void export_png (const gchar *scene,
                 const gchar *png)
{
  GdkPixbuf *pixbuf;
  GError    *error = NULL;
  guchar    *pixels;
  gfloat     x, y;
  gint       width, height;
  guint      old_mode = cs_get_ui_mode ();

  /* disable chrome */

  cs_set_ui_mode (CS_UI_MODE_BROWSE);

  cluttersmith_load_scene (scene);
  clutter_actor_get_position (cs->fake_stage, &x, &y);

  g_object_get (cs, "canvas-width", &width,
                    "canvas-height", &height,
                    NULL);

  g_print ("exporting scene %s to %s  %f,%f %dx%d\n", scene, png,
           x, y, width, height);

  pixels = clutter_stage_read_pixels (
             (ClutterStage *)clutter_actor_get_stage (cs->fake_stage),
             x, y, width, height);
  pixbuf = gdk_pixbuf_new_from_data (pixels,
                                     GDK_COLORSPACE_RGB,
                                     TRUE,
                                     8,
                                     width,
                                     height,
                                     ((gint)(width)) * 4,
                                     free_data,
                                     NULL);

  gdk_pixbuf_save (pixbuf, png, "png", &error, NULL);
  if (error)
    {
      g_warning ("Failed to save scene to file: %s: %s\n",
                 png, error->message);
      g_error_free (error);
    }

  g_object_unref (pixbuf);
  cs_set_ui_mode (old_mode);
  /* re-enable chrome */
}

void export_pdf (const gchar *pdf)
{
  g_print ("exporting scenes to %s\n", pdf);
}

void cs_export_png (void)
{
  export_png (mx_entry_get_text (MX_ENTRY (cs->scene_title)),
              mx_entry_get_text (MX_ENTRY (cs_find_by_id_int (
                                 clutter_actor_get_stage (cs->fake_stage), 
                                 "cs-png-path"))));

}

void cs_export_pdf (void)

{
  export_pdf (mx_entry_get_text (MX_ENTRY (cs_find_by_id_int (
                                 clutter_actor_get_stage (cs->fake_stage), 
        "cs-pdf-path"))));
}
