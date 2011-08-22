/*
 * ClutterSmith - a visual authoring environment for clutter.
 * Copyright (c) 2009, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * Alternatively, you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, with the following additional permissions:
 *
 * 1. Intel grants you an additional permission under Section 7 of the
 * GNU General Public License, version 3, exempting you from the
 * requirement in Section 6 of the GNU General Public License, version 3,
 * to accompany Corresponding Source with Installation Information for
 * the Program or any work based on the Program.  You are still required
 * to comply with all other Section 6 requirements to provide
 * Corresponding Source.
 *
 * 2. Intel grants you an additional permission under Section 7 of the
 * GNU General Public License, version 3, allowing you to convey the
 * Program or a work based on the Program in combination with or linked
 * to any works licensed under the GNU General Public License version 2,
 * with the terms and conditions of the GNU General Public License
 * version 2 applying to the combined or linked work as a whole.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Written by: Øyvind Kolås <oyvind.kolas@intel.com>
 */


#include <clutter/clutter.h>
#include <stdlib.h>
#include <string.h>
#include "cluttersmith.h"

void cs_selection_init (CSSelection *s)
{
  s->lassoed = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);
}


/* XXX: should be changed to deal with transformed coordinates to be able to
 * deal correctly with actors at any transformation and nesting.
 */
static gboolean
contains (gint min, gint max, gint minb, gint maxb)
{
  if (minb>=min && minb <=max &&
      maxb>=min && maxb <=max)
    return TRUE;
  return FALSE;
}

/**
 * cs_selected_get_list:
 *
 * Return value: (element-type ClutterActor) (transfer container):
 */
GList *cs_selection_get_list (CSSelection* s)
{
  return g_list_copy (s->selected);
}

static void update_active_actor (CSSelection *s)
{
  if (g_list_length (s->selected)==1)
    {
      cs_set_active (cs_selected_get_any (s));
    }
  else
    {
      cs_set_active (NULL);
    }
  if (g_list_length (s->selected)>0)
    clutter_stage_set_key_focus (CLUTTER_STAGE (clutter_actor_get_stage (cs->parasite_root)), NULL);
}

void cs_selection_add (CSSelection *s, ClutterActor *actor)
{
  if (!g_list_find (s->selected, actor))
    s->selected = g_list_append (s->selected, actor);
  update_active_actor ();
}

void cs_selected_remove (CSSelection *s, ClutterActor *actor)
{
  if (g_list_find (se, actor))
    s->selected = g_list_remove (s->selected, actor);
  update_active_actor ();
}

void cs_selection_foreach (CSSelection *s, GCallback cb, gpointer data)
{
  void (*each)(ClutterActor *actor, gpointer data)=(void*)cb;
  GList *sp;

  for (sp = s->selected; sp; sp = sp->next)
    {
      ClutterActor *actor = sp->data;
      if (actor != clutter_actor_get_stage (actor))
        each(actor, data);
    }
}

gpointer cs_selection_match (CSSelection *s, GCallback match_fun, gpointer data)
{
  gpointer ret = NULL;
  ret = cs_list_match (s->selected, match_fun, data);
  return ret;
}

void cs_selection_clear (CSSelection *s)
{
  if (s->selected)
    g_list_free (s->selected);
  s->selected = NULL;
  update_active_actor ();
}


gint cs_selection_count (CSSelection *s)
{
  return g_list_length (s->selected);
}


gboolean cs_selection_has_actor (CSSelection *s, ClutterActor *actor)
{
  if (g_list_find (s->selected, actor))
    return TRUE;
  return FALSE;
}


/**
 * cs_selection_get_any:
 *
 * Return value: (transfer none): any single of the selected actors if any or NULL
 */

ClutterActor *cs_selection_get_any (CSSelection *s)
{
  if (s->selected)
    return s->selected->data;
  return NULL;
}


struct LassoData
{
	CSSelection *s;
	GString *undo;
	GString *redo;
}

#define LASSO_BORDER 1

static gboolean
manipulate_lasso_capture (ClutterActor *stage,
                          ClutterEvent *event,
                          LassoData *data)
{
  CSSelection *s = data->s;

  switch (event->any.type)
    {
      case CLUTTER_MOTION:
        {
          gfloat ex=event->motion.x;
          gfloat ey=event->motion.y;

          gint mx = MIN (ex, lx);
          gint my = MIN (ey, ly);
          gint mw = MAX (ex, lx) - mx;
          gint mh = MAX (ey, ly) - my;

          clutter_actor_set_position (s->lasso, mx - LASSO_BORDER, my - LASSO_BORDER);
          clutter_actor_set_size (s->lasso, mw + LASSO_BORDER*2, mh+LASSO_BORDER*2);

          manipulate_x=ex;
          manipulate_y=ey;

          {
            gint no;
            GList *j, *list;
            g_hash_table_remove_all (s->lassoed);
            list = clutter_container_get_children (CLUTTER_CONTAINER (cs_get_current_container ()));

            for (no = 0, j=list; j;no++,j=j->next)
              {
                gfloat cx, cy;
                gfloat cw, ch;
                clutter_actor_get_transformed_position (j->data, &cx, &cy);
                clutter_actor_get_transformed_size (j->data, &cw, &ch);

                if (contains (mx, mx + mw, cx, cx + cw) &&
                    contains (my, my + mh, cy, cy + ch))
                  {
                    g_hash_table_insert (selection, j->data, j->data);
                  }
              }
            g_list_free (list);
          }
        }
        break;
      case CLUTTER_BUTTON_RELEASE:
         {
          ClutterModifierType state = event->button.modifier_state;
          GHashTableIter      iter;
          gpointer            key, value;

          g_hash_table_iter_init (&iter, s->lassoed);
          while (g_hash_table_iter_next (&iter, &key, &value))
            {
              if (state & CLUTTER_CONTROL_MASK)
                {
                  if (cs_selection_has_actor (s, key))
                    cs_selection_remove (s, key);
                  else
                    cs_selection_add (s, key);
                }
              else
                {
                  cs_selection_add (s, key);
                }
            }
        }
        g_hash_table_remove_all (s->lassoed);

        g_signal_handlers_disconnect_by_func (stage, manipulate_lasso_capture, data);
        clutter_actor_destroy (s->lasso);
        clutter_actor_queue_redraw (stage);
        s->lasso = NULL;
        select_action_post("select lasso", data->undo, data->redo);
	g_slice_free(LassoData, data);
      default:
        break;
    }
  return TRUE;
}

