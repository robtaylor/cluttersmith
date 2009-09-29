
#ifndef CLUTTERSMITH_H
#define CLUTTERSMITH_H

/* #define EDIT_SELF  */

typedef enum RunMode {
  CLUTTERSMITH_UI_MODE_BROWSE  = 0,
  CLUTTERSMITH_UI_MODE_UI      = 1,
  CLUTTERSMITH_UI_MODE_EDIT    = 2,
  CLUTTERSMITH_UI_MODE_CHROME  = CLUTTERSMITH_UI_MODE_UI|CLUTTERSMITH_UI_MODE_EDIT,
} RunMode;

#include "cluttersmith-context.h"

void
props_populate (ClutterActor *container,
                GObject      *object);

extern CluttersmithContext *cluttersmith;

/* Things that need to go in headers / get proper API: */

void cluttersmith_actor_editing_init (gpointer stage);


void   cluttersmith_set_project_root (const gchar *new_root);
gchar *cluttersmith_get_project_root (void);

void cluttersmith_open_layout (const gchar *new_title);
void cluttersmith_set_active (ClutterActor *item);
ClutterActor *cluttersmith_get_active (void);

void cluttersmith_tree_populate (ClutterActor *scene_graph,
                    ClutterActor *active_actor);

ClutterActor *cluttersmith_get_current_container (void);
void          cluttersmith_set_current_container (ClutterActor *actor);

void cluttersmith_set_ui_mode (guint ui_mode);
guint cluttersmith_get_ui_mode (void);

void cluttersmith_dirtied (void);

/* actor-editing: */

/* selection */

GList   *cluttersmith_selected_get_list  (void);
gint     cluttersmith_selected_count     (void);
gboolean cluttersmith_selected_has_actor (ClutterActor *actor);
void     cluttersmith_selected_clear     (void);
void     cluttersmith_selected_init      (void);
void     cluttersmith_selected_add       (ClutterActor *actor);
void     cluttersmith_selected_remove    (ClutterActor *actor);
void     cluttersmith_selected_foreach   (GCallback     cb,
                                          gpointer      data);
gboolean cluttersmith_save_timeout       (gpointer      data);
gpointer cluttersmith_selected_match     (GCallback     match_fun,
                                          gpointer      data);
void cs_load_dialog_state (void);
void cs_save_dialog_state (void);
ClutterActor *cluttersmith_selected_get_any (void);

/*****/

void previews_reload (ClutterActor *actor);
char * cluttersmith_make_config_file (const char *filename);


gboolean manipulator_key_pressed (ClutterActor *stage, ClutterModifierType modifier, guint key);
gboolean manipulator_key_pressed_global (ClutterActor *stage, ClutterModifierType modifier, guint key);

void cluttersmith_sync_chrome (void);

extern gint cs_last_x;
extern gint cs_last_y;

void playback_popup  (gint          x,
                      gint          y);
void root_popup      (gint          x,
                      gint          y);
void object_popup    (ClutterActor *actor,
                      gint          x,
                      gint          y);
void selection_popup (gint          x,
                      gint          y);

#define EDITOR_LINE_HEIGHT 24

#endif
