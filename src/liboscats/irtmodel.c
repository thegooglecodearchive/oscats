/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Abstract IRT Model Class
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
 * SECTION:irtmodel
 * @title:OscatsIrtModel
 * @short_description: Abstract IRT Model Class
 */

#include "irtmodel.h"

G_DEFINE_TYPE(OscatsIrtModel, oscats_irt_model, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_TEST_DIM,
  PROP_N_DIMS,
  PROP_DIMS,
  PROP_N_PARAMS,
  PROP_N_COV,
};

static void oscats_irt_model_dispose (GObject *object);
static void oscats_irt_model_finalize (GObject *object);
static void oscats_irt_model_constructed (GObject *object);
static void oscats_irt_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_irt_model_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
static guint8 null_get_max (const OscatsIrtModel *model)
{
  g_critical("Abstract IRT Model should be overloaded.");
  return 0;
}
static gdouble null_P (const OscatsIrtModel *model, guint resp,
                       const GGslVector *theta, const OscatsCovariates *covariates)
{
  g_critical("Abstract IRT Model should be overloaded.");
  return 0;
}
static gdouble null_distance (const OscatsIrtModel *model,
                              const GGslVector *theta, const OscatsCovariates *covariates)
{
  g_critical("Abstract IRT Model should be overloaded.");
  return 0;
}
static void null_logLik_theta (const OscatsIrtModel *model, guint resp,
                         const GGslVector *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes, gboolean Inf)
{
  g_critical("Abstract IRT Model should be overloaded.");
}
static void null_logLik_param (const OscatsIrtModel *model, guint resp,
                         const GGslVector *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes)
{
  g_critical("Abstract IRT Model should be overloaded.");
}
                   
static void oscats_irt_model_class_init (OscatsIrtModelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->constructed = oscats_irt_model_constructed;
  gobject_class->dispose = oscats_irt_model_dispose;
  gobject_class->finalize = oscats_irt_model_finalize;
  gobject_class->set_property = oscats_irt_model_set_property;
  gobject_class->get_property = oscats_irt_model_get_property;
  
  klass->get_max = null_get_max;
  klass->P = null_P;
  klass->distance = null_distance;
  klass->logLik_dtheta = null_logLik_theta;
  klass->logLik_dparam = null_logLik_param;
  
/**
 * OscatsIrtModel:Np:
 *
 * The number of parameters of the IRT model.
 */
  pspec = g_param_spec_uint("Np", "Num Params", 
                            "Number of parameters of the IRT model",
                            0, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_PARAMS, pspec);

/**
 * OscatsIrtModel:testDim:
 *
 * The IRT dimension of the test.
 * This is derived from #OscatsIrtModel:dims at construction.
 */
  pspec = g_param_spec_uint("testDim", "Test Dimension", 
                            "Total IRT Dimension for the test",
                            1, G_MAXUINT, 1,
                            G_PARAM_READABLE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_TEST_DIM, pspec);

/**
 * OscatsIrtModel:Ndims:
 *
 * The dimension of the IRT model (<= the dimension of the test).
 * Determined from #OscatsIrtModel:dims.  Default is 1.
 */
  pspec = g_param_spec_uint("Ndims", "Num Dims", 
                            "Dimension of the IRT model",
                            1, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_DIMS, pspec);

/**
 * OscatsIrtModel:dims:
 *
 * A #GBitArray indicating onto which dimensions of the test 
 * this item loads.  The default is a test with one dimension.
 * Note, the object's reference count is increased on set.
 */
  pspec = g_param_spec_object("dims", "Dimensions", 
                            "Dimension this item loads onto",
                            G_TYPE_BIT_ARRAY,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DIMS, pspec);

/**
 * OscatsIrtModel:Ncov:
 *
 * The number of covariates in the IRT model.  Default is 0.
 * The default names are Cov.1, Cov.2, etc.  It is strongly recommended
 * to change the default names with oscats_irt_model_set_covariate_name()
 * or oscats_irt_model_set_covariate_name_str().
 */
  pspec = g_param_spec_uint("Ncov", "Number of Covariates", 
                            "Number of covariates in the IRT model",
                            0, G_MAXUINT, 0,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_COV, pspec);

}

