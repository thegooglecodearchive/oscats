/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Administrand Class
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
 * SECTION:administrand
 * @title:OscatsAdministrand
 * @short_description: Administrand Container Class
 */

#include "administrand.h"

G_DEFINE_TYPE(OscatsAdministrand, oscats_administrand, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
};

static gboolean static_initialized = FALSE;
static GTree *administrands = NULL;
static GHashTable *quark_to_char = NULL;
static GArray *char_to_quark = NULL;

static gint ptr_compare(gconstpointer a, gconstpointer b) {  return b-a;  }

static void oscats_administrand_dispose (GObject *object);
static void oscats_administrand_finalize (GObject *object);
static void oscats_administrand_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_administrand_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);

static inline void initialize_static()
{
  if (!static_initialized)
  {
    GQuark q = 0;
    administrands = g_tree_new(ptr_compare);
    quark_to_char = g_hash_table_new(g_direct_hash, g_direct_equal);
    char_to_quark = g_array_new(FALSE, FALSE, sizeof(GQuark));
    g_hash_table_insert(quark_to_char, 0, 0);
    g_array_append_val(char_to_quark, q);
    static_initialized = TRUE;
  }
}
                   
static gboolean ret_false(const OscatsAdministrand *item) { return FALSE; }
static guint ret_zero(const OscatsAdministrand *item) { return 0; }

static void oscats_administrand_class_init (OscatsAdministrandClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_administrand_dispose;
  gobject_class->finalize = oscats_administrand_finalize;
  gobject_class->set_property = oscats_administrand_set_property;
  gobject_class->get_property = oscats_administrand_get_property;
  
  klass->is_cont = ret_false;
  klass->is_discr = ret_false;
  klass->num_dims = ret_zero;
  klass->num_attrs = ret_zero;

  initialize_static();
  
/**
 * OscatsAdministrand:id:
 *
 * A string identifier for the administrand.
 */
  pspec = g_param_spec_string("id", "ID", 
                              "String identifier for the administrand",
                              NULL,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

}

static void oscats_administrand_init (OscatsAdministrand *self)
{
  self->characteristics = g_bit_array_new(g_hash_table_size(quark_to_char));
  g_tree_insert(administrands, self, NULL);
}

static void oscats_administrand_dispose (GObject *object)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  G_OBJECT_CLASS(oscats_administrand_parent_class)->dispose(object);
  if (self->characteristics) g_object_unref(self->characteristics);
  self->characteristics = NULL;
  g_tree_remove(administrands, self);
}

static void oscats_administrand_finalize (GObject *object)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  g_free(self->id);
  G_OBJECT_CLASS(oscats_administrand_parent_class)->finalize(object);
}

