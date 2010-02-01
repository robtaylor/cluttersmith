#include <clutter/clutter.h>
#include <mx/mx.h>
#include "cluttersmith.h"
#include "util.h"
#include <string.h>

/****/

/**[ copy and paste ]*******************************************************/

static GList *clipboard = NULL;


static void each_duplicate (ClutterActor *actor,
                            GList        **new)
{
  ClutterActor *parent, *new_actor;
  parent = cs_get_current_container ();
  new_actor = cs_duplicator (actor, parent);
  {
    gfloat x, y;
    clutter_actor_get_position (new_actor, &x, &y);
    x+=10;y+=10;
    clutter_actor_set_position (new_actor, x, y);
  }
  *new = g_list_append (*new, new_actor);
}

void cs_duplicate (ClutterActor *ignored)
{
  GList *n, *new = NULL;
  cs_selected_foreach (G_CALLBACK (each_duplicate), &new);
  cs_selected_clear ();
  for (n=new; n; n=n->next)
    cs_selected_add (n->data);
  g_list_free (new);
  cs_dirtied ();
}

static void each_remove (ClutterActor *actor)
{
  ClutterActor *parent;
  parent = clutter_actor_get_parent (actor);
  clutter_actor_destroy (actor);
}

void cs_edit (ClutterActor *ignored)
{
  ClutterActor *active = cs_get_active ();
  if (active)
    {
      cs_edit_actor_start (active);
    }
}


void cs_remove (ClutterActor *ignored)
{
  ClutterActor *active = cs_get_active ();
  if (active)
    {
      ClutterActor *parent = clutter_actor_get_parent (active);

      if (active == cluttersmith->fake_stage)
        return;

      cs_set_active (NULL);
      cs_selected_foreach (G_CALLBACK (each_remove), NULL);
      cs_dirtied ();
      cs_selected_clear ();
      {
        GList *children;
        children = clutter_container_get_children (CLUTTER_CONTAINER (parent));
        if (children)
          {
            cs_selected_add (children->data);
            g_list_free (children);
          }
        else
          {
            cs_selected_add (parent);
          }
      }
    }
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

void cs_cut (ClutterActor *ignored)
{
  ClutterActor *active = cs_get_active ();
  empty_clipboard ();
  if (active)
    {
      ClutterActor *parent = clutter_actor_get_parent (active);
      cs_set_active (NULL);
      cs_selected_foreach (G_CALLBACK (each_cut), NULL);
      cs_dirtied ();
      cs_selected_clear ();
      cs_selected_add (parent);
    }
}

static void each_copy (ClutterActor *actor)
{
  ClutterActor *parent, *new_actor;
  parent = clutter_actor_get_parent (actor);

  new_actor = cs_duplicator (actor, parent);
  g_object_ref (new_actor);
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), new_actor);
  clipboard = g_list_append (clipboard, new_actor);
}

void cs_copy (ClutterActor *ignored)
{
  empty_clipboard ();
  cs_selected_foreach (G_CALLBACK (each_copy), NULL);
  cs_dirtied ();
}


void cs_paste (ClutterActor *ignored)
{
  if (clipboard)
    {
      GList *i;
      ClutterActor *new_actor = NULL, *parent;

      parent = cs_get_current_container ();

      for (i=clipboard; i;i=i->next)
        {
          new_actor = cs_duplicator (i->data, parent);
          g_assert (new_actor);
          {
            gfloat x, y;
            clutter_actor_get_position (new_actor, &x, &y);
            clutter_actor_set_position (new_actor, x, y);
          }
        }
      cs_selected_clear ();
      if (new_actor)
        cs_selected_add (new_actor);
    }
  cs_dirtied ();
}

void cs_raise (ClutterActor *ignored)
{
  cs_selected_foreach (G_CALLBACK (clutter_actor_raise), NULL);
  cs_dirtied ();
}

void cs_lower (ClutterActor *ignored)
{
  cs_selected_foreach (G_CALLBACK (clutter_actor_lower), NULL);
  cs_dirtied ();
}


void cs_raise_top (ClutterActor *ignored)
{
  cs_selected_foreach (G_CALLBACK (clutter_actor_raise_top), NULL);
  cs_dirtied ();
}

void cs_lower_bottom (ClutterActor *ignored)
{
  cs_selected_foreach (G_CALLBACK (clutter_actor_lower_bottom), NULL);
  cs_dirtied ();
}