static void oscats_irt_model_init (OscatsIrtModel *self)
{
}

static void oscats_irt_model_constructed(GObject *object)
{
  OscatsIrtModel *model = OSCATS_IRT_MODEL(object);
//  G_OBJECT_CLASS(oscats_irt_model_parent_class)->constructed(object);

  if (!model->dimsFlags)
  {
    model->dimsFlags = g_bit_array_new(1);
    g_bit_array_set_bit(model->dimsFlags, 0);
  }
  model->testDim = g_bit_array_get_len(model->dimsFlags);
  model->Ndims = 0;
  for (g_bit_array_iter_reset(model->dimsFlags);
       g_bit_array_iter_next(model->dimsFlags) >= 0;
       model->Ndims++) ;
  if (model->Ndims == 0)
    g_critical("Item must load onto at least one dimension.");

  g_bit_array_iter_reset(model->dimsFlags);
  if (model->Ndims < 3)
  {
    model->dim1 = g_bit_array_iter_next(model->dimsFlags);
    model->dim2 = g_bit_array_iter_next(model->dimsFlags);
      // if Ndims == 1, dim2 will be -1,
      // but that's okay, since we'll never access it
  } else {
    guint i;
    model->dims = g_new(guint, model->Ndims);
    for (i=0; i < model->Ndims; i++)
      model->dims[i] = g_bit_array_iter_next(model->dimsFlags);
  }

  // Note: Np, params, and names should be set in the overloaded constructed().
  // Chain up first to assign Ndims before setting Np and params.

}

static void oscats_irt_model_dispose (GObject *object)
{
  OscatsIrtModel *self = OSCATS_IRT_MODEL(object);
  G_OBJECT_CLASS(oscats_irt_model_parent_class)->dispose(object);
  if (self->dimsFlags) g_object_unref(self->dimsFlags);
  self->dimsFlags = NULL;
}

static void oscats_irt_model_finalize (GObject *object)
{
  OscatsIrtModel *self = OSCATS_IRT_MODEL(object);
  g_free(self->params);
  g_free(self->names);
  g_free(self->dims);
  G_OBJECT_CLASS(oscats_irt_model_parent_class)->finalize(object);
}

static void oscats_irt_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsIrtModel *self = OSCATS_IRT_MODEL(object);
  switch (prop_id)
  {
    case PROP_DIMS:			// construction only
      self->dimsFlags = g_value_dup_object(value);
      break;
    
    case PROP_N_COV:			// construction only
      self->Ncov = g_value_get_uint(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_irt_model_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec)
{
  OscatsIrtModel *self = OSCATS_IRT_MODEL(object);
  switch (prop_id)
  {
    case PROP_N_PARAMS:
      g_value_set_uint(value, self->Np);
      break;
    
    case PROP_N_DIMS:
      g_value_set_uint(value, self->Ndims);
      break;
    
    case PROP_TEST_DIM:
      g_value_set_uint(value, self->testDim);
      break;
    
    case PROP_DIMS:
      g_value_set_object(value, self->dims);
      break;
    
    case PROP_N_COV:
      g_value_set_uint(value, self->Ncov);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_irt_model_get_max:
 * @model: an #OscatsIrtModel
 *
 * Returns: the maximum valid response category for this model
 */
guint8 oscats_irt_model_get_max(const OscatsIrtModel *model)
{
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model), 0);
  return OSCATS_IRT_MODEL_GET_CLASS(model)->get_max(model);
}

/**
 * oscats_irt_model_P:
 * @model: an #OscatsIrtModel
 * @resp: the examinee response value
 * @theta: the #GGslVector parameter value
 * @covariates: the values for covariates
 * 
 * Calculates the probability of response @resp, given latent ability @theta.
 *
 * Returns: the computed probability
 */
gdouble oscats_irt_model_P(const OscatsIrtModel *model, guint resp,
                           const GGslVector *theta, const OscatsCovariates *covariates)
{
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model) && G_GSL_IS_VECTOR(theta) && theta->v, 0);
  g_return_val_if_fail(theta->v->size == model->testDim, 0);
  return OSCATS_IRT_MODEL_GET_CLASS(model)->P(model, resp, theta, covariates);
}