gboolean
cs_selection_lasso_start (CSSelection *s ,
                          ClutterActor  *actor,
                          ClutterEvent  *event)
{
  ClutterModifierType state = event->button.modifier_state;


  g_assert (s->lasso == NULL);

    {
      ClutterColor lassocolor       = {0xff,0x0,0x0,0x11};
      ClutterColor lassobordercolor = {0xff,0x0,0x0,0x88};
      s->lasso = clutter_rectangle_new_with_color (&lassocolor);
      clutter_rectangle_set_border_color (CLUTTER_RECTANGLE (s->lasso), &lassobordercolor);
      clutter_rectangle_set_border_width (CLUTTER_RECTANGLE (s->lasso), LASSO_BORDER);
      clutter_container_add_actor (CLUTTER_CONTAINER (cs->parasite_root), s->lasso);
    }
  s->lx = event->button.x;
  s->ly = event->button.y;

  clutter_actor_set_position (lasso, lx-LASSO_BORDER, ly-LASSO_BORDER);
  clutter_actor_set_size (lasso, LASSO_BORDER*2, LASSO_BORDER*2);

  manipulate_x = event->button.x;
  manipulate_y = event->button.y;

  LassoData *ld = g_slice_new0(LassoData);
  ld->s = s;
  ld->undo = g_string_new("");
  ld->redo = g_string_new("");

  g_signal_connect (clutter_actor_get_stage (actor), "captured-event",
                    G_CALLBACK (manipulate_lasso_capture), ld);
  select_action_pre2(ld->undo);

  if (!((state & CLUTTER_SHIFT_MASK) ||
        (state & CLUTTER_CONTROL_MASK)))
    {
      cs_selection_clear (s);
    }

  return TRUE;
}


void cs_selection_paint (CSSelection *s)
{
  ClutterVertex verts[4];
  /* draw outlines for actors */
  GHashTableIter      iter;
  gpointer            key, value;

  {
      {
        if (cs->fake_stage)
          {
            cogl_set_source_color4ub (0, 255, 0, 255);
            cs_draw_actor_outline (cs->fake_stage, NULL);
          }
      }
  }

  cogl_set_source_color4ub (255, 0, 0, 128);
  cs_selection_foreach (G_CALLBACK (cs_draw_actor_outline), NULL);

  g_hash_table_iter_init (&iter, s->lassoed);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      clutter_actor_get_abs_allocation_vertices (key,
                                                 verts);

      cogl_set_source_color4ub (0, 0, 25, 50);

      {
        {
          gfloat coords[]={ verts[0].x, verts[0].y, 
             verts[1].x, verts[1].y, 
             verts[3].x, verts[3].y, 
             verts[2].x, verts[2].y, 
             verts[0].x, verts[0].y };

          cogl_path_polyline (coords, 5);
          cogl_set_source_color4ub (0, 0, 255, 128);
          cogl_path_stroke ();
        }
      }
    }
}


static void each_add_to_list (ClutterActor *actor, gpointer      string)
{
  g_string_append_printf (string, "$(\"%s\"),", cs_get_id (actor));
}


void cs_select_area_action_pre2(CSSelection *s, GString *undo)
{
  g_string_append_printf (undo, "var list=[");
  cs_selection_foreach (s, G_CALLBACK (each_add_to_list), undo);
  g_string_append_printf (undo, "];\n"
                          "CS.selection_clear();\n"
                          "for (x in list) CS.selection_add (list[x]);\n");
}

void cs_select_action_post(CSSelection *s, const char *action_name, GString *undo, GString *redo) 
  g_string_append_printf (redo, "var list=[");
  cs_selection_foreach (s, G_CALLBACK (each_add_to_list), redo);
  g_string_append_printf (redo, "];\n"
                          "CS.selection_clear();\n"
                          "for (x in list) CS.selection_add (list[x]);\n");
  if (!g_str_equal (redo->str, undo->str))
    cs_history_add (action_name, redo->str, undo->str);
  g_string_free (undo, TRUE);
  g_string_free (redo, TRUE);
}