static void each_reset_size (ClutterActor *actor)
{
  clutter_actor_set_size (actor, -1, -1);
}

void cs_reset_size (ClutterActor *ignored)
{
  cs_selected_foreach (G_CALLBACK (each_reset_size), NULL);
  cs_dirtied ();
}

void cs_quit (ClutterActor *ignored)
{
  cs_save_dialog_state ();
  clutter_main_quit ();
}

void cs_focus_title (ClutterActor *ignored)
{
  ClutterActor *entry;
  entry = mx_entry_get_clutter_text (MX_ENTRY (cs_find_by_id_int (cluttersmith->parasite_root, "title")));
  g_assert (entry);
  if (entry)
    {
      clutter_stage_set_key_focus (CLUTTER_STAGE (clutter_actor_get_stage (entry)), entry);

      clutter_actor_show(cluttersmith->dialog_toolbar);
      clutter_actor_queue_relayout (cluttersmith->dialog_toolbar);
      clutter_actor_queue_relayout (clutter_actor_get_parent (cluttersmith->dialog_toolbar));
      cs_sync_chrome ();
      clutter_actor_queue_redraw (cluttersmith->dialog_toolbar);
    }
}

void cs_select_none (ClutterActor *ignored)
{
  cs_selected_clear ();
}

void cs_select_all (ClutterActor *ignored)
{
  GList *l, *list;
  cs_selected_clear ();
  list = clutter_container_get_children (CLUTTER_CONTAINER (cs_get_current_container()));
  for (l=list; l;l=l->next)
    {
      cs_selected_add (l->data);
    }
  g_list_free (list);
}

void cs_view_reset (ClutterActor *ignored)
{
  g_object_set (cluttersmith, "zoom", 100.0, "origin-x", 0.0, "origin-y", 0.0, NULL);
}

static gfloat min_x=0;
static gfloat min_y=0;

static void each_group (ClutterActor *actor,
                        gpointer      new_parent)
{
  ClutterActor *parent;
  gfloat x, y;
  parent = clutter_actor_get_parent (actor);
  clutter_actor_get_position (actor, &x, &y);
  g_object_ref (actor);
  if (x < min_x)
    min_x = x;
  if (y < min_y)
    min_y = y;
  clutter_container_remove_actor (CLUTTER_CONTAINER (parent), actor);
  clutter_container_add_actor (CLUTTER_CONTAINER (new_parent), actor);
  g_object_unref (actor);
}

static void each_group_move (ClutterActor *actor,
                             gfloat       *delta)
{
  gfloat x, y;
  clutter_actor_get_position (actor, &x, &y);
  clutter_actor_set_position (actor, x-delta[0], y-delta[1]);
}

ClutterActor *cs_group (ClutterActor *ignored)
{
  gfloat delta[2];
  ClutterActor *parent;
  ClutterActor *group;
  parent = cs_get_current_container ();
  group = clutter_group_new ();
  /* find upper left item,. and place group there,.. reposition children to
   * fit this
   */
  clutter_container_add_actor (CLUTTER_CONTAINER (parent), group);
  /* get add_parent */
  /* create group */

  min_x = 2000000.0;
  min_y = 2000000.0;

  cs_selected_foreach (G_CALLBACK (each_group), group);
  delta[0]=min_x;
  delta[1]=min_y;
  cs_selected_foreach (G_CALLBACK (each_group_move), delta);
  clutter_actor_set_position (group, min_x, min_y);
  cs_selected_clear ();
  cs_selected_add (group);
  cs_dirtied ();
  return group;
}

static void each_ungroup (ClutterActor *actor,
                          GList       **created_list)
{
  ClutterActor *parent;
  parent = clutter_actor_get_parent (actor);
  if (CLUTTER_IS_CONTAINER (actor))
    {
      GList *c, *children;
      gfloat cx, cy;
      children = clutter_container_get_children (CLUTTER_CONTAINER (actor));
      clutter_actor_get_position (actor, &cx, &cy);
      for (c = children; c; c = c->next)
        {
          gfloat x, y;
          ClutterActor *child = c->data;
          g_object_ref (child);
          clutter_actor_get_position (child, &x, &y);
          clutter_actor_set_position (child, cx + x, cy + y);
          clutter_container_remove_actor (CLUTTER_CONTAINER (actor), child);
          clutter_container_add_actor (CLUTTER_CONTAINER (parent), child);
          g_object_unref (child);
          *created_list = g_list_append (*created_list, child);
        }
      g_list_free (children);
      clutter_actor_destroy (actor);
    }
}

