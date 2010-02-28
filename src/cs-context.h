/* cluttersmith-context.h */

#ifndef _CS_CONTEXT_H
#define _CS_CONTEXT_H

#include <clutter/clutter.h>
#include <glib-object.h>
#include "clutter-states.h"

G_BEGIN_DECLS

#define CS_TYPE_CONTEXT cs_context_get_type()

#define CS_CONTEXT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  CS_TYPE_CONTEXT, CSContext))

#define CS_CONTEXT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  CS_TYPE_CONTEXT, CSContextClass))

#define CS_IS_CONTEXT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  CS_TYPE_CONTEXT))

#define CS_IS_CONTEXT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  CS_TYPE_CONTEXT))

#define CS_CONTEXT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  CS_TYPE_CONTEXT, CSContextClass))

typedef struct _CSContext CSContext;
typedef struct _CSContextClass CSContextClass;
typedef struct _CSContextPrivate CSContextPrivate;

struct _CSContext
{
  GObject parent;

  CSContextPrivate *priv;
  gint          ui_mode;
  GList        *state_machines;
  gchar        *project_root;

  ClutterActor *parasite_root;
  ClutterActor *parasite_ui;
  ClutterActor *fake_stage_canvas;
  ClutterActor *fake_stage;
  ClutterActor *project_title;
  ClutterActor *scene_graph;
  ClutterActor *property_editors;
  ClutterActor *property_editors_core;

  ClutterActor *active_panel;
  ClutterActor *active_container;
  ClutterActor *callbacks_container;
  ClutterActor *states_container;

  ClutterActor *cs_mode;
  ClutterActor *dialog_config;
  ClutterActor *dialog_callbacks;
  ClutterActor *dialog_toolbar;
  ClutterActor *dialog_property_inspector;
  ClutterActor *dialog_tree;

  ClutterActor *dialog_annotate;
  ClutterActor *dialog_editor_annotation;

  ClutterActor *dialog_type;
  ClutterActor *dialog_editor;
  ClutterActor *dialog_editor_text;
  ClutterActor *dialog_editor_error;
  ClutterActor *dialog_templates;
  ClutterActor *dialog_scenes;
  ClutterActor *dialog_export;
  ClutterActor *dialog_states;
  ClutterActor *dialog_animator;
  ClutterActor *animator_props;
  ClutterActor *animator_progress;
  ClutterActor *animator_editor;
  ClutterActor *source_state;
  ClutterActor *state_duration;

  ClutterActor *resize_handle;
  ClutterActor *move_handle;

  const gchar   *current_state;  /* interned string */
  ClutterStates *current_state_machine;

  ClutterAnimator *current_animator; /* (if any) */
  /* XXX: add currently edited state machine */
};


struct _CSContextClass
{
  GObjectClass parent_class;
};

GType cs_context_get_type (void);

void cs_prop_tweaked (GObject     *object,
                      const gchar *property_name);


gfloat cluttersmith_get_origin_x (void);
gfloat cluttersmith_get_origin_y (void);

CSContext *cs_context_new (void);

G_END_DECLS

#endif /* _CS_CONTEXT_H */
