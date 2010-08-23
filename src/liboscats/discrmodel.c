/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Classification Model Class
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
 * SECTION:discrmodel
 * @title:OscatsDiscrModel
 * @short_description: Abstract IRT Model Class
 */

#include "discrmodel.h"

G_DEFINE_TYPE(OscatsDiscrModel, oscats_discr_model, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_TEST_DIM,
  PROP_N_DIMS,
  PROP_DIMS,
  PROP_N_PARAMS,
};

static void oscats_discr_model_dispose (GObject *object);
static void oscats_discr_model_finalize (GObject *object);
static void oscats_discr_model_constructed (GObject *object);
static void oscats_discr_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_discr_model_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
static guint8 null_get_max (const OscatsDiscrModel *model)
{
  g_critical("Abstract Classification Model should be overloaded.");
  return 0;
}
static gdouble null_P (const OscatsDiscrModel *model, guint resp,
                       const OscatsAttributes *attr)
{
  g_critical("Abstract Classification Model should be overloaded.");
  return 0;
}
static void null_logLik (const OscatsDiscrModel *model, guint resp,
                         const OscatsAttributes *attr,
                         GGslVector *grad, GGslMatrix *hes)
{
  g_critical("Abstract Classification Model should be overloaded.");
}
                   
static void oscats_discr_model_class_init (OscatsDiscrModelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->constructed = oscats_discr_model_constructed;
  gobject_class->dispose = oscats_discr_model_dispose;
  gobject_class->finalize = oscats_discr_model_finalize;
  gobject_class->set_property = oscats_discr_model_set_property;
  gobject_class->get_property = oscats_discr_model_get_property;
  
  klass->get_max = null_get_max;
  klass->P = null_P;
  klass->logLik_dparam = null_logLik;
  
/**
 * OscatsDiscrModel:Np:
 *
 * The number of parameters of the IRT model.
 */
  pspec = g_param_spec_uint("Np", "Num Params", 
                            "Number of parameters of the Classification model",
                            0, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_PARAMS, pspec);

/**
 * OscatsDiscrModel:testDim:
 *
 * The number of attributes for the test.
 * This is derived from #OscatsDiscrModel:dims at construction.
 */
  pspec = g_param_spec_uint("testDim", "Test Dimension", 
                            "Total IRT Dimension for the test",
                            1, OSCATS_MAX_ATTRIBUTES, 1,
                            G_PARAM_READABLE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_TEST_DIM, pspec);

/**
 * OscatsDiscrModel:Ndims:
 *
 * The dimension of the Classification model (<= the dimension of the test).
 * Determiend from #OscatsDiscrModel:dims.  Default is 1.
 */
  pspec = g_param_spec_uint("Ndims", "Num Dims", 
                            "Dimension of the Classification model",
                            1, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_DIMS, pspec);

/**
 * OscatsDiscrModel:dims:
 *
 * An #OscatsAttributes indicating which attributes pertain to this item
 * (a row in the Q matrix).
 */
  pspec = g_param_spec_object("dims", "Attributes", 
                            "Attributes pertaining to this item",
                            OSCATS_TYPE_ATTRIBUTES,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DIMS, pspec);

}

static void oscats_discr_model_init (OscatsDiscrModel *self)
{
}

static void oscats_discr_model_constructed(GObject *object)
{
  int i, j=0;
  OscatsDiscrModel *model = OSCATS_DISCR_MODEL(object);
//  G_OBJECT_CLASS(oscats_discr_model_parent_class)->constructed(object);

  if (!model->dimsFlags)
    model->dimsFlags = g_object_new(OSCATS_TYPE_ATTRIBUTES, NULL);
  model->Ndims = 0;
  for (i=0; i < model->dimsFlags->num; i++)
    if (oscats_attributes_get(model->dimsFlags, i)) model->Ndims++;
  if (model->Ndims == 0)
    g_critical("Item must have at least one attribute.");
  model->dims = g_new(guint8, model->Ndims);
  for (i=0; i < model->dimsFlags->num; i++)
    if (oscats_attributes_get(model->dimsFlags, i))
      model->dims[j++] = i;

  // Note: Np, params, and names should be set in the overloaded constructed().
  // Chain up first to assign Ndims before setting Np and params.

}

