/* OSCATS: Open-Source Computerized Adaptive Testing System
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

/**
 * SECTION:item
 * @title:OscatsItem
 * @short_description: Item Container Class
 */

#include "item.h"

G_DEFINE_TYPE(OscatsItem, oscats_item, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
  PROP_CONT_MODEL,
  PROP_DISCR_MODEL,
};

static gint ptr_compare(gconstpointer a, gconstpointer b) {  return b-a;  }

static void oscats_item_constructed (GObject *object);
static void oscats_item_dispose (GObject *object);
static void oscats_item_finalize (GObject *object);
static void oscats_item_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_item_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
                   
static void oscats_item_class_init (OscatsItemClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;
  GQuark q = 0;

  gobject_class->constructed = oscats_item_constructed;
  gobject_class->dispose = oscats_item_dispose;
  gobject_class->finalize = oscats_item_finalize;
  gobject_class->set_property = oscats_item_set_property;
  gobject_class->get_property = oscats_item_get_property;
  
  klass->items = g_tree_new(ptr_compare);
  klass->quark_to_char = g_hash_table_new(g_direct_hash, g_direct_equal);
  klass->char_to_quark = g_array_new(FALSE, FALSE, sizeof(GQuark));
  g_hash_table_insert(klass->quark_to_char, 0, 0);
  g_array_append_val(klass->char_to_quark, q);
  
/**
 * OscatsItem:id:
 *
 * A string identifier for the item.
 */
  pspec = g_param_spec_string("id", "ID", 
                            "String identifier for the item",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsItem:contmodel:
 *
 * The Continuous IRT model used for the item.  Either a continuous or
 * discrete model must be specified (or both).
 */
  pspec = g_param_spec_object("contmodel", "Continuous Model", 
                            "Continuous IRT model used for the item",
                            OSCATS_TYPE_CONT_MODEL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_CONT_MODEL, pspec);

/**
 * OscatsItem:discrmodel:
 *
 * The discrete IRT (classification) model used for the item.  Either a
 * continuous or discrete model must be specified (or both).
 */
  pspec = g_param_spec_object("discrmodel", "Discrete Model", 
                            "Discrete IRT (Classification) model used for the item",
                            OSCATS_TYPE_DISCR_MODEL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DISCR_MODEL, pspec);

}

static void oscats_item_init (OscatsItem *self)
{
  OscatsItemClass *klass = OSCATS_ITEM_GET_CLASS(self);
  self->characteristics = g_bit_array_new(
                              g_hash_table_size(klass->quark_to_char));
  g_tree_insert(klass->items, self, NULL);
}

static void oscats_item_constructed(GObject *object)
{
  OscatsItem *item = OSCATS_ITEM(object);
//  G_OBJECT_CLASS(oscats_item_parent_class)->constructed(object);

  if (!item->cont_model && !item->discr_model)
    g_critical("A Continous or Discrete Model must be specified!");
  
  if ((item->cont_model && item->discr_model) &&
      (oscats_cont_model_get_max(item->cont_model) !=
       oscats_discr_model_get_max(item->discr_model)))
    g_critical("Continous and Discrete Models do not have compatible response categories!");

}

static void oscats_item_dispose (GObject *object)
{
  OscatsItem *self = OSCATS_ITEM(object);
  G_OBJECT_CLASS(oscats_item_parent_class)->dispose(object);
  if (self->cont_model) g_object_unref(self->cont_model);
  if (self->discr_model) g_object_unref(self->discr_model);
  if (self->characteristics) g_object_unref(self->characteristics);
  self->cont_model = NULL;
  self->discr_model = NULL;
  self->characteristics = NULL;
  g_tree_remove(OSCATS_ITEM_GET_CLASS(self)->items, self);
}

static void oscats_item_finalize (GObject *object)
{
  OscatsItem *self = OSCATS_ITEM(object);
  g_free(self->id);
  G_OBJECT_CLASS(oscats_item_parent_class)->finalize(object);
}

static void oscats_item_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsItem *self = OSCATS_ITEM(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(18);
        g_string_printf(id, "[Item %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    case PROP_CONT_MODEL:		// construction only
      self->cont_model = g_value_dup_object(value);
      break;

    case PROP_DISCR_MODEL:		// construction only
      self->discr_model = g_value_dup_object(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_item_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsItem *self = OSCATS_ITEM(object);
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
    
    case PROP_CONT_MODEL:
      g_value_set_object(value, self->cont_model);
      break;

    case PROP_DISCR_MODEL:
      g_value_set_object(value, self->discr_model);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static gboolean kill_characteristics(gpointer key, gpointer val, gpointer data)
{
  OscatsItem *item = (OscatsItem*)key;
  g_bit_array_resize(item->characteristics, 1);
  return FALSE;
}

static gboolean add_characteristic(gpointer key, gpointer val, gpointer data)
{
  OscatsItem *item = (OscatsItem*)key;
  g_bit_array_extend(item->characteristics, 1);
  return FALSE;
}

/**
 * oscats_item_reset_characteristics:
 *
 * Removes all characteristics from the internal characteristics table.
 * (Useful for more compact memory management if you shift from
 * one large set of characteristics to another in the same program.)
 */
void oscats_item_reset_characteristics()
{
  OscatsItemClass *klass = g_type_class_ref(OSCATS_TYPE_ITEM);
  g_hash_table_remove_all(klass->quark_to_char);
  g_hash_table_insert(klass->quark_to_char, 0, 0);
  g_array_set_size(klass->char_to_quark, 1);
  g_tree_foreach(klass->items, kill_characteristics, NULL);
  g_type_class_unref(klass);
}

/**
 * oscats_item_register_characteristic:
 * @characteristic: the #GQuark characteristic to register
 *
 * Adds @characteristic to the internal characteristics table.
 * It is more efficient to register all characteristics before creating
 * items than to add new characteristics after items already exist.
 */
void oscats_item_register_characteristic(GQuark characteristic)
{
  OscatsItemClass *klass = g_type_class_ref(OSCATS_TYPE_ITEM);
  int c = g_hash_table_size(klass->quark_to_char);
  g_hash_table_insert(klass->quark_to_char,
                      (gpointer)characteristic, (gpointer)c);
  g_array_append_val(klass->char_to_quark, characteristic);
  g_tree_foreach(klass->items, add_characteristic, NULL);
  g_type_class_unref(klass);
}

/**
 * oscats_item_characteristic_from_string:
 * @name: the string name of the characteristic
 *
 * A wrapper of g_quark_from_string() for language bindings.
 *
 * Returns: the characteristic as a #GQuark
 */
GQuark oscats_item_characteristic_from_string(const gchar *name)
{
  return g_quark_from_string(name);
}

/**
 * oscats_item_characteristic_as_string:
 * @characteristic: a #GQuark characteristic
 *
 * A wrapper of g_quark_to_string() for language bindings.
 *
 * Returns: the string form of @characteristic
 */
const gchar * oscats_item_characteristic_as_string(GQuark characteristic)
{
  return g_quark_to_string(characteristic);
}

/**
 * oscats_item_set_characteristic:
 * @item: an #OscatsItem
 * @characteristic: a #GQuark characteristic
 *
 * Indicate that @item has @characteristic.
 */
void oscats_item_set_characteristic(OscatsItem *item, GQuark characteristic)
{
  OscatsItemClass *klass;
  guint c;
  g_return_if_fail(OSCATS_IS_ITEM(item));
  klass = OSCATS_ITEM_GET_CLASS(item);
  c = (guint)g_hash_table_lookup(klass->quark_to_char, (gpointer)characteristic);
  if (c == 0)
  {
    c = g_hash_table_size(klass->quark_to_char);
    oscats_item_register_characteristic(characteristic);
  }
  g_bit_array_set_bit(item->characteristics, c);
}

/**
 * oscats_item_clear_characteristic:
 * @item: an #OscatsItem
 * @characteristic: a #GQuark characteristic
 *
 * Indicate that @item does not have @characteristic.
 */
void oscats_item_clear_characteristic(OscatsItem *item, GQuark characteristic)
{
  OscatsItemClass *klass;
  guint c;
  g_return_if_fail(OSCATS_IS_ITEM(item));
  klass = OSCATS_ITEM_GET_CLASS(item);
  c = (guint)g_hash_table_lookup(klass->quark_to_char, (gpointer)characteristic);
  if (c) g_bit_array_clear_bit(item->characteristics, c);
}

/**
 * oscats_item_clear_characteristics:
 * @item: an #OscatsItem
 *
 * Clear all characteristics for @item.
 */
void oscats_item_clear_characteristics(OscatsItem *item)
{
  g_return_if_fail(OSCATS_IS_ITEM(item));
  g_bit_array_reset(item->characteristics, FALSE);
}

/**
 * oscats_item_has_characteristic:
 * @item: an #OscatsItem
 * @characteristic: a #GQuark characteristic
 *
 * Returns: %TRUE if @item has @characteristic
 */
gboolean oscats_item_has_characteristic(OscatsItem *item, GQuark characteristic)
{
  g_return_val_if_fail(OSCATS_IS_ITEM(item), FALSE);
  return g_bit_array_get_bit(item->characteristics,
    (guint)g_hash_table_lookup(OSCATS_ITEM_GET_CLASS(item)->quark_to_char,
                               (gpointer)characteristic));
}

/**
 * oscats_item_characteristics_iter_reset
 * @item: an #OscatsItem
 *
 * Reset the characteristics iterator for @item.
 */
void oscats_item_characteristics_iter_reset(OscatsItem *item)
{
  g_return_if_fail(OSCATS_IS_ITEM(item));
  g_bit_array_iter_reset(item->characteristics);
}

/**
 * oscats_item_characteristics_iter_next
 * @item: an #OscatsItem
 *
 * Returns: the next #GQuark characteristic that @item has, or 0 if @item
 * has no more characteristics
 */
GQuark oscats_item_characteristics_iter_next(OscatsItem *item)
{
  gint index;
  g_return_val_if_fail(OSCATS_IS_ITEM(item), 0);
  index = g_bit_array_iter_next(item->characteristics);
  return (index < 0 ? 0 :
          g_array_index(OSCATS_ITEM_GET_CLASS(item)->char_to_quark,
                        GQuark, index) );
}