static void oscats_administrand_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(18);
        g_string_printf(id, "[Administrand %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_administrand_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
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

static gboolean kill_characteristics(gpointer key, gpointer val, gpointer data)
{
  OscatsAdministrand *administrand = (OscatsAdministrand*)key;
  g_bit_array_resize(administrand->characteristics, 1);
  return FALSE;
}

static gboolean add_characteristic(gpointer key, gpointer val, gpointer data)
{
  OscatsAdministrand *administrand = (OscatsAdministrand*)key;
  g_bit_array_extend(administrand->characteristics, 1);
  return FALSE;
}

/**
 * oscats_administrand_reset_characteristics:
 *
 * Removes all characteristics from the internal characteristics table.
 * (Useful for more compact memory management if you shift from
 * one large set of characteristics to another in the same program.)
 */
void oscats_administrand_reset_characteristics()
{
  if (static_initialized)
  {
    g_hash_table_remove_all(quark_to_char);
    g_hash_table_insert(quark_to_char, 0, 0);
    g_array_set_size(char_to_quark, 1);
    g_tree_foreach(administrands, kill_characteristics, NULL);
  }
}

/**
 * oscats_administrand_register_characteristic:
 * @characteristic: the #GQuark characteristic to register
 *
 * Adds @characteristic to the internal characteristics table.  It is more
 * efficient to register all characteristics before creating administrands
 * than to add new characteristics after administrands already exist.
 */
void oscats_administrand_register_characteristic(GQuark characteristic)
{
  int c = (static_initialized ? g_hash_table_size(quark_to_char) : 1);
  initialize_static();
  g_hash_table_insert(quark_to_char, (gpointer)characteristic, (gpointer)c);
  g_array_append_val(char_to_quark, characteristic);
  g_tree_foreach(administrands, add_characteristic, NULL);
}

/**
 * oscats_administrand_characteristic_from_string:
 * @name: the string name of the characteristic
 *
 * A wrapper of g_quark_from_string() for language bindings.
 *
 * Returns: the characteristic as a #GQuark
 */
GQuark oscats_administrand_characteristic_from_string(const gchar *name)
{
  return g_quark_from_string(name);
}

/**
 * oscats_administrand_characteristic_as_string:
 * @characteristic: a #GQuark characteristic
 *
 * A wrapper of g_quark_to_string() for language bindings.
 *
 * Returns: the string form of @characteristic
 */
const gchar * oscats_administrand_characteristic_as_string(GQuark characteristic)
{
  return g_quark_to_string(characteristic);
}

/**
 * oscats_administrand_set_characteristic:
 * @administrand: an #OscatsAdministrand
 * @characteristic: a #GQuark characteristic
 *
 * Indicate that @administrand has @characteristic.
 */
void oscats_administrand_set_characteristic(OscatsAdministrand *administrand, GQuark characteristic)
{
  guint c;
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  c = (guint)g_hash_table_lookup(quark_to_char, (gpointer)characteristic);
  if (c == 0)
  {
    c = g_hash_table_size(quark_to_char);
    oscats_administrand_register_characteristic(characteristic);
  }
  g_bit_array_set_bit(administrand->characteristics, c);
}

/**
 * oscats_administrand_clear_characteristic:
 * @administrand: an #OscatsAdministrand
 * @characteristic: a #GQuark characteristic
 *
 * Indicate that @administrand does not have @characteristic.
 */
void oscats_administrand_clear_characteristic(OscatsAdministrand *administrand, GQuark characteristic)
{
  guint c;
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  c = (guint)g_hash_table_lookup(quark_to_char, (gpointer)characteristic);
  if (c) g_bit_array_clear_bit(administrand->characteristics, c);
}

/**
 * oscats_administrand_clear_characteristics:
 * @administrand: an #OscatsAdministrand
 *
 * Clear all characteristics for @administrand.
 */
void oscats_administrand_clear_characteristics(OscatsAdministrand *administrand)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  g_bit_array_reset(administrand->characteristics, FALSE);
}

/**
 * oscats_administrand_has_characteristic:
 * @administrand: an #OscatsAdministrand
 * @characteristic: a #GQuark characteristic
 *
 * Returns: %TRUE if @administrand has @characteristic
 */
gboolean oscats_administrand_has_characteristic(OscatsAdministrand *administrand, GQuark characteristic)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), FALSE);
  return g_bit_array_get_bit(administrand->characteristics,
    (guint)g_hash_table_lookup(quark_to_char, (gpointer)characteristic));
}

/**
 * oscats_administrand_characteristics_iter_reset
 * @administrand: an #OscatsAdministrand
 *
 * Reset the characteristics iterator for @administrand.
 */
void oscats_administrand_characteristics_iter_reset(OscatsAdministrand *administrand)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  g_bit_array_iter_reset(administrand->characteristics);
}

/**
 * oscats_administrand_characteristics_iter_next
 * @administrand: an #OscatsAdministrand
 *
 * Returns: the next #GQuark characteristic that @administrand has, or 0 if @administrand
 * has no more characteristics
 */
GQuark oscats_administrand_characteristics_iter_next(OscatsAdministrand *administrand)
{
  gint index;
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), 0);
  index = g_bit_array_iter_next(administrand->characteristics);
  return (index < 0 ? 0 : g_array_index(char_to_quark, GQuark, index) );
}

/**
 * oscats_administrad_is_cont:
 * @admininstrand: an #OscatsAdmininstrand
 *
 * Returns: %TRUE if @administrand is compatible with continuous models.
 */
gboolean oscats_administrand_is_cont(const OscatsAdministrand *administrand)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(administrand)->is_cont(administrand);
}

/**
 * oscats_administrad_is_discr:
 * @admininstrand: an #OscatsAdmininstrand
 *
 * Returns: %TRUE if @administrand is compatible with discrete models.
 */
gboolean oscats_administrand_is_discr(const OscatsAdministrand *administrand)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(administrand)->is_discr(administrand);
}

/**
 * oscats_administrad_num_dims:
 * @admininstrand: an #OscatsAdmininstrand
 *
 * Returns: the number of (test) dimensions for the continous model used by
 * @administrand.
 */
guint oscats_administrand_num_dims(const OscatsAdministrand *administrand)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(administrand)->num_dims(administrand);
}

/**
 * oscats_administrad_num_attrs:
 * @admininstrand: an #OscatsAdmininstrand
 *
 * Returns: the number of (test) attributes for the discrete model used by
 * @administrand.
 */
guint oscats_administrand_num_attrs(const OscatsAdministrand *administrand)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(administrand)->num_attrs(administrand);
}