void cs_ungroup (ClutterActor *ignored)
{
  GList *i, *created_list = NULL;
  cs_set_active (NULL);
  cs_selected_foreach (G_CALLBACK (each_ungroup), &created_list);
  cs_selected_clear (); 
  for (i=created_list; i; i=i->next)
    {
      cs_selected_add (i->data);
    }
  g_list_free (created_list);
  cs_dirtied ();
}

void cs_make_group_box (ClutterActor *ignored)
{
  ClutterActor *active_actor = cs_selected_get_any ();
  gboolean vertical = TRUE;
  if (!active_actor)
    return;
  
  {
    GList *children, *c;
    gint minx=9000, miny=9000, maxx=-9000, maxy=-90000;
    children = clutter_container_get_children (CLUTTER_CONTAINER (active_actor));
    for (c=children; c; c=c->next)
      {
        gfloat x, y;
        clutter_actor_get_position (c->data, &x, &y);
        if (x < minx)
          minx = x;
        if (y < miny)
          miny = y;
        if (x > maxx)
          maxx = x;
        if (y > maxy)
          maxy = y;
      }
    if (maxx-minx > maxy - miny)
      vertical = FALSE;
  }

  active_actor = cs_actor_change_type (active_actor, "MxBoxLayout");
  g_object_set (G_OBJECT (active_actor), "vertical", vertical, NULL);
  cs_selected_clear (); 
  cs_selected_add (active_actor);
  cs_dirtied ();
}

void cs_make_box (ClutterActor *ignored)
{
  cs_group (ignored);
  cs_make_group_box (ignored);
}

void cs_make_group (ClutterActor *ignored)
{
  ClutterActor *active_actor = cs_selected_get_any ();
  if (!active_actor)
    return;
  
  active_actor = cs_actor_change_type (active_actor, "ClutterGroup");
  cs_selected_clear (); 
  cs_selected_add (active_actor);
  cs_dirtied ();
}

void cs_select_parent (ClutterActor *ignored)
{
  ClutterActor *active_actor = cs_selected_get_any ();
  if (active_actor)
    {
      ClutterActor *parent = clutter_actor_get_parent (active_actor);
      if (parent)
        {
          ClutterActor *gparent;
          gparent = clutter_actor_get_parent (parent);
          if (parent != cluttersmith->fake_stage)
            {
              cs_selected_clear ();
              cs_selected_add (parent);
              cs_set_current_container (gparent);
              clutter_actor_queue_redraw (active_actor);
            }
        }
    }
  else if (cs_get_current_container () != cluttersmith->fake_stage)
    {
      ClutterActor *parent = cs_get_current_container ();
      if (parent && parent != cluttersmith->fake_stage )
        {
          cs_selected_clear ();
          cs_selected_add (parent);
          parent = clutter_actor_get_parent (parent);
          cs_set_current_container (parent);
          clutter_actor_queue_redraw (active_actor);
        }
    }
}

void
cs_help (ClutterActor *ignored)
{
  cluttersmith_set_project_root (PKGDATADIR "docs");
}


static void select_nearest (gboolean vertical,
                            gboolean reverse)
{
  ClutterActor *actor = cs_selected_get_any ();
  if (actor)
    {
      ClutterActor *new = cs_find_nearest (actor, vertical, reverse);
      if (new)
        {
          cs_selected_clear ();
          cs_selected_add (new);
        }
    }
  else
    {
      GList *children = clutter_container_get_children (
                            CLUTTER_CONTAINER (cs_get_current_container ()));
      if (children)
        {
          cs_selected_clear ();
          cs_selected_add (children->data);
          g_list_free (children);
        }
    }
}

void
cs_keynav_left (ClutterActor *ignored)
{
  select_nearest (FALSE, TRUE);
}

void
cs_keynav_right (ClutterActor *ignored)
{
  select_nearest (FALSE, FALSE);
}

void
cs_keynav_up (ClutterActor *ignored)
{
  select_nearest (TRUE, TRUE);
}

void
cs_keynav_down (ClutterActor *ignored)
{
  select_nearest (TRUE, FALSE);
}




