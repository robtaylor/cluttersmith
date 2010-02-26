#include "cluttersmith.h"
#include <animator-editor.h>

enum
{
  PROP_0,
};

struct _CsAnimatorEditorPrivate
{
  ClutterActor    *background;
  ClutterAnimator *animator;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                       CS_TYPE_ANIMATOR_EDITOR, \
                                                       CsAnimatorEditorPrivate))
G_DEFINE_TYPE (CsAnimatorEditor, cs_animator_editor, CLUTTER_TYPE_ACTOR)

static void
cs_animator_editor_finalize (GObject *object)
{
  G_OBJECT_CLASS (cs_animator_editor_parent_class)->finalize (object);
}

static void
cs_animator_editor_dispose (GObject *object)
{
    CsAnimatorEditor *editor = (CsAnimatorEditor *) object;
    CsAnimatorEditorPrivate *priv;
    priv = editor->priv;

    if (priv->animator)
      g_object_unref (priv->animator);
    priv->animator = NULL;

    G_OBJECT_CLASS (cs_animator_editor_parent_class)->dispose (object);
}

static void
cs_animator_editor_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
    switch (prop_id) {
    default:
        break;
    }
}

static void
cs_animator_editor_get_property (GObject *object, guint prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  switch (prop_id)
    {
      default:
        break;
    }
}

static void
cs_animator_editor_allocate (ClutterActor *actor, const ClutterActorBox *box,
                           ClutterAllocationFlags flags)
{
  CsAnimatorEditor        *editor = (CsAnimatorEditor *) actor;
  CsAnimatorEditorPrivate *priv   = editor->priv;

  CLUTTER_ACTOR_CLASS (cs_animator_editor_parent_class)->allocate
                             (actor, box, flags);

  if (priv->background ) 
      clutter_actor_allocate_preferred_size (priv->background,
                                             flags);
}

static void
cs_animator_editor_paint (ClutterActor *actor)
{
  CsAnimatorEditor        *editor = (CsAnimatorEditor *) actor;
  CsAnimatorEditorPrivate *priv   = editor->priv;
  GObject *currobject = NULL;
  const gchar *currprop = NULL;
  gint width = clutter_actor_get_width (actor);
  GList *k, *keys;
  gint propno = 0;

  if (priv->background)
     clutter_actor_paint (priv->background);

  if (!priv->animator)
    return;

  keys = clutter_animator_get_keys (priv->animator, NULL, NULL, -1);
  for (k = keys; k; k = k->next)
    {
      ClutterAnimatorKey *key = k->data;
      GObject *object = clutter_animator_key_get_object (key);
      const gchar *prop = g_intern_string (clutter_animator_key_get_property_name (key));
      gfloat progress = clutter_animator_key_get_progress (key);
      //guint mode = clutter_animator_key_get_mode (key);

      if (object != currobject ||
          prop != currprop)
        {
          currobject = object;
          currprop = prop;
          cogl_path_stroke ();
          cogl_path_new ();
          propno ++;
          cogl_path_move_to (progress * width, 10 * propno);
        }
      else
        {
          cogl_path_line_to (progress * width, 10 * propno);
          cogl_path_line_to (progress * width, 10 * propno + 2);
          cogl_path_line_to (progress * width, 10 * propno - 2);
          cogl_path_line_to (progress * width, 10 * propno);
          cogl_path_stroke ();
        }
      cogl_path_ellipse (progress * width, 10 * propno, 5, 5);
      cogl_path_fill ();
      cogl_path_move_to (progress * width, 10 * propno);
    }

  g_list_free (keys);
}

static void
cs_animator_editor_pick (ClutterActor *actor, const ClutterColor *color)
{
    cs_animator_editor_paint (actor);
}

static void
cs_animator_editor_map (ClutterActor *self)
{
    CsAnimatorEditorPrivate *priv = CS_ANIMATOR_EDITOR (self)->priv;

    CLUTTER_ACTOR_CLASS (cs_animator_editor_parent_class)->map (self);

    if (priv->background)
      clutter_actor_map (CLUTTER_ACTOR (priv->background));
}

static void
cs_animator_editor_unmap (ClutterActor *self)
{
    CsAnimatorEditorPrivate *priv = CS_ANIMATOR_EDITOR (self)->priv;

    CLUTTER_ACTOR_CLASS (cs_animator_editor_parent_class)->unmap (self);

    if (priv->background)
      clutter_actor_unmap (CLUTTER_ACTOR (priv->background));
}


