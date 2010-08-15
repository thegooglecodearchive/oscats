/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Estimate latent IRT ability
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

#include <math.h>
#include "gsl.h"
#include "algorithm.h"
#include "algorithms/estimate_theta.h"

#define MAX_MLE_ITERS 10

enum {
  PROP_0,
  PROP_MU,
  PROP_SIGMA,
};

G_DEFINE_TYPE(OscatsAlgEstimateTheta, oscats_alg_estimate_theta, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_estimate_theta_dispose (GObject *object);
static void oscats_alg_estimate_theta_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_estimate_theta_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_estimate_theta_class_init (OscatsAlgEstimateThetaClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->dispose = oscats_alg_estimate_theta_dispose;
  gobject_class->set_property = oscats_alg_estimate_theta_set_property;
  gobject_class->get_property = oscats_alg_estimate_theta_get_property;
  
  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgEstimateTheta:mu:
 *
 * Population mean for EAP.  Default: 0.
 */
  pspec = g_param_spec_object("mu", "Pop mean", 
                              "Population mean for EAP",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MU, pspec);

/**
 * OscatsAlgEstimateTheta:Sigma:
 *
 * Population covariance matrix for EAP.  Default: identity.
 */
  pspec = g_param_spec_object("Sigma", "Pop covariance", 
                              "Population covariance matrix for EAP",
                              G_TYPE_GSL_MATRIX,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIGMA, pspec);

}

static void oscats_alg_estimate_theta_init (OscatsAlgEstimateTheta *self)
{
}

static void oscats_alg_estimate_theta_dispose (GObject *object)
{
  OscatsAlgEstimateTheta *self = OSCATS_ALG_ESTIMATE_THETA(object);
  G_OBJECT_CLASS(oscats_alg_estimate_theta_parent_class)->dispose(object);
  if (self->mu) g_object_unref(self->mu);
  if (self->Sigma) g_object_unref(self->Sigma);
  self->mu = NULL;
  self->Sigma = NULL;
}

static void oscats_alg_estimate_theta_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgEstimateTheta *self = OSCATS_ALG_ESTIMATE_THETA(object);
  switch (prop_id)
  {
    case PROP_MU:			// construction only
      self->mu = g_value_dup_object(value);
      break;
    
    case PROP_SIGMA:			// construction only
      self->Sigma = g_value_dup_object(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_estimate_theta_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgEstimateTheta *self = OSCATS_ALG_ESTIMATE_THETA(object);
  switch (prop_id)
  {
    case PROP_MU:
      g_value_set_object(value, self->mu);
      break;
    
    case PROP_SIGMA:
      g_value_set_object(value, self->Sigma);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

// FIXME: This isn't quite right for multidimensional tests
static void administered (OscatsTest *test, OscatsExaminee *e,
                          OscatsItem *item, guint resp, gpointer alg_data)
{
  OscatsAlgEstimateTheta *self = OSCATS_ALG_ESTIMATE_THETA(alg_data);
  if (e->items->len == 1)
  {
    if (resp == 0) self->flag = -1;
    else if (resp == oscats_irt_model_get_max(item->irt_model))
      self->flag = 1;
    else self->flag = 0;
  } else if (self->flag &&
      (resp != (self->flag == 1 ? oscats_irt_model_get_max(item->irt_model)
                                 : 0) ))
      self->flag = 0;
  if (self->flag)
    oscats_estimate_theta_eap(e->items, e->resp, e->covariates,
                              e->theta_hat, e->theta_err,
                              self->mu, self->Sigma);
  else
    if (oscats_estimate_theta_mle(e->items, e->resp, e->covariates,
                                  e->theta_hat, e->theta_err))
      oscats_estimate_theta_eap(e->items, e->resp, e->covariates,
                                e->theta_hat, e->theta_err,
                                self->mu, self->Sigma);
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
  OscatsAlgEstimateTheta *self = OSCATS_ALG_ESTIMATE_THETA(alg_data);
  guint dims = oscats_item_bank_num_dims(test->itembank);
  g_return_if_fail(oscats_item_bank_is_irt(test->itembank));
  if (self->mu)
    g_return_if_fail(self->mu->v->size == dims);
  else
    self->mu = g_gsl_vector_new(dims);
  if (self->Sigma)
    g_return_if_fail(self->Sigma->v->size1 == dims &&
                     self->Sigma->v->size2 == dims);
  else
  {
    guint i;
    self->Sigma = g_gsl_matrix_new(dims, dims);
    for (i=0; i < dims; i++)
      g_gsl_matrix_set(self->Sigma, i, i, 1);
  }

  g_signal_connect_data(test, "administered", G_CALLBACK(administered),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}

static gdouble TOL = 1e-6;

/**
 * oscats_estimate_theta_mle:
 * @items: an array of items
 * @resp: an array of responses
 * @covariates: a set of #OscatsCovariates (or %NULL)
 * @theta: starting value and return for estimate
 * @err: return matrix for covariance matrix of estimate (or %NULL)
 *
 * Calculates the Maximum Likelihood Estimate for the latent IRT ability
 * from the given items and responses via the Newton-Raphson algorithm.
 * Note that the MLE does not always exist (notably for response patterns
 * that are all perfect or worst scores).  In these cases, use
 * oscats_estimate_theta_eap().
 * Must have @items->len == @resp->len > 0.
 * All items must have IRT models and the same test dimension.
 * Note: @err is not set if the Newton-Raphson algorithm does not converge.
 *
 * Returns: %TRUE if failed to converge, %FALSE on success
 */
gboolean oscats_estimate_theta_mle(const GPtrArray *items, const GByteArray *resp,
                                   const OscatsCovariates *covariates,
                                   GGslVector *theta, GGslMatrix *err)
{
  OscatsItem *item;
  GGslVector *grad, *delta;
  GGslMatrix *hes;
  GGslPermutation *perm;
  guint dim, num, i, iters = 0;
  gdouble diff, x;
  gboolean fail = FALSE;
  g_return_val_if_fail(items && resp &&
                       G_GSL_IS_VECTOR(theta) && theta->v, TRUE);
  g_return_val_if_fail(items->len == resp->len && items->len > 0, TRUE);
  item = g_ptr_array_index(items, 0);
  g_return_val_if_fail(item->irt_model && 
                       theta->v->size == item->irt_model->testDim, TRUE);
  if (err)
    g_return_val_if_fail(G_GSL_IS_MATRIX(err) && err->v &&
                         err->v->size1 == err->v->size2 &&
                         err->v->size1 == theta->v->size, TRUE);

  num = items->len;
  dim = theta->v->size;
  grad = g_gsl_vector_new(dim);
  delta = g_gsl_vector_new(dim);
  hes = g_gsl_matrix_new(dim, dim);
  perm = g_gsl_permutation_new(dim);
  
  do
  {
    g_gsl_vector_set_all(grad, 0);
    g_gsl_matrix_set_all(hes, 0);
    for (i=0; i < num; i++)
    {
      item = g_ptr_array_index(items, i);
      oscats_irt_model_logLik_dtheta(item->irt_model, resp->data[i],
                                     theta, covariates, grad, hes);
    }
    // delta = hes^(-1) * grad
    g_gsl_matrix_solve(hes, grad, delta, perm);
    // theta <- theta - delta
    diff = 0;
    for (i=0; i < dim; i++)
    {
      if (++iters == MAX_MLE_ITERS) { fail = TRUE; break; }
      x = gsl_vector_get(delta->v, i);
      if (fabs(x) > diff) diff = fabs(x);
      x = gsl_vector_get(theta->v, i) - x;
      gsl_vector_set(theta->v, i, x);
      if (!isfinite(x)) fail = TRUE;
    }
  } while (diff > TOL && !fail);
  if (err && !fail)
  {
    // FIXME: This is wrong.
    gsl_matrix_scale(hes->v, -1);		// Fisher Information
    g_gsl_matrix_invert(hes, err, perm);
  }

  // FIXME: Improve performance by putting persistent instances in alg_data
  g_object_unref(grad);
  g_object_unref(delta);
  g_object_unref(hes);
  g_object_unref(perm);
  return fail;
}

static gdouble EPS = 1e-14;

/**
 * oscats_estimate_theta_eap:
 * @items: an array of items
 * @resp: an array of responses
 * @covariates: a set of #OscatsCovariates (or %NULL)
 * @theta: return for estimate
 * @err: return for covariance matrix of estimate (or %NULL)
 * @mu: population mean
 * @Sigma: population covariance matrix
 *
 * Calculates the Expected A Posteriori estimate for the latent IRT ability
 * from the given items and responses, given a (multivariate) normal prior.
 * Must have @items->len == @resp->len > 0.
 * All items must have IRT models and the same test dimension.
 */
void oscats_estimate_theta_eap(const GPtrArray *items, const GByteArray *resp,
                               const OscatsCovariates *covariates,
                               GGslVector *theta, GGslMatrix *err,
                               const GGslVector *mu, const GGslMatrix *Sigma)
{
  static const guint N_X = 42;
  static const gdouble X[] = { 8.42258928701857, 7.96774992108814, 7.5212544151757, 7.08207039338074, 6.64937761183707, 6.22242653735663, 5.80058077383636, 5.38330292012255, 4.97008385933276, 4.56045690099139, 4.15404020743661, 3.75045194100658, 3.34935269044632, 2.95043132877212, 2.55339087113587, 2.15793433268948, 1.76380715499172, 1.37075336538781, 0.978525476504317, 0.586883072035648, 0.195588564103324, -0.195588564103324, -0.586883072035648, -0.978525476504317, -1.37075336538781, -1.76380715499172, -2.15793433268948, -2.55339087113587, -2.95043132877212, -3.34935269044632, -3.75045194100658, -4.15404020743661, -4.56045690099139, -4.97008385933276, -5.38330292012255, -5.80058077383636, -6.22242653735663, -6.64937761183707, -7.08207039338074, -7.5212544151757, -7.96774992108814, -8.42258928701857 };
  static const gdouble A[] = { 7.22213444003647e-17, 2.94429104449399e-15, 9.18692466474154e-14, 2.23372811539061e-12, 4.29642780042454e-11, 6.62144179043318e-10, 8.26611084543324e-09, 8.43762447883176e-08, 7.09942320561484e-07, 4.95838015500946e-06, 2.89199631199495e-05, 0.00014160255843715, 0.000584686591118046, 0.00204382754346427, 0.00606842316063967, 0.0153477056602080, 0.0331404961375952, 0.0612134414357645, 0.096863452840980, 0.131453352018710, 0.153108076614439, 0.153108076614439, 0.131453352018710, 0.096863452840980, 0.0612134414357645, 0.0331404961375952, 0.0153477056602080, 0.00606842316063967, 0.00204382754346427, 0.000584686591118046, 0.00014160255843715, 2.89199631199495e-05, 4.95838015500946e-06, 7.09942320561484e-07, 8.43762447883176e-08, 8.26611084543324e-09, 6.62144179043318e-10, 4.29642780042454e-11, 2.23372811539061e-12, 9.18692466474154e-14, 2.94429104449399e-15, 7.22213444003647e-17 };
  OscatsItem *item;
  GGslVector *theta_temp;

  g_return_if_fail(items && resp &&
                   G_GSL_IS_VECTOR(theta) && theta->v &&
                   G_GSL_IS_VECTOR(mu) && mu->v &&
                   G_GSL_IS_MATRIX(Sigma) && Sigma->v);
  g_return_if_fail(items->len == resp->len && items->len > 0);
  item = g_ptr_array_index(items, 0);
  g_return_if_fail(item->irt_model &&
                   theta->v->size == item->irt_model->testDim);
  if (err)
    g_return_if_fail(G_GSL_IS_MATRIX(err) && err->v &&
                     err->v->size1 == err->v->size2 &&
                     err->v->size1 == theta->v->size);

  // FIXME: Expand to multiple dimensions
  g_return_if_fail(theta->v->size == 1);

  theta_temp = g_gsl_vector_new(theta->v->size);

  if (theta->v->size == 1)
  {
    gdouble posterior[N_X];
    gdouble mean = mu->v->data[0];
    gdouble sd = Sigma->v->data[0];
    gdouble *Z = &(theta_temp->v->data[0]);
    gdouble sum = 0, eap = 0, sqrs = 0;
    guint z, i;
    
    for (z=0; z < N_X; z++)
    {
      posterior[z] = 0;
      if (A[z] < EPS) continue;
      *Z = X[z]*sd + mean;		// sets theta_temp[0]
      for (i=0; i < items->len; i++)
      {
        item = g_ptr_array_index(items, i);
        posterior[z] += log(oscats_irt_model_P(item->irt_model, resp->data[i],
                                               theta_temp, covariates));
      }
      sum += (posterior[z] = exp(posterior[z])*A[z]);
    }
    for (z=0; z < N_X; z++)
    {
      gdouble x, y;
      x = (X[z]*sd + mean);
      y = x * posterior[z]/sum;
      eap += y;
      sqrs += x*y;
    }

    theta->v->data[0] = eap;
    if (err) err->v->data[0] = sqrs - eap;
  }

  g_object_unref(theta_temp);
}

/**
 * oscats_estimate_theta_mle_set_tol:
 * @tol: tolerance level
 *
 * Sets the stopping criterion for the Newton-Raphson algorithm
 * in oscats_estimate_theta_mle().  Must have @tol > 0.  Default: 1e-6.
 */
void oscats_estimate_theta_mle_set_tol(gdouble tol)
{
  g_return_if_fail(tol > 0);
  TOL = tol;
}

/**
 * oscats_estimate_theta_eap_set_eps:
 * @eps: tolerance level
 *
 * Sets the weight threshold for computing the posterior distributuion
 * in oscats_estimate_theta_eap().  Must have @eps > 0.  Default: 1e-14.
 */
void oscats_estimate_theta_eap_set_eps(gdouble eps)
{
  g_return_if_fail(eps > 0);
  EPS = eps;
}

