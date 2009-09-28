#include <clutter/clutter.h>
#include <nbtk/nbtk.h>
#include "cb-texture.h"

gpointer foo = NULL;

static void o(GType type)
{
  if (!G_TYPE_IS_INTERFACE (type))
    {
      foo = g_type_class_ref (type);
    }
}

void init_types(void){
o(CLUTTER_TYPE_ACTOR);
o(CLUTTER_TYPE_ALLOCATION_FLAGS);
o(CLUTTER_TYPE_ALPHA);
o(CLUTTER_TYPE_ANIMATABLE);
o(CLUTTER_TYPE_ANIMATION);
o(CLUTTER_TYPE_ANIMATION_MODE);
o(CLUTTER_TYPE_BACKEND);
o(CLUTTER_TYPE_BEHAVIOUR);
o(CLUTTER_TYPE_BEHAVIOUR_DEPTH);
o(CLUTTER_TYPE_BEHAVIOUR_ELLIPSE);
o(CLUTTER_TYPE_BEHAVIOUR_OPACITY);
o(CLUTTER_TYPE_BEHAVIOUR_PATH);
o(CLUTTER_TYPE_BEHAVIOUR_ROTATE);
o(CLUTTER_TYPE_BEHAVIOUR_SCALE);
o(CLUTTER_TYPE_BINDING_POOL);
o(CLUTTER_TYPE_CAIRO_TEXTURE);
o(CLUTTER_TYPE_CHILD_META);
o(CLUTTER_TYPE_CLONE);
o(CLUTTER_TYPE_CONTAINER);
o(CLUTTER_TYPE_EVENT_FLAGS);
o(CLUTTER_TYPE_EVENT_TYPE);
o(CLUTTER_TYPE_FEATURE_FLAGS);
o(CLUTTER_TYPE_FONT_FLAGS);
o(CLUTTER_TYPE_GRAVITY);
o(CLUTTER_TYPE_GROUP);
o(CLUTTER_TYPE_INIT_ERROR);
o(CLUTTER_TYPE_INPUT_DEVICE_TYPE);
o(CLUTTER_TYPE_INTERVAL);
o(CLUTTER_TYPE_LIST_MODEL);
o(CLUTTER_TYPE_MEDIA);
o(CLUTTER_TYPE_MODEL);
o(CLUTTER_TYPE_MODEL_ITER);
o(CLUTTER_TYPE_MODIFIER_TYPE);
o(CLUTTER_TYPE_PARAM_COLOR);
o(CLUTTER_TYPE_PARAM_FIXED);
o(CLUTTER_TYPE_PARAM_UNITS);
o(CLUTTER_TYPE_RECTANGLE);
o(CLUTTER_TYPE_STAGE);
o(CLUTTER_TYPE_TEXT);
o(CLUTTER_TYPE_TEXTURE);
o(NBTK_TYPE_BIN);
o(NBTK_TYPE_BOX_LAYOUT);
o(NBTK_TYPE_BUTTON);
o(NBTK_TYPE_CLIPBOARD);
o(NBTK_TYPE_COMBO_BOX);
o(NBTK_TYPE_DRAG_AXIS);
o(NBTK_TYPE_DRAG_CONTAINMENT);
o(NBTK_TYPE_DRAGGABLE);
o(NBTK_TYPE_DROPPABLE);
o(NBTK_TYPE_ENTRY);
o(NBTK_TYPE_EXPANDER);
o(NBTK_TYPE_GRID);
o(NBTK_TYPE_ICON);
o(NBTK_TYPE_ITEM_FACTORY);
o(NBTK_TYPE_ITEM_VIEW);
o(NBTK_TYPE_LABEL);
o(NBTK_TYPE_LIST_VIEW);
o(NBTK_TYPE_PROGRESS_BAR);
o(NBTK_TYPE_SCROLLABLE);
o(NBTK_TYPE_SCROLL_BAR);
o(NBTK_TYPE_SCROLL_VIEW);
o(NBTK_TYPE_STYLABLE);
o(NBTK_TYPE_STYLE);
o(NBTK_TYPE_STYLE_ERROR);
o(NBTK_TYPE_TABLE);
o(NBTK_TYPE_TABLE_CHILD);
o(NBTK_TYPE_TABLE_CHILD_OPTIONS);
o(NBTK_TYPE_TEXTURE_CACHE);
o(NBTK_TYPE_TEXTURE_FRAME);
o(NBTK_TYPE_TOOLTIP);
o(NBTK_TYPE_VIEWPORT);
o(NBTK_TYPE_WIDGET);
o(CB_TYPE_TEXTURE);
}
