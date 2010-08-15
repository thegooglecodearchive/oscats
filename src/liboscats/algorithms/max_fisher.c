/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Select Item based on Fisher Information
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
#include "algorithms/max_fisher.h"
#include "irtmodel.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_TYPE,
};

G_DEFINE_TYPE(OscatsAlgMaxFisher, oscats_alg_max_fisher, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_max_fisher_dispose(GObject *object);
static void oscats_alg_max_fisher_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_max_fisher_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_max_fisher_class_init (OscatsAlgMaxFisherClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_max_fisher_dispose;
  gobject_class->set_property = oscats_alg_max_fisher_set_property;
  gobject_class->get_property = oscats_alg_max_fisher_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgMaxFisher:num:
 *
 * Number of items from which to choose.  If one, then the exact optimal
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgMaxFisher:num optimal items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgMaxFisher:A-opt:
 *
 * If true, use the A-optimality critierion.  Otherwise, use the
 * D-optimality criterion.
 */
  pspec = g_param_spec_boolean("A-opt", "A-optimality", 
                               "Use A-optimality instead of D-optimality",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_TYPE, pspec);

}

static void oscats_alg_max_fisher_init (OscatsAlgMaxFisher *self)
{
}

static void oscats_alg_max_fisher_dispose (GObject *object)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(object);
  G_OBJECT_CLASS(oscats_alg_max_fisher_parent_class)->dispose(object);
  if (self->chooser) g_object_unref(self->chooser);
  if (self->base) g_object_unref(self->base);
  if (self->work) g_object_unref(self->work);
  if (self->inv) g_object_unref(self->inv);
  if (self->perm) g_object_unref(self->perm);
  self->chooser = NULL;
  self->base = self->work = self->inv = NULL;
  self->perm = NULL;
}

static void oscats_alg_max_fisher_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->chooser = g_object_new(OSCATS_TYPE_ALG_CHOOSER,
                                   "num", g_value_get_uint(value), NULL);
      break;
    
    case PROP_TYPE:			// construction only
      self->A_opt = g_value_get_boolean(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_max_fisher_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->chooser->num);
      break;
    
    case PROP_TYPE:
      g_value_set_boolean(value, self->A_opt);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void initialize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(alg_data);
  if (self->base) g_gsl_matrix_set_all(self->base, 0);
  self->base_num = 0;
}

// This value will be minimized
static gdouble criterion(const OscatsItem *item,
                         const OscatsExaminee *e,
                         gpointer data)
{
  guint k;
  OscatsAlgMaxFisher *alg_data = (OscatsAlgMaxFisher*)data;
  if (alg_data->base)
    g_gsl_matrix_copy(alg_data->work, alg_data->base);
  else
    g_gsl_matrix_set_all(alg_data->work, 0);
  oscats_irt_model_fisher_inf(item->irt_model, e->theta_hat, e->covariates,
                              alg_data->work);
  if (item->irt_model->testDim == 1)
    return -alg_data->work->v->data[0];
    // max I_j(theta) <==> min -I_j(theta)
  if (alg_data->A_opt)
  {
    gdouble ret = 0, *data = alg_data->inv->v->data;
    guint stride = alg_data->inv->v->tda;
    g_gsl_matrix_invert(alg_data->work, alg_data->inv, alg_data->perm);
    for (k=0; k < item->irt_model->testDim; k++)
      ret += data[k*stride+k];
    return ret;
    // min tr{[sum I_j(theta)]^-1}
  } else  // D_optimality
    return -g_gsl_matrix_det(alg_data->work, alg_data->perm);
    // max det[sum I_j(theta)] <==> min -det[sum I_j(theta)]
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(alg_data);
  
  if (self->base)
    for (; self->base_num < e->items->len; self->base_num++)
      oscats_irt_model_fisher_inf(g_ptr_array_index(e->items, self->base_num),
                                  e->theta_hat, e->covariates, self->base);

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
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(alg_data);
  guint num = oscats_item_bank_num_dims(test->itembank);
  g_return_if_fail(oscats_item_bank_is_irt(test->itembank));

  self->work = g_gsl_matrix_new(num, num);
  if (num > 1)
  {
    self->base = g_gsl_matrix_new(num, num);
    self->inv = g_gsl_matrix_new(num, num);
    self->perm = g_gsl_permutation_new(num);
  }

  self->chooser->bank = test->itembank;
  self->chooser->criterion = criterion;

  g_signal_connect_data(test, "initialize", G_CALLBACK(initialize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_object_ref(alg_data);
}
                   