static void
cs_animator_editor_class_init (CsAnimatorEditorClass *klass)
{
    GObjectClass      *o_class = (GObjectClass *) klass;
    ClutterActorClass *a_class = (ClutterActorClass *) klass;

    o_class->dispose      = cs_animator_editor_dispose;
    o_class->finalize     = cs_animator_editor_finalize;
    o_class->set_property = cs_animator_editor_set_property;
    o_class->get_property = cs_animator_editor_get_property;

    a_class->allocate = cs_animator_editor_allocate;
    a_class->paint    = cs_animator_editor_paint;
    a_class->pick     = cs_animator_editor_pick;
    a_class->map      = cs_animator_editor_map;
    a_class->unmap    = cs_animator_editor_unmap;

    g_type_class_add_private (klass, sizeof (CsAnimatorEditorPrivate));
}

static void
cs_animator_editor_init (CsAnimatorEditor *self)
{
  CsAnimatorEditorPrivate *priv = GET_PRIVATE (self);

  self->priv = priv;

  priv->background = clutter_rectangle_new ();
  priv->animator = NULL;
  clutter_actor_set_reactive (priv->background, TRUE);
  clutter_actor_set_parent (priv->background, CLUTTER_ACTOR (self));
  clutter_actor_show (priv->background);
}

void cs_animator_editor_set_animator (CsAnimatorEditor *animator_editor,
                                      ClutterAnimator  *animator)
{
  CsAnimatorEditorPrivate *priv = animator_editor->priv;
  if (priv->animator)
    g_object_unref (priv->animator);
  priv->animator = g_object_ref (animator);
}





static GList *handles = NULL;
typedef struct AnimatorHandle {
  ClutterAnimator *animator;
  GObject         *object;
  gint             key_no;

  ClutterActor    *actor;
} AnimatorHandle;

static gdouble manipulate_x;
static gdouble manipulate_y;

static gboolean
handle_move_capture (ClutterActor *stage,
                     ClutterEvent *event,
                     gpointer      data)
{
  AnimatorHandle *handle = data;
  switch (event->any.type)
    {
      case CLUTTER_MOTION:
        {
          gfloat delta[2];
          delta[0]=manipulate_x-event->motion.x;
          delta[1]=manipulate_y-event->motion.y;
          cs_dirtied ();

          {
            GList *xlist = clutter_animator_get_keys (handle->animator,
                                                      handle->object,
                                                      "x", -1);
            GList *ylist = clutter_animator_get_keys (handle->animator,
                                                      handle->object,
                                                      "y", -1);
            ClutterAnimatorKey *xkey;
            ClutterAnimatorKey *ykey;
            GValue              xvalue = {0, };
            GValue              yvalue = {0, };
            gdouble progress;

            g_value_init (&xvalue, G_TYPE_FLOAT);
            g_value_init (&yvalue, G_TYPE_FLOAT);

            xkey = g_list_nth_data (xlist, handle->key_no);
            ykey = g_list_nth_data (ylist, handle->key_no);
            g_list_free (xlist);
            g_list_free (ylist);

            progress = clutter_animator_key_get_progress (xkey);

            clutter_animator_key_get_value (xkey, &xvalue);
            clutter_animator_key_get_value (ykey, &yvalue);

            clutter_animator_set (handle->animator,
                                  handle->object, "x",
                                  CLUTTER_LINEAR,
                                  progress, 
                                  g_value_get_float (&xvalue) - delta[0],
                                  
                                  NULL);
            clutter_animator_set (handle->animator,
                                  handle->object, "y",
                                  CLUTTER_LINEAR,
                                  progress, 
                                  g_value_get_float (&yvalue) - delta[1],
                                  NULL);

            g_value_unset (&xvalue);
            g_value_unset (&yvalue);
          }

          manipulate_x=event->motion.x;
          manipulate_y=event->motion.y;
        }
        break;
      case CLUTTER_BUTTON_RELEASE:
        g_signal_handlers_disconnect_by_func (stage, handle_move_capture, data);

        if (clutter_event_get_button (event)==3)
          {
            GList *xlist = clutter_animator_get_keys (handle->animator,
                                                      handle->object,
                                                      "x", -1);
            GList *ylist = clutter_animator_get_keys (handle->animator,
                                                      handle->object,
                                                      "y", -1);
            ClutterAnimatorKey *xkey;
            ClutterAnimatorKey *ykey;
            gdouble progress;

            xkey = g_list_nth_data (xlist, handle->key_no);
            ykey = g_list_nth_data (ylist, handle->key_no);
            g_list_free (xlist);
            g_list_free (ylist);
            progress = clutter_animator_key_get_progress (xkey);

            clutter_animator_remove_key (handle->animator,
                                         handle->object, "x", progress);
            if (ykey)
            clutter_animator_remove_key (handle->animator,
                                         handle->object, "y", progress);
          }

        clutter_actor_queue_redraw (stage);
      default:
        break;
    }
  return TRUE;
}