void
cs_ui_mode (ClutterActor *ignored)
{
  switch (cs_get_ui_mode ())
    {
        case CS_UI_MODE_BROWSE:
          cs_set_ui_mode (CS_UI_MODE_CHROME);
          break;
/*        case CS_UI_MODE_EDIT:
          cs_set_ui_mode (CS_UI_MODE_CHROME);
          g_print ("Run mode : ui with edit\n");
          break;*/
        case CS_UI_MODE_CHROME: 
        default:
          cs_set_ui_mode (CS_UI_MODE_BROWSE);
          break;
    }
}


static void dialog_toggle (gpointer action,
                           ClutterActor *dialog)
{
  if (clutter_actor_get_paint_visibility (dialog))
    {
      clutter_actor_hide (dialog);
    }
  else
    {
      clutter_actor_show(dialog);
    }
  clutter_actor_queue_relayout (dialog);
  clutter_actor_queue_relayout (clutter_actor_get_parent (dialog));
  cs_sync_chrome ();
  clutter_actor_queue_redraw (dialog);
}

static void dialog_toggle_toolbar (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_toolbar); }

#if 0
static void dialog_toggle_tree (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_tree); }

static void dialog_toggle_property_inspector (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_property_inspector); }
#endif

static void dialog_toggle_sidebar (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_tree);
  dialog_toggle (NULL, cluttersmith->dialog_property_inspector); }

static void dialog_toggle_editor (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_editor); }

static void dialog_toggle_templates (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_templates); }

static void dialog_toggle_scenes (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_scenes); }

static void dialog_toggle_config (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_config); }

static void dialog_toggle_callbacks (ClutterActor *ignored)
{ dialog_toggle (NULL, cluttersmith->dialog_callbacks); }


static MxAction *dialog_toggle_action (const gchar *name,
                                         ClutterActor *actor)
{
  MxAction *action;
  gchar *label;
  if (actor)
    label = g_strdup_printf ("%s %s",
            clutter_actor_get_paint_visibility (actor)?"hide":"show",
            name);
  else
    label = g_strdup (name);
  action = mx_action_new_full (label, label, G_CALLBACK (dialog_toggle), actor);
  g_free (label);
  return action;
}



/******************************************************************************/

typedef struct KeyBinding {
  ClutterModifierType modifier;
  guint key_symbol;
  void (*callback) (ClutterActor *actor);
  gpointer callback_data;
} KeyBinding;

static KeyBinding keybindings[]={
  {CLUTTER_CONTROL_MASK, CLUTTER_x,         cs_cut},
  {CLUTTER_CONTROL_MASK, CLUTTER_c,         cs_copy},
  {CLUTTER_CONTROL_MASK, CLUTTER_v,         cs_paste},
  {CLUTTER_CONTROL_MASK, CLUTTER_d,         cs_duplicate},
  {CLUTTER_CONTROL_MASK, CLUTTER_l,         cs_focus_title},


  /* check for the more specific modifier state before the more generic ones */
  {CLUTTER_CONTROL_MASK|
   CLUTTER_SHIFT_MASK,   CLUTTER_g,         cs_ungroup},
  {CLUTTER_CONTROL_MASK, CLUTTER_g,         (void*)cs_group},
  {0,                    CLUTTER_Escape,    cs_select_parent},

  {0,                    CLUTTER_BackSpace, cs_remove},
  {0,                    CLUTTER_Delete,    cs_remove},
  {0,                    CLUTTER_Page_Up,   cs_raise},
  {0,                    CLUTTER_Page_Down, cs_lower},
  {0,                    CLUTTER_Home,      cs_raise_top},
  {0,                    CLUTTER_End,       cs_lower_bottom},

  {0,                    CLUTTER_Return,    cs_edit},

  {0,                    CLUTTER_Up,        cs_keynav_up},
  {0,                    CLUTTER_Down,      cs_keynav_down},
  {0,                    CLUTTER_Left,      cs_keynav_left},
  {0,                    CLUTTER_Right,     cs_keynav_right},

  {0,                    CLUTTER_F1,        dialog_toggle_toolbar},
  {0,                    CLUTTER_F2,        dialog_toggle_sidebar},
  {0,                    CLUTTER_F3,        dialog_toggle_templates},
  {0,                    CLUTTER_F4,        dialog_toggle_scenes},
  {0,                    CLUTTER_F5,        dialog_toggle_editor},
  {0,                    CLUTTER_F6,        dialog_toggle_callbacks},
  {0,                    CLUTTER_F12,       dialog_toggle_config},

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
          keybindings[i].callback (keybindings[i].callback_data);
          return TRUE;
        }
    }
  return FALSE;
}


