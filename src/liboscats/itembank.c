/* OSCATS: Open-Source Computerized Adaptive Testing System
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

/**
 * SECTION:itembank
 * @title:OscatsItemBank
 * @short_description: Item Bank Class
 *
 * Note: Since an #OscatsItemBank is also an #OscatsAdministrand, it can be
 * tagged with characteristics and can also serve as an item group for
 * multi-stage testing or testlets, with appropriately coded algorithms.
 *
 * The total number of dimensions or attributes in the item group must be
 * the same for the IRT models (either continuous or discrete) of each item,
 * even though some items may load onto only a subset of the total number of
 * dimensions or attributes.  The dimension of the group is set when the
 * first item is added.
 */

#include "itembank.h"
#include "item.h"

G_DEFINE_TYPE(OscatsItemBank, oscats_item_bank, OSCATS_TYPE_ADMINISTRAND);

enum
{
  PROP_0,
  PROP_ID,
  PROP_SIZE_HINT,
};

static void oscats_item_bank_constructed (GObject *object);
static void oscats_item_bank_dispose (GObject *object);
static void oscats_item_bank_finalize (GObject *object);
static void oscats_item_bank_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_item_bank_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
                   
static gboolean is_cont(const OscatsAdministrand *self)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  OscatsAdministrand *item;
  if (!bank->items || bank->items->len == 0) return FALSE;
  item = g_ptr_array_index(bank->items, 0);
  return oscats_administrand_is_cont(item);
}

static gboolean is_discr(const OscatsAdministrand *self)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  OscatsAdministrand *item;
  if (!bank->items || bank->items->len == 0) return FALSE;
  item = g_ptr_array_index(bank->items, 0);
  return oscats_administrand_is_discr(item);
}

static guint num_dims(const OscatsAdministrand *bank)
{
  return OSCATS_ITEM_BANK(bank)->Ndims;
}

static guint num_attrs(const OscatsAdministrand *bank)
{
  return OSCATS_ITEM_BANK(bank)->Nattrs;
}

static guint max_resp(const OscatsAdministrand *self)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  OscatsAdministrand *item;
  guint i, k, max = 0;
  for (i=0; i < bank->items->len; i++)
  {
    item = g_ptr_array_index(bank->items, i);
    k = oscats_administrand_max_resp(item);
    if (k > max) max = k;
  }
  return max;
}

static void oscats_item_bank_class_init (OscatsItemBankClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsAdministrandClass *admin_class = OSCATS_ADMINISTRAND_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->constructed = oscats_item_bank_constructed;
  gobject_class->dispose = oscats_item_bank_dispose;
  gobject_class->finalize = oscats_item_bank_finalize;
  gobject_class->set_property = oscats_item_bank_set_property;
  gobject_class->get_property = oscats_item_bank_get_property;
  
  admin_class->is_cont = is_cont;
  admin_class->is_discr = is_discr;
  admin_class->num_dims = num_dims;
  admin_class->num_attrs = num_attrs;
  admin_class->max_resp = max_resp;

/**
 * OscatsItemBank:id:
 *
 * A string identifier for the item bank.
 */
  pspec = g_param_spec_string("id", "ID", 
                            "String identifier for the item bank",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsItemBank:sizeHint:
 *
 * Hint of how many items will be in the item bank.
 */
  pspec = g_param_spec_uint("sizeHint", "Size Hint", 
                            "Hint of how many items will be in the bank",
                            0, G_MAXUINT, 1,
                            G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIZE_HINT, pspec);

}

static void oscats_item_bank_init (OscatsItemBank *self)
{
}

static void oscats_item_bank_constructed (GObject *object)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
//  G_OBJECT_CLASS(oscats_item_bank_parent_class)->constructed(object);
  g_ptr_array_set_free_func(self->items, g_object_unref);
}

static void oscats_item_bank_dispose (GObject *object)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  G_OBJECT_CLASS(oscats_item_bank_parent_class)->dispose(object);
  if (self->items) g_ptr_array_unref(self->items);
  self->items = NULL;
}

static void oscats_item_bank_finalize (GObject *object)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  g_free(self->id);
  G_OBJECT_CLASS(oscats_item_bank_parent_class)->finalize(object);
}

static void oscats_item_bank_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(22);
        g_string_printf(id, "[ItemBank %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    case PROP_SIZE_HINT:		// construction only
      self->items = g_ptr_array_sized_new(g_value_get_uint(value));
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_item_bank_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_item_bank_add_item:
 * @bank: an #OscatsItemBank
 * @item: an #OscatsAdministrand
 *
 * Adds @item to the item bank @bank.  (Increases the @item reference count.)
 */
void oscats_item_bank_add_item(OscatsItemBank *bank, OscatsAdministrand *item)
{
  g_return_if_fail(OSCATS_IS_ITEM_BANK(bank) && OSCATS_IS_ADMINISTRAND(item));
  g_return_if_fail(bank->items);
  if (bank->items->len == 0)
  {
    if (oscats_administrand_is_cont(item))
      bank->Ndims = oscats_administrand_num_dims(item);
    if (oscats_administrand_is_discr(item))
      bank->Nattrs = oscats_administrand_num_attrs(item);
  }
  if (oscats_administrand_is_cont(item))
    g_return_if_fail(bank->Ndims == oscats_administrand_num_dims(item));
  if (oscats_administrand_is_discr(item))
    g_return_if_fail(bank->Nattrs == oscats_administrand_num_attrs(item));
  g_ptr_array_add(bank->items, item);
  g_object_ref(item);
}

/**
 * oscats_item_bank_num_items:
 * @bank: an #OscatsItemBank
 *
 * Returns: the number of items in the @bank
 */
guint oscats_item_bank_num_items(const OscatsItemBank *bank)
{
  g_return_val_if_fail(OSCATS_IS_ITEM_BANK(bank) && bank->items, 0);
  return bank->items->len;
}

/**
 * oscats_item_bank_is_pure:
 * @bank: an #OscatsItemBank
 *
 * Returns: %TRUE if @bank is composed exclusively of class #OscatsItem (or
 * has no items).
 */
gboolean oscats_item_bank_is_pure(const OscatsItemBank *bank)
{
  OscatsAdministrand *item;
  guint i;
  for (i=0; i < bank->items->len; i++)
  {
    item = g_ptr_array_index(bank->items, i);
    if (!OSCATS_IS_ITEM(item)) return FALSE;
  }
  return TRUE;
}

/**
 * oscats_item_bank_get_item:
 * @bank: an #OscatsItemBank
 * @i: item index
 *
 * Must have 0 < @i < number of items in the bank.
 * (Item's reference count is not increased.)
 *
 * Returns: the item @i
 */
const OscatsAdministrand * oscats_item_bank_get_item(const OscatsItemBank *bank, guint i)
{
  g_return_val_if_fail(OSCATS_IS_ITEM_BANK(bank) && bank->items &&
                       i < bank->items->len, NULL);
  return g_ptr_array_index(bank->items, i);
}
