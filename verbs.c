#include <clutter/clutter.h>
#include <nbtk/nbtk.h>
#include "cluttersmith.h"
#include "util.h"

/****/

/**[ copy and paste ]*******************************************************/

static GList *clipboard = NULL;

static void each_duplicate (ClutterActor *actor)
{
  ClutterActor *parent, *new_actor;
  parent = cluttersmith_get_add_root (actor);
  new_actor = util_duplicator (actor, parent);
  {
    gfloat x, y;
    clutter_actor_get_position (new_actor, &x, &y);
    x+=10;y+=10;
    clutter_actor_set_position (new_actor, x, y);
  }
  cluttersmith_selected_clear ();
  cluttersmith_selected_add (new_actor);
}

void cb_duplicate_selected (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (each_duplicate), NULL);
  CS_REVISION++;
}

static void each_remove (ClutterActor *actor)
{
  ClutterActor *parent;
  parent = clutter_actor_get_parent (actor);
  g_object_ref (actor);
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), actor);
  clutter_actor_destroy (actor);
}

void cb_remove_selected (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (each_remove), NULL);
  CS_REVISION++;
  cluttersmith_selected_clear ();
}

static void empty_clipboard (void)
{
  while (clipboard)
    {
      g_object_unref (clipboard->data);
      clipboard = g_list_remove (clipboard, clipboard->data);
    }
}

static void each_cut (ClutterActor *actor)
{
  ClutterActor *parent;
  parent = clutter_actor_get_parent (actor);
  g_object_ref (actor);
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), actor);
  clipboard = g_list_append (clipboard, actor);
  clutter_actor_destroy (actor);
}

void cb_cut_selected (ClutterActor *actor)
{
  empty_clipboard ();
  cluttersmith_selected_foreach (G_CALLBACK (each_cut), NULL);
  CS_REVISION++;
  cluttersmith_selected_clear ();
}

static void each_copy (ClutterActor *actor)
{
  ClutterActor *parent, *new_actor;
  parent = clutter_actor_get_parent (actor);

  new_actor = util_duplicator (actor, parent);
  g_object_ref (new_actor);
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), new_actor);
  clipboard = g_list_append (clipboard, new_actor);
}

void cb_copy_selected (ClutterActor *actor)
{
  empty_clipboard ();
  cluttersmith_selected_foreach (G_CALLBACK (each_copy), NULL);
  CS_REVISION++;
}


void cb_paste_selected (ClutterActor *actor)
{
  if (clipboard)
    {
      GList *i;
      ClutterActor *new_actor, *parent;

      parent = cluttersmith_get_add_root (actor);

      for (i=clipboard; i;i=i->next)
        {
          new_actor = util_duplicator (i->data, parent);
          g_assert (new_actor);
          {
            gfloat x, y;
            clutter_actor_get_position (new_actor, &x, &y);
            x+=10;y+=10;
            clutter_actor_set_position (new_actor, x, y);
          }
        }
      cluttersmith_selected_clear ();
      cluttersmith_selected_add (new_actor);
    }
  CS_REVISION++;
}

void cb_raise_selected (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (clutter_actor_raise), NULL);
  CS_REVISION++;
}

void cb_lower_selected (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (clutter_actor_lower), NULL);
  CS_REVISION++;
}


void cb_raise_top_selected (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (clutter_actor_raise_top), NULL);
  CS_REVISION++;
}

void cb_lower_bottom_selected (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (clutter_actor_lower_bottom), NULL);
  CS_REVISION++;
}

static void each_reset_size (ClutterActor *actor)
{
  clutter_actor_set_size (actor, -1, -1);
}

void cb_reset_size (ClutterActor *actor)
{
  cluttersmith_selected_foreach (G_CALLBACK (each_reset_size), NULL);
}

void cb_quit (ClutterActor *actor)
{
  clutter_main_quit ();
}

void cb_focus_entry (ClutterActor *actor)
{
  ClutterActor *entry;
  entry = nbtk_entry_get_clutter_text (NBTK_ENTRY (util_find_by_id_int (actor, "title")));
  g_assert (entry);
  if (entry)
    {
      g_print ("trying to set key focus\n");
      clutter_stage_set_key_focus (CLUTTER_STAGE (clutter_actor_get_stage (entry)), entry);
    }
}

void cb_select_none (ClutterActor *actor)
{
  g_print ("%s NYI\n", G_STRFUNC);
}

void cb_select_all (ClutterActor *actor)
{
  g_print ("%s NYI\n", G_STRFUNC);
}


