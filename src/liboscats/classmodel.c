/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
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
 * SECTION:classmodel
 * @title:OscatsClassModel
 * @short_description: Abstract IRT Model Class
 */

#include "classmodel.h"

G_DEFINE_TYPE(OscatsClassModel, oscats_class_model, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_TEST_DIM,
  PROP_N_DIMS,
  PROP_DIMS,
  PROP_N_PARAMS,
};

static void oscats_class_model_dispose (GObject *object);
static void oscats_class_model_finalize (GObject *object);
static void oscats_class_model_constructed (GObject *object);
static void oscats_class_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_class_model_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
static guint8 null_get_max (const OscatsClassModel *model)
{
  g_critical("Abstract Classification Model should be overloaded.");
  return 0;
}
static gdouble null_P (const OscatsClassModel *model, guint resp,
                       const OscatsAttributes *attr)
{
  g_critical("Abstract Classification Model should be overloaded.");
  return 0;
}
static void null_logLik (const OscatsClassModel *model, guint resp,
                         const OscatsAttributes *attr,
                         GGslVector *grad, GGslMatrix *hes)
{
  g_critical("Abstract Classification Model should be overloaded.");
}
                   
static void oscats_class_model_class_init (OscatsClassModelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->constructed = oscats_class_model_constructed;
  gobject_class->dispose = oscats_class_model_dispose;
  gobject_class->finalize = oscats_class_model_finalize;
  gobject_class->set_property = oscats_class_model_set_property;
  gobject_class->get_property = oscats_class_model_get_property;
  
  klass->get_max = null_get_max;
  klass->P = null_P;
  klass->logLik_dparam = null_logLik;
  
/**
 * OscatsClassModel:Np:
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
 * OscatsClassModel:testDim:
 *
 * The number of attributes for the test.
 * This is derived from #OscatsClassModel:dims at construction.
 */
  pspec = g_param_spec_uint("testDim", "Test Dimension", 
                            "Total IRT Dimension for the test",
                            1, OSCATS_MAX_ATTRIBUTES, 1,
                            G_PARAM_READABLE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_TEST_DIM, pspec);

/**
 * OscatsClassModel:Ndims:
 *
 * The dimension of the Classification model (<= the dimension of the test).
 * Determiend from #OscatsClassModel:dims.  Default is 1.
 */
  pspec = g_param_spec_uint("Ndims", "Num Dims", 
                            "Dimension of the Classification model",
                            1, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_DIMS, pspec);

/**
 * OscatsClassModel:dims:
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

static void oscats_class_model_init (OscatsClassModel *self)
{
}

static void oscats_class_model_constructed(GObject *object)
{
  int i, j=0;
  OscatsClassModel *model = OSCATS_CLASS_MODEL(object);
//  G_OBJECT_CLASS(oscats_class_model_parent_class)->constructed(object);

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

static void oscats_class_model_dispose (GObject *object)
{
  OscatsClassModel *self = OSCATS_CLASS_MODEL(object);
  G_OBJECT_CLASS(oscats_class_model_parent_class)->dispose(object);
  if (self->dimsFlags) g_object_unref(self->dimsFlags);
  self->dimsFlags = NULL;
}

static void oscats_class_model_finalize (GObject *object)
{
  OscatsClassModel *self = OSCATS_CLASS_MODEL(object);
  g_free(self->params);
  g_free(self->names);
  g_free(self->dims);
  G_OBJECT_CLASS(oscats_class_model_parent_class)->finalize(object);
}

static void oscats_class_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsClassModel *self = OSCATS_CLASS_MODEL(object);
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

static void oscats_class_model_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec)
{
  OscatsClassModel *self = OSCATS_CLASS_MODEL(object);
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
 * oscats_class_model_get_max:
 * @model: an #OscatsClassModel
 *
 * Returns: the maximum valid response category for this model
 */
guint8 oscats_class_model_get_max(const OscatsClassModel *model)
{
  g_return_val_if_fail(OSCATS_IS_CLASS_MODEL(model), 0);
  return OSCATS_CLASS_MODEL_GET_CLASS(model)->get_max(model);
}

/**
 * oscats_class_model_P:
 * @model: an #OscatsClassModel
 * @resp: the examinee response value
 * @attr: the #OscatsAttributes value
 * 
 * Calculates the probability of response @resp, given latent class @attr.
 *
 * Returns: the computed probability
 */
gdouble oscats_class_model_P(const OscatsClassModel *model, guint resp,
                             const OscatsAttributes *attr)
{
  g_return_val_if_fail(OSCATS_IS_CLASS_MODEL(model) &&
                       OSCATS_IS_ATTRIBUTES(attr), 0);
  g_return_val_if_fail(model->dimsFlags->num == attr->num, 0);
  return OSCATS_CLASS_MODEL_GET_CLASS(model)->P(model, resp, attr);
}

/**
 * oscats_class_model_logLik_dparam:
 * @model: an #OscatsClassModel
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
void oscats_class_model_logLik_dparam(const OscatsClassModel *model,
                                    guint resp, const OscatsAttributes *attr,
                                    GGslVector *grad, GGslMatrix *hes)
{
  g_return_if_fail(OSCATS_IS_CLASS_MODEL(model) && OSCATS_IS_ATTRIBUTES(attr));
  g_return_if_fail(model->dimsFlags->num == attr->num);
  if (grad) g_return_if_fail(G_GSL_IS_VECTOR(grad) && grad->v &&
                             grad->v->size == model->Np);
  if (hes) g_return_if_fail(G_GSL_IS_MATRIX(hes) && hes->v &&
                            hes->v->size1 == model->Np && 
                            hes->v->size2 == model->Np);
  OSCATS_CLASS_MODEL_GET_CLASS(model)->logLik_dparam(model, resp, attr,
                                                     grad, hes);
}

/**
 * oscats_class_model_get_param_name:
 * @model: an #OscatsClassModel
 * @index: the parameter index
 * 
 * Returns: the name of parameter @index < #OscatsClassModel:Np
 */
const gchar* oscats_class_model_get_param_name(const OscatsClassModel *model,
                                             guint index)
{
  g_return_val_if_fail(OSCATS_IS_CLASS_MODEL(model) && index < model->Np, NULL);
  return g_quark_to_string(model->names[index]);
}

/**
 * oscats_class_model_has_param_name:
 * @model: an #OscatsClassModel
 * @name: parameter name to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_class_model_has_param_name(const OscatsClassModel *model, 
                                         const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  if (!q) return FALSE;
  return oscats_class_model_has_param(model, q);
}

/**
 * oscats_class_model_has_param:
 * @model: an #OscatsClassModel
 * @name: parameter name (as a #GQuark) to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_class_model_has_param(const OscatsClassModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_CLASS_MODEL(model), FALSE);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return TRUE;
  return FALSE;
}

/**
 * oscats_class_model_get_param_by_index:
 * @model: an #OscatsClassModel
 * @index: the parameter index
 * 
 * Returns: the parameter @index < #OscatsClassModel:Np
 */
gdouble oscats_class_model_get_param_by_index(const OscatsClassModel *model, guint index)
{
  g_return_val_if_fail(OSCATS_IS_CLASS_MODEL(model) && index < model->Np, 0);
  return model->params[index];
}

/**
 * oscats_class_model_get_param:
 * @model: an #OscatsClassModel
 * @name: the parameter name (as a #GQuark)
 * 
 * Returns: the parameter @name
 */
gdouble oscats_class_model_get_param(const OscatsClassModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_CLASS_MODEL(model), 0);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return model->params[i];
  g_critical("Unknown parameter %s", g_quark_to_string(name));
  return 0;
}

