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

G_DEFINE_TYPE(OscatsItem, oscats_item, OSCATS_TYPE_ADMINISTRAND);

enum
{
  PROP_0,
  PROP_CONT_MODEL,
  PROP_DISCR_MODEL,
};

static void oscats_item_constructed (GObject *object);
static void oscats_item_dispose (GObject *object);
static void oscats_item_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_item_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
                   
static void oscats_item_class_init (OscatsItemClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->constructed = oscats_item_constructed;
  gobject_class->dispose = oscats_item_dispose;
  gobject_class->set_property = oscats_item_set_property;
  gobject_class->get_property = oscats_item_get_property;
  
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
  self->cont_model = NULL;
  self->discr_model = NULL;
}

static void oscats_item_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsItem *self = OSCATS_ITEM(object);
  switch (prop_id)
  {
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