static KeyBinding global_keybindings[]={
  {CLUTTER_CONTROL_MASK, CLUTTER_q,           cs_quit},
  {0,                    CLUTTER_Scroll_Lock, cs_ui_mode},


  /* XXX: these shouldnt be here, they will interfere with the
   * app in browse mode,. but it is here now to steal them
   * from various entires that also want select all
   */
  {CLUTTER_CONTROL_MASK|
   CLUTTER_SHIFT_MASK,   CLUTTER_a,         cs_select_none},
  {CLUTTER_CONTROL_MASK, CLUTTER_a,         cs_select_all},

  {0, 0, NULL},
};


gboolean manipulator_key_pressed_global (ClutterActor *stage, ClutterModifierType modifier, guint key)
{
  gint i;
  for (i=0; global_keybindings[i].key_symbol; i++)
    {
      if (global_keybindings[i].key_symbol == key &&
          ((global_keybindings[i].modifier & modifier) == global_keybindings[i].modifier))
        {
          global_keybindings[i].callback (global_keybindings[i].callback_data);
          return TRUE;
        }
    }
  return FALSE;
}



static gboolean delayed_destroy (gpointer actor)
{
  clutter_actor_destroy (actor);
  return FALSE;
}

static void popup_menu_hidden (gpointer popup)
{
  g_idle_add (delayed_destroy, popup);
}

static void popup_action_activated (gpointer a, gpointer b)
{
  clutter_actor_hide (a);
}

static MxPopup *cs_popup_new (void)
{
  MxPopup *popup;
  popup = MX_POPUP (mx_popup_new ());
  g_signal_connect (popup, "action-activated", G_CALLBACK (popup_action_activated), NULL);
  g_signal_connect (popup, "hide", G_CALLBACK (popup_menu_hidden), NULL);
  return popup;
}


static void foo (void)
{
  g_print ("hoi\n");
}

static void cs_save_dialog_state2 (gpointer ignored)
{
  cs_save_dialog_state ();
}

void dialogs_popup (gint x,
                    gint y)
{
  MxPopup *popup = cs_popup_new ();
  MxAction *action;
  x = cs_last_x;
  y = cs_last_y;
  mx_popup_add_action (popup, mx_action_new_full ("Hide All",  "Hide All", foo,  NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Show All",  "Show All", foo,  NULL));

  action = dialog_toggle_action ("Toolbar (F1)", cluttersmith->dialog_toolbar);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Tree (F2)", cluttersmith->dialog_tree);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Property Editor (F2)", cluttersmith->dialog_property_inspector);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Templates (F3)", cluttersmith->dialog_templates);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Scenes (F4)", cluttersmith->dialog_scenes);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Editor (F5)", cluttersmith->dialog_editor);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Callbacks (F6)", cluttersmith->dialog_callbacks);
  mx_popup_add_action (popup, action);

  action = dialog_toggle_action ("Config (F12)", cluttersmith->dialog_config);
  mx_popup_add_action (popup, action);


  mx_popup_add_action (popup, mx_action_new_full ("", "", NULL, NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Remember positions and visibility",
      "Remember positions and visibility", G_CALLBACK (cs_save_dialog_state2), NULL));
  clutter_group_add (cluttersmith->parasite_root, popup);
  clutter_actor_set_position (CLUTTER_ACTOR (popup), x, y);
  clutter_actor_show (CLUTTER_ACTOR (popup));
}

void playback_popup (gint x,
                     gint y)
{
  MxPopup *popup = cs_popup_new ();
  MxAction *action;

  action = mx_action_new_full ("Edit (scrollock)", "Edit (scrollock)", G_CALLBACK (cs_ui_mode),  NULL);
  mx_popup_add_action (popup, action);
  action = mx_action_new_full ("Quit (ctrl q)", "Quit (ctrl q)", G_CALLBACK (cs_quit),  NULL);
  mx_popup_add_action (popup, action);
  clutter_group_add (cluttersmith->parasite_root, popup);
  clutter_actor_set_position (CLUTTER_ACTOR (popup), x, y);
  clutter_actor_show (CLUTTER_ACTOR (popup));
}