/**
 * oscats_irt_model_distance:
 * @model: an #OscatsIrtModel
 * @theta: the #GGslVector parameter value
 * @covariates: the values of covariates
 *
 * Calculates a distance metric between the model and the given latent
 * IRT ability.  Metric definition is implementation specific.
 *
 * Returns: the distance metric
 */
gdouble oscats_irt_model_distance(const OscatsIrtModel *model,
                                  const GGslVector *theta,
                                  const OscatsCovariates *covariates)
{
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model) && G_GSL_IS_VECTOR(theta), 0);
  g_return_val_if_fail(theta->v->size == model->testDim, 0);
  return OSCATS_IRT_MODEL_GET_CLASS(model)->distance(model, theta, covariates);
}

/**
 * oscats_irt_model_logLik_dtheta:
 * @model: an #OscatsIrtModel
 * @resp: the examinee response value 
 * @theta: the #GGslVector parameter value
 * @covariates: the value of covariates
 * @grad: a #GGslVector for returning the gradient (or %NULL)
 * @hes: a #GGslMatrix for returning the Hessian (or %NULL)
 * 
 * Calculates the derivative of the log-likelihood with respect to the 
 * latent ability @theta, given the examinee response @resp,
 * evaluated at @theta.  If given, the graident is <emphasis>added</emphasis>
 * to @grad, and the Hessian (second derivatives) to @hes.  The vectors and
 * matrix must have dimension @model->dims->size.
 */
void oscats_irt_model_logLik_dtheta(const OscatsIrtModel *model,
                                    guint resp, const GGslVector *theta,
                                    const OscatsCovariates *covariates,
                                    GGslVector *grad, GGslMatrix *hes)
{
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model) && G_GSL_IS_VECTOR(theta));
  g_return_if_fail(theta->v->size == model->testDim);
  if (grad) g_return_if_fail(G_GSL_IS_VECTOR(grad) && grad->v && 
                             grad->v->size == model->testDim);
  if (hes) g_return_if_fail(G_GSL_IS_MATRIX(hes) && hes->v &&
                            hes->v->size1 == model->testDim && 
                            hes->v->size2 == model->testDim);
  OSCATS_IRT_MODEL_GET_CLASS(model)->logLik_dtheta(model, resp, theta,
                                              covariates, grad, hes, FALSE);
}

/**
 * oscats_irt_model_logLik_dparam:
 * @model: an #OscatsIrtModel
 * @resp: the examinee response value 
 * @theta: the #GGslVector parameter value
 * @covariates: the values of covariates
 * @grad: a #GGslVector for returning the gradient (or %NULL)
 * @hes: a #GGslMatrix for returning the Hessian (or %NULL)
 * 
 * Calculates the derivative of the log-likelihood with respect to the
 * parameters, given the examinee response @resp, evaluated at
 * @theta.  If given, the graident is <emphasis>added</emphasis> to @grad,
 * and the Hessian (second derivatives) to @hes.  The gradient and Hessian
 * must have dimension @model->Np, and the latent ability @theta must have
 * dimension @model->dims->size.
 */