static void each_group (ClutterActor *actor,
                        gpointer      new_parent)
{
  ClutterActor *parent;
  parent = clutter_actor_get_parent (actor);
  g_object_ref (actor);
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), actor);
  clutter_container_add_actor (CLUTTER_CONTAINER (new_parent), actor);
  g_object_unref (actor);
}

void cb_group (ClutterActor *actor)
{
  ClutterActor *parent;
  ClutterActor *group;
  parent = cluttersmith_get_add_root (actor);
  group = clutter_group_new ();
  /* find upper left item,. and place group there,.. reposition children to
   * fit this
   */
  clutter_container_add_actor (CLUTTER_CONTAINER (parent), group);
  /* get add_parent */
  /* create group */
  cluttersmith_selected_foreach (G_CALLBACK (each_group), group);
  /* reparent all children to group */
  g_print ("%s NYC\n", G_STRFUNC);
}


static void each_ungroup (ClutterActor *actor,
                        gpointer      new_parent)
{
  ClutterActor *parent;
  parent = clutter_actor_get_parent (actor);
  g_object_ref (actor);
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), actor);
  clutter_container_add_actor (CLUTTER_CONTAINER (new_parent), actor);
  g_object_unref (actor);
}


void cb_ungroup (ClutterActor *actor)
{
  ClutterActor *parent;
  /* XXX: this is all wrong... */
  ClutterActor *active_actor = cluttersmith_selected_get_any ();
  /* The group needs to be directly selected for now */
  parent = clutter_actor_get_parent (active_actor);
  cluttersmith_selected_foreach (G_CALLBACK (each_ungroup), parent);
  clutter_actor_destroy (active_actor);
  /* reparent all children to group */
  g_print ("%s NYC\n", G_STRFUNC);
}

void cb_select_parent (ClutterActor *actor)
{
  ClutterActor *active_actor = cluttersmith_selected_get_any ();
  if (active_actor)
    {
      ClutterActor *parent = clutter_actor_get_parent (active_actor);
      if (parent) 
        {
          cluttersmith_selected_clear ();
          cluttersmith_selected_add (parent);
          clutter_actor_queue_redraw (active_actor);
        }
    }
}

/******************************************************************************/

typedef struct KeyBinding {
  ClutterModifierType modifier;
  guint key_symbol;
  void (*callback) (ClutterActor *actor);
} KeyBinding;

static KeyBinding keybindings[]={
  {CLUTTER_CONTROL_MASK, CLUTTER_x,         cb_cut_selected},
  {CLUTTER_CONTROL_MASK, CLUTTER_c,         cb_copy_selected},
  {CLUTTER_CONTROL_MASK, CLUTTER_v,         cb_paste_selected},
  {CLUTTER_CONTROL_MASK, CLUTTER_d,         cb_duplicate_selected},
  {CLUTTER_CONTROL_MASK, CLUTTER_q,         cb_quit},
  {CLUTTER_CONTROL_MASK, CLUTTER_l,         cb_focus_entry},

  /* check for the more specific modifier state before the more generic ones */
  {CLUTTER_CONTROL_MASK|
   CLUTTER_SHIFT_MASK,   CLUTTER_a,         cb_select_none},
  {CLUTTER_CONTROL_MASK, CLUTTER_a,         cb_select_all},

  /* check for the more specific modifier state before the more generic ones */
  {CLUTTER_CONTROL_MASK|
   CLUTTER_SHIFT_MASK,   CLUTTER_g,         cb_ungroup},
  {CLUTTER_CONTROL_MASK, CLUTTER_g,         cb_group},
  {CLUTTER_CONTROL_MASK, CLUTTER_p,         cb_select_parent},

  {0,                    CLUTTER_BackSpace, cb_remove_selected},
  {0,                    CLUTTER_Delete,    cb_remove_selected},
  {0,                    CLUTTER_Page_Up,   cb_raise_selected},
  {0,                    CLUTTER_Page_Down, cb_lower_selected},
  {0,                    CLUTTER_Home,      cb_raise_top_selected},
  {0,                    CLUTTER_End,       cb_lower_bottom_selected},
  {0, 0, NULL},
};

gboolean manipulator_key_pressed (ClutterActor *stage, ClutterModifierType modifier, guint key)
{
  gint i;
  for (i=0; keybindings[i].key_symbol; i++)
    {
      if (keybindings[i].key_symbol == key &&
          ((keybindings[i].modifier & modifier) == keybindings[i].modifier))
        {
          keybindings[i].callback (stage);
          return TRUE;
        }
    }
  return FALSE;
}