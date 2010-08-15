/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Latent Status (Classification) Identifier
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
 * SECTION:attributes
 * @title:OscatsAttributes
 * @short_description: Attributes Container Class
 */

#include "attributes.h"

G_DEFINE_TYPE(OscatsAttributes, oscats_attributes, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_NUM,
};

static void oscats_attributes_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec);
static void oscats_attributes_get_property(GObject *object, guint prop_id,
                                           GValue *value, GParamSpec *pspec);

static void oscats_attributes_class_init (OscatsAttributesClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->set_property = oscats_attributes_set_property;
  gobject_class->get_property = oscats_attributes_get_property;

/**
 * OscatsLatentClass:num:
 *
 * Number of attributes.
 */
  pspec = g_param_spec_uint("num", "Size", 
                            "Number of attributes",
                            1, OSCATS_MAX_ATTRIBUTES, 1,
                            G_PARAM_READWRITE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

}

static void oscats_attributes_init (OscatsAttributes *self)
{
}

static void oscats_attributes_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsAttributes *self = OSCATS_ATTRIBUTES(object);
  switch (prop_id)
  {
    case PROP_NUM:
      self->num = g_value_get_uint(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_attributes_get_property(GObject *object, guint prop_id,
                                             GValue *value, GParamSpec *pspec)
{
  OscatsAttributes *self = OSCATS_ATTRIBUTES(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->num);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_attributes_from_string:
 * @str: a NULL-terminated string of whitespace, 0s, and 1s
 *
 * Converts a string of 1s and 0s to an #OscatsAttributes.
 *
 * Returns: the new #OscatsAttributes
 */
OscatsAttributes * oscats_attributes_from_string(const gchar *str)
{
  OscatsAttributes *attr;
  guint i, k, num = 0;
  g_return_val_if_fail(str != NULL, NULL);
  for (i=0; str[i] != '\0'; i++)
    if (str[i] == '0' || str[i] == '1') num++;
    else g_return_val_if_fail(str[i] == ' ' || str[i] == '\t' ||
                              str[i] == '\n' || str[i] == '\r', NULL);
  g_return_val_if_fail(num > 0 && num <= OSCATS_MAX_ATTRIBUTES, NULL);
  attr = g_object_new(OSCATS_TYPE_ATTRIBUTES, "num", num, NULL);
  for (i=0,k=0; str[i] != '\0'; i++)
    if (str[i] == '1') oscats_attributes_set(attr, k++, TRUE);
    else if (str[i] == '0') k++;
  return attr;
}

/**
 * oscats_attributes_set:
 * @attributes: an #OscatsAttributes object
 * @index: the attribute to set
 * @value: the attribute value
 *
 * Sets the attriute @index to @value.
 */
void oscats_attributes_set(OscatsAttributes *attributes,
                              guint8 index, gboolean value)
{
  g_return_if_fail(OSCATS_IS_ATTRIBUTES(attributes) && 
                   index < attributes->num);
  if (value)
    attributes->data |= 1 << index;
  else
    attributes->data &= ~(1 << index);
}

/**
 * oscats_attributes_get:
 * @attributes: an #OscatsAttributes object
 * @index: the the attribute to get
 *
 * Returns: the value of attribute @index
 */
gboolean oscats_attributes_get(const OscatsAttributes *attributes,
                                  guint8 index)
{
  g_return_val_if_fail(OSCATS_IS_ATTRIBUTES(attributes) && 
                       index < attributes->num, FALSE);
  return attributes->data & (1 << index);
}

/**
 * oscats_attributes_as_int:
 * @attributes: an #OscatsAttributes object
 *
 * Returns: the bit array of attributes, as an integer
 */
guint32 oscats_attributes_as_int(const OscatsAttributes *attributes)
{
  g_return_val_if_fail(OSCATS_IS_ATTRIBUTES(attributes), 0);
  return attributes->data;
}

/**
 * oscats_attributes_as_string:
 * @attributes: an #OscatsAttributes object
 *
 * Returns: a new string of 1s and 0s representing the attributes
 */
gchar * oscats_attributes_as_string(const OscatsAttributes *attributes)
{
  gchar *ret;
  int i;
  g_return_val_if_fail(OSCATS_IS_ATTRIBUTES(attributes), NULL);
  ret = g_new(gchar, attributes->num+1);
  for (i=0; i < attributes->num; i++)
    ret[i] = (oscats_attributes_get(attributes, i) ? '1' : '0');
  ret[i] = '\0';
  return ret;
}

/**
 * oscats_attributes_copy:
 * @lhs: an #OscatsAttributes object
 * @rhs: the #OscatsAttributes object to copy
 *
 * Copies the status @rhs to @lhs.
 */
void oscats_attributes_copy(OscatsAttributes *lhs,
                               const OscatsAttributes *rhs)
{
  g_return_if_fail(OSCATS_IS_ATTRIBUTES(lhs) &&
                   OSCATS_IS_ATTRIBUTES(rhs));
  lhs->num = rhs->num;
  lhs->data = rhs->data;
}

/**
 * oscats_attributes_compare:
 * @a: an #OscatsAttributes object
 * @b: an #OscatsAttributes object
 *
 * Compares the integer representation of @a and @b.
 *
 * Returns: 0 if @a == @b, -1 if @a < @b, and 1 if @a > @b.
 */
gint oscats_attributes_compare(OscatsAttributes *a, OscatsAttributes *b)
{
  g_return_val_if_fail(OSCATS_IS_ATTRIBUTES(a) && OSCATS_IS_ATTRIBUTES(b), 0);
  if (a->data == b->data) return 0;
  return (a->data < b->data ? -1 : 1);
}
