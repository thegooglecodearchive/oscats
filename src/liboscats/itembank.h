/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Item Bank Class
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

#ifndef _LIBOSCATS_ITEMBANK_H_
#define _LIBOSCATS_ITEMBANK_H_
#include <glib.h>
#include "item.h"
G_BEGIN_DECLS

#define OSCATS_TYPE_ITEM_BANK		(oscats_item_bank_get_type())
#define OSCATS_ITEM_BANK(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ITEM_BANK, OscatsItemBank))
#define OSCATS_IS_ITEM_BANK(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ITEM_BANK))
#define OSCATS_ITEM_BANK_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ITEM_BANK, OscatsItemBankClass))
#define OSCATS_IS_ITEM_BANK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ITEM_BANK))
#define OSCATS_ITEM_BANK_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ITEM_BANK, OscatsItemBankClass))

typedef struct _OscatsItemBank OscatsItemBank;
typedef struct _OscatsItemBankClass OscatsItemBankClass;

struct _OscatsItemBank {
  GObject parent_instance;
  gchar *id;
  GPtrArray *items;
  /*< private >*/
  guint Ndims, Nattrs;
};

struct _OscatsItemBankClass {
  GObjectClass parent_class;
};

GType oscats_item_bank_get_type();

void oscats_item_bank_add_item(OscatsItemBank *bank, OscatsItem *item);
guint oscats_item_bank_num_dims(const OscatsItemBank *bank);
guint oscats_item_bank_num_attrs(const OscatsItemBank *bank);
guint oscats_item_bank_num_items(const OscatsItemBank *bank);
guint oscats_item_bank_max_response(const OscatsItemBank *bank);
const OscatsItem * oscats_item_bank_get_item(const OscatsItemBank *bank, guint i);

gboolean oscats_item_bank_is_cont(const OscatsItemBank *bank);
gboolean oscats_item_bank_is_discr(const OscatsItemBank *bank);

G_END_DECLS
#endif
