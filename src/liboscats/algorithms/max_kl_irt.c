/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Select Item based on Kullback-Leibler Divergence (IRT)
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

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "random.h"
#include "algorithms/max_kl_irt.h"
#include "irtmodel.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_INF_BOUNDS,
  PROP_POSTERIOR,
  PROP_C,
  PROP_MU,
  PROP_SIGMA,
};

G_DEFINE_TYPE(OscatsAlgMaxKlIrt, oscats_alg_max_kl_irt, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_max_kl_irt_dispose(GObject *object);
static void oscats_alg_max_kl_irt_finalize(GObject *object);
static void oscats_alg_max_kl_irt_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_max_kl_irt_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_max_kl_irt_class_init (OscatsAlgMaxKlIrtClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_max_kl_irt_dispose;
  gobject_class->finalize = oscats_alg_max_kl_irt_finalize;
  gobject_class->set_property = oscats_alg_max_kl_irt_set_property;
  gobject_class->get_property = oscats_alg_max_kl_irt_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgMaxKlIrt:num:
 *
 * Number of items from which to choose.  If one, then the exact optimal
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgMaxKlIrt:num optimal items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgMaxKlIrt:inf-bounds:
 *
 * If true, integrate over the confidence ellipsoid.
 * Otherwise, integrate over the box theta.hat +/- c/sqrt(n).
 */
  pspec = g_param_spec_boolean("inf-bounds", "Fisher Information Bounds", 
                               "Integrate over confidence ellipsoid",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_INF_BOUNDS, pspec);

/**
 * OscatsAlgMaxKlIrt:posterior:
 *
 * If true, use posterior-weighted KL index.  (Note: if true, 
 * #OscatsAlgMaxKlIrt:inf-bounds is ignored.)
 */
  pspec = g_param_spec_boolean("posterior", "Posterior-weighted", 
                               "Use posterior-weighted KL index",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_POSTERIOR, pspec);

/**
 * OscatsAlgMaxKlIrt:c:
 *
 * The constant c in integration bounds.
 */
  pspec = g_param_spec_double("c", "Scaling constant", 
                              "The constant c in integration bounds",
                              1e-16, 1e16, 3,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_C, pspec);

/**
 * OscatsAlgMaxKlIrt:mu:
 *
 * Prior population mean for posterior weight.  Default: 0.
 */
  pspec = g_param_spec_object("mu", "Prior mean", 
                              "Prior population mean for posterior weight",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MU, pspec);

/**
 * OscatsAlgMaxKlIrt:Sigma:
 *
 * Prior population covariance matrix for posterior weight.
 * Default: identity.
 */
  pspec = g_param_spec_object("Sigma", "Prior covariance", 
                              "Prior population covariance matrix for posterior weight",
                              G_TYPE_GSL_MATRIX,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIGMA, pspec);

}

static void oscats_alg_max_kl_irt_init (OscatsAlgMaxKlIrt *self)
{
  self->integrator = g_object_new(OSCATS_TYPE_INTEGRATE, NULL);
}

static void oscats_alg_max_kl_irt_dispose (GObject *object)
{
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(object);
  G_OBJECT_CLASS(oscats_alg_max_kl_irt_parent_class)->dispose(object);
  if (self->chooser) g_object_unref(self->chooser);
  if (self->Inf) g_object_unref(self->Inf);
  if (self->integrator) g_object_unref(self->integrator);
  if (self->Inf_inv) g_object_unref(self->Inf_inv);
  self->chooser = NULL;
  self->Inf = NULL;
  self->integrator = NULL;
  self->Inf_inv = NULL;
}

static void oscats_alg_max_kl_irt_finalize (GObject *object)
{
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(object);
  if (self->mu) gsl_vector_free(self->mu);
  if (self->Sigma_half) gsl_matrix_free(self->Sigma_half);
  if (self->p) g_free(self->p);
  if (self->tmp) gsl_vector_free(self->tmp);
  if (self->tmp2) gsl_vector_free(self->tmp2);
  G_OBJECT_CLASS(oscats_alg_max_kl_irt_parent_class)->finalize(object);
}

static void oscats_alg_max_kl_irt_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->chooser = g_object_new(OSCATS_TYPE_ALG_CHOOSER,
                                   "num", g_value_get_uint(value), NULL);
      break;
    
    case PROP_INF_BOUNDS:		// construction only
      self->inf_bounds = g_value_get_boolean(value);
      break;
    
    case PROP_POSTERIOR:		// construction only
      self->posterior = g_value_get_boolean(value);
      break;
    
    case PROP_C:
      self->c = g_value_get_double(value);
      break;
    
    case PROP_MU:
    {
      GGslVector *mu = g_value_get_object(value);
      if (mu)
      {
        if (self->dims > 0) g_return_if_fail(mu->v->size == self->dims);
        if (self->mu == NULL) self->mu = gsl_vector_alloc(self->mu->size);
        gsl_vector_memcpy(self->mu, mu->v);
      } else {
        if (self->mu) gsl_vector_set_zero(self->mu);
      }
      break;
    }
    
    case PROP_SIGMA:
    {
      GGslMatrix *Sigma = g_value_get_object(value);
      if (Sigma)
      {
        g_return_if_fail(Sigma->v->size1 == Sigma->v->size2);
        if (self->dims > 0) g_return_if_fail(Sigma->v->size1 == self->dims);
        if (self->Sigma_half == NULL)
          self->Sigma_half = gsl_matrix_alloc(Sigma->v->size1, Sigma->v->size2);
        gsl_matrix_memcpy(self->Sigma_half, Sigma->v);
        gsl_linalg_cholesky_decomp(self->Sigma_half);
      } else {
        if (self->Sigma_half) gsl_matrix_set_identity(self->Sigma_half);
      }
      break;
    }
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_max_kl_irt_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->chooser->num);
      break;
    
    case PROP_INF_BOUNDS:
      g_value_set_boolean(value, self->inf_bounds);
      break;
    
    case PROP_POSTERIOR:
      g_value_set_boolean(value, self->posterior);
      break;
    
    case PROP_C:
      g_value_set_double(value, self->c);
      break;
    
    case PROP_MU:
      if (self->mu)
      {
        GGslVector *mu = g_gsl_vector_new(self->mu->size);
        gsl_vector_memcpy(mu->v, self->mu);
        g_value_take_object(value, mu);
      } else
        g_value_set_object(value, NULL);
      break;
    
    case PROP_SIGMA:
      if (self->Sigma_half)
      {
        GGslMatrix *Sigma = g_gsl_matrix_new(self->Sigma_half->size1,
                                             self->Sigma_half->size2);
        // set lower triangle of Sigma->v
        guint i, j;
        for (i=0; i < Sigma->v->size1; i++)
          for (j=0; j <= i; j++)
            Sigma->v->data[i*Sigma->v->tda+j] =
              self->Sigma_half->data[i*self->Sigma_half->tda+j];
        // Sigma = Sigma_half Sigma_half'
        gsl_blas_dtrmm(CblasRight, CblasTrans, CblasLower, CblasNonUnit,
                       1, self->Sigma_half, Sigma->v);
        g_value_take_object(value, Sigma);
      } else
        g_value_set_object(value, NULL);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void initialize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(alg_data);
  if (self->Inf) g_gsl_matrix_set_all(self->Inf, 0);
  self->base_num = 0;
  self->e = e;
}