void oscats_irt_model_logLik_dparam(const OscatsIrtModel *model,
                                    guint resp, const GGslVector *theta,
                                    const OscatsCovariates *covariates,
                                    GGslVector *grad, GGslMatrix *hes)
{
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model) && G_GSL_IS_VECTOR(theta));
  g_return_if_fail(theta->v->size == model->testDim);
  if (grad) g_return_if_fail(G_GSL_IS_VECTOR(grad) && grad->v &&
                             grad->v->size == model->Np);
  if (hes) g_return_if_fail(G_GSL_IS_MATRIX(hes) && hes->v &&
                            hes->v->size1 == model->Np && 
                            hes->v->size2 == model->Np);
  OSCATS_IRT_MODEL_GET_CLASS(model)->logLik_dparam(model, resp, theta,
                                              covariates, grad, hes);
}

/**
 * oscats_irt_model_fisher_inf:
 * @model: an #OscatsIrtModel
 * @theta: the #GGslVector parameter value
 * @covariates: the value of covariates
 * @I: a #GGslMatrix for returning the Fisher Information
 * 
 * Calculates the Fisher Information at @theta, given @covariates:
 * I = E_{X|theta}[-d^2/dtheta dtheta' log(P(X))].
 * The informatoin is <emphasis>added</emphasis> to @I.
 * The vector and matrix must have dimension @model->dims->size.
 */
void oscats_irt_model_fisher_inf(const OscatsIrtModel *model,
                                 const GGslVector *theta,
                                 const OscatsCovariates *covariates,
                                 GGslMatrix *I)
{
  OscatsIrtModelClass *klass;
  guint k, max;
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model) && G_GSL_IS_VECTOR(theta));
  g_return_if_fail(theta->v->size == model->testDim);
  g_return_if_fail(G_GSL_IS_MATRIX(I) && I->v &&
                            I->v->size1 == model->testDim && 
                            I->v->size2 == model->testDim);
  klass = OSCATS_IRT_MODEL_GET_CLASS(model);
  max = klass->get_max(model);
  for (k=0; k <= max; k++)
    klass->logLik_dtheta(model, k, theta, covariates, NULL, I, TRUE);
}

/**
 * oscats_irt_model_get_param_name:
 * @model: an #OscatsIrtModel
 * @index: the parameter index
 * 
 * Returns: the name of parameter @index < #OscatsIrtModel:Np
 */
const gchar* oscats_irt_model_get_param_name(const OscatsIrtModel *model,
                                             guint index)
{
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model) && index < model->Np, NULL);
  return g_quark_to_string(model->names[index]);
}

/**
 * oscats_irt_model_has_param_name:
 * @model: an #OscatsIrtModel
 * @name: parameter name to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_irt_model_has_param_name(const OscatsIrtModel *model, 
                                         const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  if (!q) return FALSE;
  return oscats_irt_model_has_param(model, q);
}

/**
 * oscats_irt_model_has_param:
 * @model: an #OscatsIrtModel
 * @name: parameter name (as a #GQuark) to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_irt_model_has_param(const OscatsIrtModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model), FALSE);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return TRUE;
  return FALSE;
}

/**
 * oscats_irt_model_get_param_by_index:
 * @model: an #OscatsIrtModel
 * @index: the parameter index
 * 
 * Returns: the parameter @index < #OscatsIrtModel:Np
 */
gdouble oscats_irt_model_get_param_by_index(const OscatsIrtModel *model, guint index)
{
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model) && index < model->Np, 0);
  return model->params[index];
}

/**
 * oscats_irt_model_get_param:
 * @model: an #OscatsIrtModel
 * @name: the parameter name (as a #GQuark)
 * 
 * Returns: the parameter @name
 */
gdouble oscats_irt_model_get_param(const OscatsIrtModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model), 0);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return model->params[i];
  g_critical("Unknown parameter %s", g_quark_to_string(name));
  return 0;
}

/**
 * oscats_irt_model_get_param_by_name:
 * @model: an #OscatsIrtModel
 * @name: the parameter name
 * 
 * The #GQuark version oscats_irt_model_get_param() is faster.
 *
 * Returns: the parameter called @name
 */
