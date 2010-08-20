/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Item Class
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LIBOSCATS_ITEM_H_
#define _LIBOSCATS_ITEM_H_
#include <glib.h>
#include "contmodel.h"
#include "discrmodel.h"
G_BEGIN_DECLS

#define OSCATS_TYPE_ITEM		(oscats_item_get_type())
#define OSCATS_ITEM(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ITEM, OscatsItem))
#define OSCATS_IS_ITEM(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ITEM))
#define OSCATS_ITEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ITEM, OscatsItemClass))
#define OSCATS_IS_ITEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ITEM))
#define OSCATS_ITEM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ITEM, OscatsItemClass))

typedef struct _OscatsItem OscatsItem;
typedef struct _OscatsItemClass OscatsItemClass;

struct _OscatsItem {
  GObject parent_instance;
  gchar *id;
  OscatsContModel *cont_model;
  OscatsDiscrModel *discr_model;
  GBitArray *characteristics;
};

struct _OscatsItemClass {
  GObjectClass parent_class;
  GTree *items;
  GHashTable *quark_to_char;
  GArray *char_to_quark;
};

GType oscats_item_get_type();

void oscats_item_reset_characteristics();
void oscats_item_register_characteristic(GQuark characteristic);
GQuark oscats_item_characteristic_from_string(const gchar *name);
const gchar * oscats_item_characteristic_as_string(GQuark characteristic);

void oscats_item_set_characteristic(OscatsItem *item, GQuark characteristic);
void oscats_item_clear_characteristic(OscatsItem *item, GQuark characteristic);
void oscats_item_clear_characteristics(OscatsItem *item);
gboolean oscats_item_has_characteristic(OscatsItem *item, GQuark characteristic);

void oscats_item_characteristics_iter_reset(OscatsItem *item);
GQuark oscats_item_characteristics_iter_next(OscatsItem *item);

G_END_DECLS
#endif