static gdouble integrand(const GGslVector *theta, gpointer data)
{
  OscatsAlgMaxKlIrt *self = (OscatsAlgMaxKlIrt*)data;
  guint k;
  gdouble val = 0;
  for (k=0; k <= self->max; k++)
    val += self->p[k] * log(oscats_irt_model_P(self->model, k, theta,
                                               self->e->covariates));
  return (val - self->p_sum);
}

static gdouble integrand_posterior(const GGslVector *theta, gpointer data)
{
  OscatsAlgMaxKlIrt *self = (OscatsAlgMaxKlIrt*)data;
  GPtrArray *items = self->e->items;
  guint8 *resp = self->e->resp->data;
  guint k, num = self->e->items->len;
  gdouble g, L=1, sum=0;
  for (k=0; k <= self->max; k++)
    sum += self->p[k] * log(oscats_irt_model_P(self->model, k, theta,
                                               self->e->covariates));
  for (k=0; k < num; k++)
    L *= oscats_irt_model_P(g_ptr_array_index(items, k), resp[k], theta,
                            self->e->covariates);
  gsl_vector_memcpy(self->tmp, theta->v);
  gsl_vector_sub(self->tmp, self->mu);
  gsl_linalg_cholesky_solve(self->Sigma_half, self->tmp, self->tmp2);
  gsl_blas_ddot(self->tmp, self->tmp2, &g);
  return (sum - self->p_sum) * L * g;
}

