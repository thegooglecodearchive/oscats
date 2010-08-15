/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Select Item based on Kullback-Leibler Divergence (Classification)
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

#include "random.h"
#include "algorithms/max_kl_class.h"
#include "classmodel.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_POSTERIOR,
  PROP_PRIOR,
};

G_DEFINE_TYPE(OscatsAlgMaxKlClass, oscats_alg_max_kl_class, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_max_kl_class_dispose(GObject *object);
static void oscats_alg_max_kl_class_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_max_kl_class_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_max_kl_class_class_init (OscatsAlgMaxKlClassClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_max_kl_class_dispose;
  gobject_class->set_property = oscats_alg_max_kl_class_set_property;
  gobject_class->get_property = oscats_alg_max_kl_class_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgMaxKlClass:num:
 *
 * Number of items from which to choose.  If one, then the exact optimal
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgMaxKlClass:num optimal items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgMaxKlClass:posterior:
 *
 * If true, use posterior-weighted KL index.
 */
  pspec = g_param_spec_boolean("posterior", "Posterior-weighted", 
                               "Use posterior-weighted KL index",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_POSTERIOR, pspec);

/**
 * OscatsAlgMaxKlClass:prior:
 *
 * Prior distribution for ability patterns as a vector of probabilities for
 * all 2^K attribute patterns (summing to 1).  Default: uniform.
 */
  pspec = g_param_spec_object("prior", "Prior for alpha", 
                              "Prior distribution for attributes",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE | 
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_PRIOR, pspec);

}

static void oscats_alg_max_kl_class_init (OscatsAlgMaxKlClass *self)
{
}

static void oscats_alg_max_kl_class_dispose (GObject *object)
{
  OscatsAlgMaxKlClass *self = OSCATS_ALG_MAX_KL_CLASS(object);
  G_OBJECT_CLASS(oscats_alg_max_kl_class_parent_class)->dispose(object);
  if (self->chooser) g_object_unref(self->chooser);
  if (self->attr) g_object_unref(self->attr);
  if (self->prior) g_object_unref(self->prior);
  self->chooser = NULL;
  self->attr = NULL;
  self->prior = NULL;
}

static void oscats_alg_max_kl_class_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxKlClass *self = OSCATS_ALG_MAX_KL_CLASS(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->chooser = g_object_new(OSCATS_TYPE_ALG_CHOOSER,
                                   "num", g_value_get_uint(value), NULL);
      break;
    
    case PROP_POSTERIOR:		// construction only
      self->posterior = g_value_get_boolean(value);
      break;
    
    case PROP_PRIOR:
    {
      GGslVector *prior = g_value_get_object(value);
      if (prior)
      {
        if (self->numAttrs > 0)
          g_return_if_fail(prior->v->size == (1 << self->numAttrs));
        g_object_ref(prior);
      } else {
        if (self->prior) g_object_unref(self->prior);
      }
      self->prior = prior;
      break;
    }
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_max_kl_class_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxKlClass *self = OSCATS_ALG_MAX_KL_CLASS(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->chooser->num);
      break;
    
    case PROP_POSTERIOR:
      g_value_set_boolean(value, self->posterior);
      break;
    
    case PROP_PRIOR:
      g_value_set_object(value, self->prior);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

// This value will be minimized!
// Return value has been negated accordingly.
static gdouble criterion(const OscatsItem *item,
                         const OscatsExaminee *e,
                         gpointer data)
{
  OscatsAlgMaxKlClass *alg_data = OSCATS_ALG_MAX_KL_CLASS(data);
  OscatsAttributes *attr = alg_data->attr;
  guint k, num = (1 << alg_data->numAttrs);
  guint max = oscats_class_model_get_max(item->class_model);
  guint stride = 1;
  gdouble p[max], *prior = NULL;
  gdouble L = 1, I = 0;
  
  if (alg_data->prior)
  {
    stride = alg_data->prior->v->stride;
    prior = alg_data->prior->v->data;
  }

  for (k=0; k <= max; k++)
    p[k] = oscats_class_model_P(item->class_model, k, e->class_hat);

  for (attr->data=0; attr->data < num; attr->data++)
  {
    if (alg_data->posterior)
    {
      for (L=1,k=0; k < e->items->len; k++)
        L *= oscats_class_model_P(
               OSCATS_ITEM(g_ptr_array_index(e->items, k))->class_model,
               e->resp->data[k],
               attr);
      if (prior)
        L *= prior[attr->data*stride];
    }

    for (k=0; k <= max; k++)
      I += p[k] * L *
           log(p[k]/oscats_class_model_P(item->class_model, k, attr));
  }

  return -I;
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  OscatsAlgMaxKlClass *self = OSCATS_ALG_MAX_KL_CLASS(alg_data);
  return oscats_alg_chooser_choose(self->chooser, e, eligible, alg_data);
}

/*
 * Note that unless someone does something naughty, alg_data will be of the
 * appropriate type, and test will be an OscatsTest.  The signal connections
 * should include oscats_algorithm_closure_finalize as the destruction
 * callback.  The first connection should take alg_data's reference.  Any
 * subsequent connections should be accompanied by g_object_ref(alg_data).
 */
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
  OscatsAlgMaxKlClass *self = OSCATS_ALG_MAX_KL_CLASS(alg_data);
  guint numAttrs = oscats_item_bank_num_attrs(test->itembank);
  g_return_if_fail(oscats_item_bank_is_class(test->itembank));

  if (self->posterior && self->prior)
    g_return_if_fail(self->prior->v->size == (1 << numAttrs));
  self->numAttrs = numAttrs;
  self->attr = g_object_new(OSCATS_TYPE_ATTRIBUTES, "num", numAttrs, NULL);

  self->chooser->bank = test->itembank;
  self->chooser->criterion = criterion;

  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
                   
