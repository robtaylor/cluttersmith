/*
 * ClutterSmith - a visual authoring environment for clutter.
 * Copyright (c) 2009, Intel Corporation
 * Copyright (c) 2011, Codethink Ltd.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * Alternatively, you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, with the following additional permissions:
 *
 * 1. The copyright holders grant you an additional permission under Section 7 of the
 * GNU General Public License, version 3, exempting you from the
 * requirement in Section 6 of the GNU General Public License, version 3,
 * to accompany Corresponding Source with Installation Information for
 * the Program or any work based on the Program.  You are still required
 * to comply with all other Section 6 requirements to provide
 * Corresponding Source.
 *
 * 2. The copywrite holders grants you an additional permission under Section 7 of the
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
 * Written by: Øyvind Kolås <oyvind.kolas@intel.com>, Rob Taylor <rob.taylor@codethink.co.uk>
 */

#ifndef __CS_SELECTION_H__
#define __CS_SELECTION_H__

#include <glib.h>

typedef struct _CSSelection CSSelection;

struct _CSSelection
{
	GList        *selected;

	/* what would be added/removed by
 	 * current lasso, not yet paret of the selected
	 */
	GHashTable   *lassoed; 

	gint          lx, ly;
	gfloat        manipulate_x;
	gfloat        manipulate_y;

  	ClutterActor *lasso;
};

CSSelection * cs_selection_new();
void          cs_selection_free(CSSelection *s);

GList   *cs_selection_get_list  (CSSelection *s);
gint     cs_selection_count     (CSSelection *s);
gboolean cs_selection_has_actor (CSSelection *s, ClutterActor *actor);
void     cs_selection_clear     (CSSelection *s);
void     cs_selection_add       (CSSelection *s, ClutterActor *actor);
void     cs_selection_remove    (CSSelection *s, ClutterActor *actor);
void     cs_selection_foreach   (CSSelection *s, GCallback cb, gpointer data);
gpointer cs_selection_match     (CSSelection *s, GCallback match_fun, gpointer data);
ClutterActor *cs_selection_get_any (CSSelection *s);
gboolean cs_selection_lasso_start (CSSelection *s, ClutterActor  *actor, ClutterEvent  *event);
void     cs_selection_paint (CSSelection *s);

void     cs_select_area_action_pre2(CSSelection *s, GString *undo);
void     cs_select_area_action_post(CSSelection *s, const char *action_name, GString *undo, GString *redo);
ClutterActor * cs_selection_pick (CSSelection *s, gfloat x, gfloat y);

/* these are defined here to allow sharing the boilerplate 
 * among different .c files
 */

#define SELECT_ACTION_PRE2(sel) cs_select_area_action_pre2(sel, undo);

#define SELECT_ACTION_PRE(sel) \
  GString *undo = g_string_new ("");\
  GString *redo = g_string_new ("");\
  SELECT_ACTION_PRE2(sel);

#define SELECT_ACTION_POST(sel, action_name) \
  cs_select_area_action_post(sel, action_name, undo, redo); \
  undo = redo = NULL;


#endif
