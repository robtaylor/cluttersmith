
ClutterActor *util_show (const gchar *name);
ClutterActor *util_load_json (const gchar *name);
void util_dismiss_cb (ClutterActor *actor);
ClutterScript *util_get_script (ClutterActor *actor);
void util_replace_content2 (ClutterActor  *actor,
                            const gchar *name,
                            const gchar *new_script);
void util_remove_children (ClutterActor *actor);
gboolean util_has_ancestor (ClutterActor *actor,
                            ClutterActor *ancestor);
gboolean util_movable_press (ClutterActor  *actor,
                             ClutterEvent  *event);


/* creates an (internal) list of all the properties applied to actor */
void util_build_transient (ClutterActor *actor);
/* applies and frees a prior transient list of values created with
 * util_apply_transient
 */
void util_apply_transient (ClutterActor *actor);

ClutterActor *util_duplicator (ClutterActor *actor, ClutterActor *parent);