gdouble oscats_irt_model_get_param_by_name(const OscatsIrtModel *model,
                                           const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  g_return_val_if_fail(q != 0, 0);
  return oscats_irt_model_get_param(model, q);
}

/**
 * oscats_irt_model_set_param_by_index:
 * @model: an #OscatsIrtModel
 * @index: the parameter index < #OscatsIrtModel:Np
 * @value: the #gdouble value to set
 */
void oscats_irt_model_set_param_by_index(OscatsIrtModel *model, guint index,
                                         gdouble value)
{
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model) && index < model->Np);
  model->params[index] = value;
}

/**
 * oscats_irt_model_set_param:
 * @model: an #OscatsIrtModel
 * @name: the parameter name (as a #GQuark)
 * @value: the #gdouble to set
 */
void oscats_irt_model_set_param(OscatsIrtModel *model,
                                GQuark name, gdouble value)
{
  guint i;
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model));
  for (i=0; i < model->Np; i++)
    if (model->names[i] == name)
    {
      model->params[i] = value;
      return;
    }
  g_critical("Unknown parameter %s", g_quark_to_string(name));
}

/**
 * oscats_irt_model_set_param_by_name:
 * @model: an #OscatsIrtModel
 * @name: the parameter name
 * @value: the #gdouble to set
 *
 * The #GQuark version oscats_irt_model_set_param() is faster.
 */
void oscats_irt_model_set_param_by_name(OscatsIrtModel *model,
                                        const gchar *name, gdouble value)
{
  GQuark q = g_quark_try_string(name);
  g_return_if_fail(q != 0);
  oscats_irt_model_set_param(model, q, value);
}

/**
 * oscats_irt_model_get_covariate_name:
 * @model: an #OscatsIrtModel
 * @index: the covariate index
 *
 * Returns: the name of the covariate
 */
const gchar * oscats_irt_model_get_covariate_name(const OscatsIrtModel *model,
                                                  guint index)
{
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model) &&
                       index < model->Ncov, NULL);
  return g_quark_to_string(model->covariates[index]);
}

/**
 * oscats_irt_model_has_covariate:
 * @model: an #OscatsIrtModel
 * @name: covariate name (as a #GQuark)
 *
 * Returns: %TRUE if @model incorporates covariate @name
 */
gboolean oscats_irt_model_has_covariate(const OscatsIrtModel *model,
                                        GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_IRT_MODEL(model), FALSE);
  for (i=0; i < model->Ncov; i++)
    if (model->covariates[i] == name) return TRUE;
  return FALSE;
}

/**
 * oscats_irt_model_has_covariate_name:
 * @model: an #OscatsIrtModel
 * @name: covariate name
 *
 * Returns: %TRUE if @model incorporates covariate @name
 */
gboolean oscats_irt_model_has_covariate_name(const OscatsIrtModel *model,
                                             const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  if (!q) return FALSE;
  return oscats_irt_model_has_covariate(model, q);
}

/**
 * oscats_irt_model_set_covariate_name:
 * @model: an #OscatsIrtModel
 * @index: covariate number in this model
 * @name: the covariate name (as a #GQuark)
 *
 * Sets the name of covariate @index to @name.
 * Must have @index < @model:Ncov.
 */
void oscats_irt_model_set_covariate_name(const OscatsIrtModel *model,
                                         guint index, GQuark name)
{
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model) && index < model->Ncov
                   && name != 0);
  model->covariates[index] = name;
}

/**
 * oscats_irt_model_set_covariate_name_str:
 * @model: an #OscatsIrtModel
 * @index: covariate number in this model
 * @name: the covariate name
 *
 * Sets the name of the covariate @index to @name.
 * Must have @index < @model:Ncov.
 */
void oscats_irt_model_set_covariate_name_str(const OscatsIrtModel *model,
                                             guint index, const gchar *name)
{
  g_return_if_fail(OSCATS_IS_IRT_MODEL(model) && index < model->Ncov
                   && name != NULL);
  model->covariates[index] = g_quark_from_string(name);
}
