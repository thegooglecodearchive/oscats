/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Noisy Inputs Deterministic And Gate (NIDA) Classification Model
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
 * SECTION:nida
 * @title:OscatsDiscrModelNida
 * @short_description: Noisy Input Determistic And Gate (NIDA) Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/nida.h"

enum
{
  PARAM_GUESS,
  PARAM_SLIP,
};

G_DEFINE_TYPE(OscatsDiscrModelNida, oscats_discr_model_nida, OSCATS_TYPE_DISCR_MODEL);

static void model_constructed (GObject *object);
static guint8 get_max (const OscatsDiscrModel *model);
static gdouble P(const OscatsDiscrModel *model, guint resp, const OscatsAttributes *attr);
static void logLik_dparam(const OscatsDiscrModel *model,
                          guint resp, const OscatsAttributes *attr,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_discr_model_nida_class_init (OscatsDiscrModelNidaClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsDiscrModelClass *model_class = OSCATS_DISCR_MODEL_CLASS(klass);

  gobject_class->constructed = model_constructed;

  model_class->get_max = get_max;
  model_class->P = P;
  model_class->logLik_dparam = logLik_dparam;
  
}

static void oscats_discr_model_nida_init (OscatsDiscrModelNida *self)
{
}

static void model_constructed(GObject *object)
{
  OscatsDiscrModel *model = OSCATS_DISCR_MODEL(object);
  GString *str;
  guint i;
  G_OBJECT_CLASS(oscats_discr_model_nida_parent_class)->constructed(object);

  model->Np = 2*model->Ndims;
  model->params = g_new0(gdouble, model->Np);

  str = g_string_sized_new(10);
  model->names = g_new(GQuark, model->Np);
  for (i=0; i < model->Ndims; i++)
  {
    g_string_printf(str, "Guess.%d", model->dims[i]+1);
    model->names[PARAM_GUESS*model->Ndims+i] = g_quark_from_string(str->str);
    g_string_printf(str, "Diff.%d", model->dims[i]+1);
    model->names[PARAM_SLIP*model->Ndims+i] = g_quark_from_string(str->str);
  }
  g_string_free(str, TRUE);
  
}

static guint8 get_max (const OscatsDiscrModel *model)
{
  return 1;
}

static gdouble P(const OscatsDiscrModel *model, guint resp,
                 const OscatsAttributes *attr)
{
  gdouble p = 1;
  guint i;
  g_return_val_if_fail(resp == 0 || resp == 1, 0);
  for (i=0; i < model->Ndims; i++)
    p *= (oscats_attributes_get(attr, model->dims[i])
          ? 1-model->params[PARAM_SLIP*model->Ndims+i]
          : model->params[PARAM_GUESS*model->Ndims+i]);
  return (resp ? p : 1-p);
}

static void logLik_dparam(const OscatsDiscrModel *model,
                          guint resp, const OscatsAttributes *attr,
                          GGslVector *grad, GGslMatrix *hes)
{
  gdouble *g = (grad ? grad->v->data : NULL);
  gdouble *H = (hes ? hes->v->data : NULL);
  gdouble p, logit_p, grad_val, hes_val;
  guint j, k, Ndims = model->Ndims;
  guint grad_stride = (grad ? grad->v->stride : 0);
  guint hes_stride = (hes ? hes->v->tda : 0);
  g_return_if_fail(resp == 0 || resp == 1);

#define GRAD(t, j) (g[((t)*Ndims+(j))*grad_stride])
#define HES(t1, j, t2, k) (H[((t1)*Ndims+(j))*hes_stride + ((t2)*Ndims+(k))])

  if (resp)
    for (j=0; j < Ndims; j++)
      if (oscats_attributes_get(attr, model->dims[j]))
      {
        grad_val = 1 / (model->params[PARAM_SLIP*Ndims+j] - 1);
        if (g) GRAD(PARAM_SLIP, j) += grad_val;
        if (H) HES(PARAM_SLIP, j, PARAM_SLIP, j) -= grad_val*grad_val;
      } else {		// alpha_j == 0
        grad_val = 1 / model->params[PARAM_GUESS*Ndims+j];
        if (g) GRAD(PARAM_GUESS, j) += grad_val;
        if (H) HES(PARAM_GUESS, j, PARAM_GUESS, j) -= grad_val*grad_val;
      }
  else 			// !resp
  {
    p = P(model, 0, attr);
    logit_p = p/(1-p);
    for (j=0; j < Ndims; j++)
      if (oscats_attributes_get(attr, model->dims[j]))
      {
        grad_val = logit_p/(1-model->params[PARAM_SLIP*Ndims+j]);
        GRAD(PARAM_SLIP, j) += grad_val;
        HES(PARAM_SLIP, j, PARAM_SLIP, j) -= grad_val*grad_val;
        grad_val /= (1-p);
        for (k=j+1; k < Ndims; k++)
          if (oscats_attributes_get(attr, model->dims[k]))
          {
            hes_val = grad_val / (1-model->params[PARAM_SLIP*Ndims+k]);
            HES(PARAM_SLIP, j, PARAM_SLIP, k) -= hes_val;
            HES(PARAM_SLIP, k, PARAM_SLIP, j) -= hes_val;
          } else {	// alpha_k == 0
            hes_val = grad_val / model->params[PARAM_GUESS*Ndims+k];
            HES(PARAM_SLIP, j, PARAM_GUESS, k) += hes_val;
            HES(PARAM_GUESS, k, PARAM_SLIP, j) += hes_val;
          }
      } else {		// alpha_j == 0
        grad_val = logit_p / model->params[PARAM_GUESS*Ndims+j];
        GRAD(PARAM_GUESS, j) -= grad_val;
        HES(PARAM_GUESS, j, PARAM_GUESS, j) -= grad_val*grad_val;
        grad_val /= (1-p);
        for (k=j+1; k < Ndims; k++)
          if (oscats_attributes_get(attr, model->dims[k]))
          {
            hes_val = grad_val / (1-model->params[PARAM_SLIP*Ndims+k]);
            HES(PARAM_GUESS, j, PARAM_SLIP, k) += hes_val;
            HES(PARAM_SLIP, k, PARAM_GUESS, j) += hes_val;
          } else {	// alpha_k == 0
            hes_val = grad_val / model->params[PARAM_GUESS*Ndims+k];
            HES(PARAM_GUESS, j, PARAM_GUESS, k) += hes_val;
            HES(PARAM_GUESS, k, PARAM_GUESS, j) += hes_val;
          }
      }
  }
}