// This value will be minimized
// Return values have been negated accordingly.
static gdouble criterion(const OscatsItem *item,
                         const OscatsExaminee *e,
                         gpointer data)
{
  OscatsAlgMaxKlIrt *alg_data = OSCATS_ALG_MAX_KL_IRT(data);
  gdouble I = 0;
  guint k;
  alg_data->model = item->irt_model;
  alg_data->max = oscats_irt_model_get_max(item->irt_model);
  for (k=0; k <= alg_data->max; k++)
  {
    gdouble p = oscats_irt_model_P(item->irt_model, k,
                                   e->theta_hat, e->covariates);
    alg_data->p[k] = p;
    I += p * log(p);
  }
  alg_data->p_sum = I;
  
  if (alg_data->posterior)
    return oscats_integrate_space(alg_data->integrator, alg_data);
  else if (alg_data->inf_bounds)
    return oscats_integrate_ellipse(alg_data->integrator,
                                    alg_data->e->theta_hat,
                                    alg_data->Inf_inv,
                                    alg_data->c,
                                    alg_data);
  else
    return oscats_integrate_cube(alg_data->integrator,
                                 alg_data->e->theta_hat,
                                 alg_data->c / 
                                 (alg_data->e->items->len > 0 ?
                                  sqrt(alg_data->e->items->len) : 1),
                                 alg_data);
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(alg_data);
  OscatsItem *item;

  if (self->Inf)
  {
    for (; self->base_num < e->items->len; self->base_num++)
    {
      item = g_ptr_array_index(e->items, self->base_num);
      oscats_irt_model_fisher_inf(item->irt_model, e->theta_hat,
                                  e->covariates, self->Inf);
    }
    g_gsl_matrix_copy(self->Inf_inv, self->Inf);
    gsl_linalg_cholesky_decomp(self->Inf_inv->v);
    gsl_linalg_cholesky_invert(self->Inf_inv->v);
  }

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
  OscatsAlgMaxKlIrt *self = OSCATS_ALG_MAX_KL_IRT(alg_data);
  guint dims = oscats_item_bank_num_dims(test->itembank);
  g_return_if_fail(oscats_item_bank_is_irt(test->itembank));

  if (self->posterior)
  {
    if (self->mu)
      g_return_if_fail(self->mu->size == dims);
    else
      self->mu = gsl_vector_calloc(dims);
    if (self->Sigma_half)
      g_return_if_fail(self->Sigma_half->size1 == dims &&
                       self->Sigma_half->size2 == dims);
    else
    {
      self->Sigma_half = gsl_matrix_alloc(dims, dims);
      gsl_matrix_set_identity(self->Sigma_half);
    }
    self->tmp = gsl_vector_alloc(dims);
    self->tmp2 = gsl_vector_alloc(dims);
    oscats_integrate_set_c_function(self->integrator, dims, integrand_posterior);
  } else {
    oscats_integrate_set_c_function(self->integrator, dims, integrand);
    if (self->inf_bounds)
    {
      self->Inf = g_gsl_matrix_new(dims, dims);
      self->Inf_inv = g_gsl_matrix_new(dims, dims);
    }
  }

  self->p = g_new(gdouble, oscats_item_bank_max_response(test->itembank)+1);

  self->chooser->bank = test->itembank;
  self->chooser->criterion = criterion;

  g_signal_connect_data(test, "initialize", G_CALLBACK(initialize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_object_ref(alg_data);
}
                   