void root_popup (gint x,
                 gint y)
{
  MxPopup *popup = cs_popup_new ();
  MxAction *action;

  action = mx_action_new_full ("Browse (scrollock)", "Browse (scrollock)", G_CALLBACK (cs_ui_mode),  NULL);
  mx_popup_add_action (popup, action);

  action = mx_action_new_full ("reset view", "reset view", G_CALLBACK (cs_view_reset),  NULL);
  mx_popup_add_action (popup, action);

  if (clipboard)
    mx_popup_add_action (popup, mx_action_new_full ("Paste (ctrl v)", "Paste (ctrl v)", G_CALLBACK (cs_paste), NULL));
  action = mx_action_new_full ("Dialogs", "Dialogs", G_CALLBACK (dialogs_popup),  NULL);
  mx_popup_add_action (popup, action);
  action = mx_action_new_full ("Quit (ctrl q)", "Quit (ctrl q)", G_CALLBACK (cs_quit),  NULL);
  mx_popup_add_action (popup, mx_action_new_full ("Select All (ctrl a)", "Select All (ctrl a)", G_CALLBACK (cs_select_all), NULL));
  mx_popup_add_action (popup, action);

  clutter_group_add (cluttersmith->parasite_root, popup);
  clutter_actor_set_position (CLUTTER_ACTOR (popup), x, y);
  clutter_actor_show (CLUTTER_ACTOR (popup));

  if (cs_get_current_container () != cluttersmith->fake_stage)
    mx_popup_add_action (popup, mx_action_new_full ("Move up in tree (Escape)", "Move up in tree (Escape)", G_CALLBACK (cs_select_parent), NULL));
}