static gboolean handle_event (ClutterActor *actor,
                              ClutterEvent *event,
                              gpointer      data)
{
  AnimatorHandle *handle = data;

  switch (clutter_event_type (event))
    {
      case CLUTTER_BUTTON_PRESS:
        manipulate_x = event->button.x;
        manipulate_y = event->button.y;

        g_signal_connect (clutter_actor_get_stage (actor), "captured-event",
                          G_CALLBACK (handle_move_capture), handle);
        return TRUE;
      case CLUTTER_ENTER:
        clutter_actor_set_opacity (actor, 0xff);
        break;
      case CLUTTER_LEAVE:
        clutter_actor_set_opacity (actor, 0xaa);
        break;
      default:
        break;
    }
  return FALSE;
}


static void ensure_animator_handle (ClutterAnimator *animator,
                                    GObject         *object,
                                    gfloat           x,
                                    gfloat           y,
                                    gint             key_no)
{
  AnimatorHandle *handle;
  GList *h;
  for (h = handles; h; h=h->next)
    {
      handle = h->data;
      if (handle->key_no == key_no)
        {
          if (object == NULL)
            {
              handles = g_list_remove (handles, handle);
              clutter_actor_destroy (handle->actor);
              g_free (handle);
              return;
            }
          break;
        }
      else
        handle = NULL;
    }
  if (!object)
    return;
  if (!handle)
    {
      handle = g_new0 (AnimatorHandle, 1);
      handle->key_no = key_no;
      handle->actor = clutter_texture_new_from_file (PKGDATADIR "link-bg.png", NULL);
      clutter_actor_set_opacity (handle->actor, 0xaa);
      clutter_actor_set_anchor_point_from_gravity (handle->actor, CLUTTER_GRAVITY_CENTER);
      clutter_actor_set_size (handle->actor, 20, 20);
      clutter_container_add_actor (CLUTTER_CONTAINER (cluttersmith->parasite_root),
                                   handle->actor);
      handles = g_list_append (handles, handle);
      clutter_actor_set_reactive (handle->actor, TRUE);
      g_signal_connect   (handle->actor, "event", G_CALLBACK (handle_event), handle);
    }
  handle->object = object;
  handle->animator = animator;
  clutter_actor_set_position (handle->actor, x, y);
}

void animator_editor_update_handles (void)
{
  /* Update positions of drag handles */
  if (cluttersmith->current_animator)
    {
      ClutterActor *actor = cs_selected_get_any ();
      gint i = 0;
      if (actor)
        {
          GValue xv = {0, };
          GValue yv = {0, };

          g_value_init (&xv, G_TYPE_FLOAT);
          g_value_init (&yv, G_TYPE_FLOAT);

          {
            GList *k, *keys;

            keys = clutter_animator_get_keys (cluttersmith->current_animator,
                                              G_OBJECT (actor), "x", -1.0);
            for (k = keys, i = 0; k; k = k->next, i++)
              {
                gdouble progress = clutter_animator_key_get_progress (k->data);
                ClutterVertex vertex = {0, };
                gfloat x, y;
                clutter_animator_compute_value (cluttersmith->current_animator,
                                             G_OBJECT (actor), "x", progress, &xv);
                clutter_animator_compute_value (cluttersmith->current_animator,
                                             G_OBJECT (actor), "y", progress, &yv);
                x = g_value_get_float (&xv);
                y = g_value_get_float (&yv);
                vertex.x = x;
                vertex.y = y;

                clutter_actor_apply_transform_to_point (clutter_actor_get_parent (actor),
                                                        &vertex, &vertex);

                ensure_animator_handle (cluttersmith->current_animator,
                                        G_OBJECT (actor), vertex.x, vertex.y, i);
              }
            g_list_free (keys);
          }
          g_value_unset (&xv);
          g_value_unset (&yv);
        }
      for (; i<100; i++)
        {
          ensure_animator_handle (NULL, NULL, 0,0, i);
        }
    }
}