static void oscats_discr_model_dispose (GObject *object)
{
  OscatsDiscrModel *self = OSCATS_DISCR_MODEL(object);
  G_OBJECT_CLASS(oscats_discr_model_parent_class)->dispose(object);
  if (self->dimsFlags) g_object_unref(self->dimsFlags);
  self->dimsFlags = NULL;
}

static void oscats_discr_model_finalize (GObject *object)
{
  OscatsDiscrModel *self = OSCATS_DISCR_MODEL(object);
  g_free(self->params);
  g_free(self->names);
  g_free(self->dims);
  G_OBJECT_CLASS(oscats_discr_model_parent_class)->finalize(object);
}

static void oscats_discr_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsDiscrModel *self = OSCATS_DISCR_MODEL(object);
  switch (prop_id)
  {
    case PROP_DIMS:			// construction only
      self->dimsFlags = g_value_dup_object(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_discr_model_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec)
{
  OscatsDiscrModel *self = OSCATS_DISCR_MODEL(object);
  switch (prop_id)
  {
    case PROP_N_PARAMS:
      g_value_set_uint(value, self->Np);
      break;
    
    case PROP_N_DIMS:
      g_value_set_uint(value, self->Ndims);
      break;
    
    case PROP_TEST_DIM:
      g_value_set_uint(value, self->dimsFlags->num);
      break;
    
    case PROP_DIMS:
      g_value_set_object(value, self->dims);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_discr_model_get_max:
 * @model: an #OscatsDiscrModel
 *
 * Returns: the maximum valid response category for this model
 */
guint8 oscats_discr_model_get_max(const OscatsDiscrModel *model)
{
  g_return_val_if_fail(OSCATS_IS_DISCR_MODEL(model), 0);
  return OSCATS_DISCR_MODEL_GET_CLASS(model)->get_max(model);
}

/**
 * oscats_discr_model_P:
 * @model: an #OscatsDiscrModel
 * @resp: the examinee response value
 * @attr: the #OscatsAttributes value
 * 
 * Calculates the probability of response @resp, given latent class @attr.
 *
 * Returns: the computed probability
 */
gdouble oscats_discr_model_P(const OscatsDiscrModel *model, guint resp,
                             const OscatsAttributes *attr)
{
  g_return_val_if_fail(OSCATS_IS_DISCR_MODEL(model) &&
                       OSCATS_IS_ATTRIBUTES(attr), 0);
  g_return_val_if_fail(model->dimsFlags->num == attr->num, 0);
  return OSCATS_DISCR_MODEL_GET_CLASS(model)->P(model, resp, attr);
}

/**
 * oscats_discr_model_logLik_dparam:
 * @model: an #OscatsDiscrModel
 * @resp: the examinee response value 
 * @attr: the #OscatsAttributes parameter value
 * @grad: a #GGslVector for returning the gradient (or %NULL)
 * @hes: a #GGslMatrix for returning the Hessian (or %NULL)
 * 
 * Calculates the derivative of the log-likelihood with respect to the
 * parameters, given the examinee response @resp, evaluated at latent class
 * @attr.  If given, the graident is <emphasis>added</emphasis> to @grad,
 * and the Hessian (second derivatives) to @hes.  The gradient and Hessian
 * must have dimension @model->Np, and the latent class @attr must have
 * dimension @model->dimsFlags->num.
 */
void oscats_discr_model_logLik_dparam(const OscatsDiscrModel *model,
                                    guint resp, const OscatsAttributes *attr,
                                    GGslVector *grad, GGslMatrix *hes)
{
  g_return_if_fail(OSCATS_IS_DISCR_MODEL(model) && OSCATS_IS_ATTRIBUTES(attr));
  g_return_if_fail(model->dimsFlags->num == attr->num);
  if (grad) g_return_if_fail(G_GSL_IS_VECTOR(grad) && grad->v &&
                             grad->v->size == model->Np);
  if (hes) g_return_if_fail(G_GSL_IS_MATRIX(hes) && hes->v &&
                            hes->v->size1 == model->Np && 
                            hes->v->size2 == model->Np);
  OSCATS_DISCR_MODEL_GET_CLASS(model)->logLik_dparam(model, resp, attr,
                                                     grad, hes);
}

/**
 * oscats_discr_model_get_param_name:
 * @model: an #OscatsDiscrModel
 * @index: the parameter index
 * 
 * Returns: the name of parameter @index < #OscatsDiscrModel:Np
 */
const gchar* oscats_discr_model_get_param_name(const OscatsDiscrModel *model,
                                             guint index)
{
  g_return_val_if_fail(OSCATS_IS_DISCR_MODEL(model) && index < model->Np, NULL);
  return g_quark_to_string(model->names[index]);
}

/**
 * oscats_discr_model_has_param_name:
 * @model: an #OscatsDiscrModel
 * @name: parameter name to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_discr_model_has_param_name(const OscatsDiscrModel *model, 
                                         const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  if (!q) return FALSE;
  return oscats_discr_model_has_param(model, q);
}

/**
 * oscats_discr_model_has_param:
 * @model: an #OscatsDiscrModel
 * @name: parameter name (as a #GQuark) to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_discr_model_has_param(const OscatsDiscrModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_DISCR_MODEL(model), FALSE);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return TRUE;
  return FALSE;
}

/**
 * oscats_discr_model_get_param_by_index:
 * @model: an #OscatsDiscrModel
 * @index: the parameter index
 * 
 * Returns: the parameter @index < #OscatsDiscrModel:Np
 */
gdouble oscats_discr_model_get_param_by_index(const OscatsDiscrModel *model, guint index)
{
  g_return_val_if_fail(OSCATS_IS_DISCR_MODEL(model) && index < model->Np, 0);
  return model->params[index];
}

/**
 * oscats_discr_model_get_param:
 * @model: an #OscatsDiscrModel
 * @name: the parameter name (as a #GQuark)
 * 
 * Returns: the parameter @name
 */
gdouble oscats_discr_model_get_param(const OscatsDiscrModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_DISCR_MODEL(model), 0);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return model->params[i];
  g_critical("Unknown parameter %s", g_quark_to_string(name));
  return 0;
}

/**
 * oscats_discr_model_get_param_by_name:
 * @model: an #OscatsDiscrModel
 * @name: the parameter name
 * 
 * The #GQuark version oscats_discr_model_get_param() is faster.
 *
 * Returns: the parameter called @name
 */
gdouble oscats_discr_model_get_param_by_name(const OscatsDiscrModel *model,
                                           const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  g_return_val_if_fail(q != 0, 0);
  return oscats_discr_model_get_param(model, q);
}

/**
 * oscats_discr_model_set_param_by_index:
 * @model: an #OscatsDiscrModel
 * @index: the parameter index < #OscatsDiscrModel:Np
 * @value: the #gdouble value to set
 */
void oscats_discr_model_set_param_by_index(OscatsDiscrModel *model, guint index,
                                         gdouble value)
{
  g_return_if_fail(OSCATS_IS_DISCR_MODEL(model) && index < model->Np);
  model->params[index] = value;
}

/**
 * oscats_discr_model_set_param:
 * @model: an #OscatsDiscrModel
 * @name: the parameter name (as a #GQuark)
 * @value: the #gdouble to set
 */
void oscats_discr_model_set_param(OscatsDiscrModel *model,
                                GQuark name, gdouble value)
{
  guint i;
  g_return_if_fail(OSCATS_IS_DISCR_MODEL(model));
  for (i=0; i < model->Np; i++)
    if (model->names[i] == name)
    {
      model->params[i] = value;
      return;
    }
  g_critical("Unknown parameter %s", g_quark_to_string(name));
}

/**
 * oscats_discr_model_set_param_by_name:
 * @model: an #OscatsDiscrModel
 * @name: the parameter name
 * @value: the #gdouble to set
 *
 * The #GQuark version oscats_discr_model_set_param() is faster.
 */
void oscats_discr_model_set_param_by_name(OscatsDiscrModel *model,
                                        const gchar *name, gdouble value)
{
  GQuark q = g_quark_try_string(name);
  g_return_if_fail(q != 0);
  oscats_discr_model_set_param(model, q, value);
}