static void add_common (MxPopup *popup)
{
  MxAction *action;
  action = mx_action_new_full ("reset view", "reset view", G_CALLBACK (cs_view_reset),  NULL);
  mx_popup_add_action (popup, action);
  mx_popup_add_action (popup, mx_action_new_full ("______", "______", NULL, NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Edit (Return)", "Edit (Return)", G_CALLBACK (cs_edit), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Raise (PgUp)", "Raise (PgUp)", G_CALLBACK (cs_raise), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Send to front (Home)", "Send to front (Home)", G_CALLBACK (cs_raise_top), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Send to back (End)", "Send to back (End)", G_CALLBACK (cs_lower_bottom), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Lower (PgDn)", "Lower (PgDn)", G_CALLBACK (cs_lower), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("______", "_____", NULL, NULL));

  mx_popup_add_action (popup, mx_action_new_full ("Cut (ctrl x)", "Cut (ctrl x)", G_CALLBACK (cs_cut), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Copy (ctrl c)", "Copy (ctrl c)", G_CALLBACK (cs_copy), NULL));
  if (clipboard)
    mx_popup_add_action (popup, mx_action_new_full ("Paste (ctrl v)", "Paste (ctrl v)", G_CALLBACK (cs_paste), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Duplicate (ctrl d)", "Duplicate (ctrl d)", G_CALLBACK (cs_duplicate), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Remove (delete)", "Remove (delete)", G_CALLBACK (cs_remove), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("______", "______", NULL, NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Select All (ctrl a)", "Select All (ctrl a)", G_CALLBACK (cs_select_all), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Select None (shift ctrl a)", "Select None (shift ctrl a)", G_CALLBACK (cs_select_none), NULL));

  if (cs_get_current_container () != cluttersmith->fake_stage)
    mx_popup_add_action (popup, mx_action_new_full ("Move up in tree (ctrl p)", "Move up in tree (ctrl p)", G_CALLBACK (cs_select_parent), NULL));
}

void cs_change_type (ClutterActor *actor);

static gboolean is_link (ClutterActor *actor)
{
  return (actor && MX_IS_BUTTON (actor) &&
          mx_stylable_get_style_class (MX_STYLABLE (actor)) &&
          g_str_equal (mx_stylable_get_style_class (MX_STYLABLE (actor)),
                    "ClutterSmithLink"));
}

static void destination_set2 (MxAction *action,
                              gpointer    data)
{
  ClutterActor *actor = cs_selected_get_any ();
  ClutterActor *text;
  gchar *value;
  value = g_strdup_printf ("link=%s", mx_action_get_name (action));
  g_print ("%s\n", value);
  clutter_actor_set_name (actor, value);
  text = cs_container_get_child_no (CLUTTER_CONTAINER(actor), 0);
  clutter_text_set_text (CLUTTER_TEXT (text), mx_action_get_name (action));
  g_free (value);
  cs_dirtied ();
}

static MxAction *destination_set (const gchar *name)
{
  MxAction *action;
  gchar *label;
  label = g_strdup (name);
  action = mx_action_new_full (label, label, G_CALLBACK (destination_set2), NULL);
  g_free (label);
  return action;
}


void new_scene (MxAction *action,
                gpointer    ignored)
{
  ClutterActor *actor = cs_selected_get_any ();
  ClutterActor *text;
  if (!is_link (actor))
    return;
  text = cs_container_get_child_no (CLUTTER_CONTAINER(actor), 0);
  /* the name:link=link in the edited hyperlink is updated automatically */
  edit_text_start (text);
}

void link_edit_link (MxAction *action,
                     gpointer    ignored)
{
  MxPopup *popup = cs_popup_new ();
  gint x, y;
  x = cs_last_x;
  y = cs_last_y;


{
  GDir *dir;
  const gchar *path = cs_get_project_root ();
  const gchar *name;

  if (!path)
    return;

  action = mx_action_new_full ("new scene", "new scene", G_CALLBACK (new_scene), NULL);
  mx_popup_add_action (popup, action);

  dir = g_dir_open (path, 0, NULL);

  while ((name = g_dir_read_name (dir)))
    {
      gchar *name2;
      if (!g_str_has_suffix (name, ".json"))
        continue;
      name2 = g_strdup (name);
      *strstr (name2, ".json")='\0';

      action = destination_set (name2);
      g_free (name2);
      mx_popup_add_action (popup, action);
    }
  g_dir_close (dir);
}

  clutter_group_add (cluttersmith->parasite_root, popup);
  clutter_actor_set_position (CLUTTER_ACTOR (popup), x, y);
  clutter_actor_show (CLUTTER_ACTOR (popup));
}

void object_popup (ClutterActor *actor,
                   gint          x,
                   gint          y)
{
  MxPopup *popup = cs_popup_new ();


  if (is_link (actor))
    {
      gchar *label = "type: Link";
      mx_popup_add_action (popup, mx_action_new_full (label, label, NULL, NULL));
    }
  else
    {
      gchar *label = g_strdup_printf ("type: %s", G_OBJECT_TYPE_NAME (actor));
      mx_popup_add_action (popup, mx_action_new_full (label, label, G_CALLBACK (cs_change_type), NULL));
      g_free (label);
    }

  if (CLUTTER_IS_GROUP (actor))
    {
      mx_popup_add_action (popup, mx_action_new_full ("Ungroup (shift ctrl g)", "Ungroup (shift ctrl g)", G_CALLBACK (cs_ungroup), NULL));
      mx_popup_add_action (popup, mx_action_new_full ("Make box", "Make box", G_CALLBACK (cs_make_group_box), NULL));
    }
  else if (MX_IS_BUTTON (actor) &&
           mx_stylable_get_style_class (MX_STYLABLE (actor)) &&
           g_str_equal (mx_stylable_get_style_class (MX_STYLABLE (actor)),
                        "ClutterSmithLink"))
    {
      mx_popup_add_action (popup, mx_action_new_full ("Change destination", "Change destination", G_CALLBACK (link_edit_link), NULL));
    }
  else if (CLUTTER_IS_CONTAINER (actor))
    {
      mx_popup_add_action (popup, mx_action_new_full ("Make group", "Make group", G_CALLBACK (cs_make_group), NULL));
    }
  
  add_common (popup);

  clutter_group_add (cluttersmith->parasite_root, popup);
  clutter_actor_set_position (CLUTTER_ACTOR (popup), x, y);
  clutter_actor_show (CLUTTER_ACTOR (popup));
}

void selection_popup (gint x,
                      gint y)
{
  MxPopup *popup = cs_popup_new ();
  mx_popup_add_action (popup, mx_action_new_full ("Group (ctrl g)", "Group (ctrl g)", G_CALLBACK (cs_group), NULL));
  mx_popup_add_action (popup, mx_action_new_full ("Make box (ctrl b)", "Make box (ctrl b)", G_CALLBACK (cs_make_box), NULL));
  add_common (popup);

  clutter_group_add (cluttersmith->parasite_root, popup);
  clutter_actor_set_position (CLUTTER_ACTOR (popup), x, y);
  clutter_actor_show (CLUTTER_ACTOR (popup));
}