/**
 * oscats_class_model_get_param_by_name:
 * @model: an #OscatsClassModel
 * @name: the parameter name
 * 
 * The #GQuark version oscats_class_model_get_param() is faster.
 *
 * Returns: the parameter called @name
 */
gdouble oscats_class_model_get_param_by_name(const OscatsClassModel *model,
                                           const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  g_return_val_if_fail(q != 0, 0);
  return oscats_class_model_get_param(model, q);
}

/**
 * oscats_class_model_set_param_by_index:
 * @model: an #OscatsClassModel
 * @index: the parameter index < #OscatsClassModel:Np
 * @value: the #gdouble value to set
 */
void oscats_class_model_set_param_by_index(OscatsClassModel *model, guint index,
                                         gdouble value)
{
  g_return_if_fail(OSCATS_IS_CLASS_MODEL(model) && index < model->Np);
  model->params[index] = value;
}

/**
 * oscats_class_model_set_param:
 * @model: an #OscatsClassModel
 * @name: the parameter name (as a #GQuark)
 * @value: the #gdouble to set
 */
void oscats_class_model_set_param(OscatsClassModel *model,
                                GQuark name, gdouble value)
{
  guint i;
  g_return_if_fail(OSCATS_IS_CLASS_MODEL(model));
  for (i=0; i < model->Np; i++)
    if (model->names[i] == name)
    {
      model->params[i] = value;
      return;
    }
  g_critical("Unknown parameter %s", g_quark_to_string(name));
}

/**
 * oscats_class_model_set_param_by_name:
 * @model: an #OscatsClassModel
 * @name: the parameter name
 * @value: the #gdouble to set
 *
 * The #GQuark version oscats_class_model_set_param() is faster.
 */
void oscats_class_model_set_param_by_name(OscatsClassModel *model,
                                        const gchar *name, gdouble value)
{
  GQuark q = g_quark_try_string(name);
  g_return_if_fail(q != 0);
  oscats_class_model_set_param(model, q, value);
}

