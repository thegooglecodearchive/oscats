/* OSCATS: Open-Source Computerized Adaptive Testing System
 * One-Parameter Logistic IRT Model
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
 * SECTION:l1p
 * @title:OscatsContModelL1p
 * @short_description: One-Parameter Logistic (1PL) Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/l1p.h"

G_DEFINE_TYPE(OscatsContModelL1p, oscats_cont_model_l1p, OSCATS_TYPE_CONT_MODEL);

enum
{
  PARAM_B,
  NUM_PARAMS,
};

static void model_constructed (GObject *object);
static guint8 get_max (const OscatsContModel *model);
static gdouble P(const OscatsContModel *model, guint resp, const GGslVector *theta, const OscatsCovariates *covariates);
static gdouble distance(const OscatsContModel *model, const GGslVector *theta, const OscatsCovariates *covariates);
static void logLik_dtheta(const OscatsContModel *model,
                          guint resp, const GGslVector *theta,
                          const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean I);
static void logLik_dparam(const OscatsContModel *model,
                          guint resp, const GGslVector *theta,
                          const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_cont_model_l1p_class_init (OscatsContModelL1pClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsContModelClass *model_class = OSCATS_CONT_MODEL_CLASS(klass);

  gobject_class->constructed = model_constructed;

  model_class->get_max = get_max;
  model_class->P = P;
  model_class->distance = distance;
  model_class->logLik_dtheta = logLik_dtheta;
  model_class->logLik_dparam = logLik_dparam;
  
}

static void oscats_cont_model_l1p_init (OscatsContModelL1p *self)
{
}

static void model_constructed(GObject *object)
{
  OscatsContModel *model = OSCATS_CONT_MODEL(object);
  G_OBJECT_CLASS(oscats_cont_model_l1p_parent_class)->constructed(object);

  model->Np = NUM_PARAMS + model->Ncov;
  model->params = g_new0(gdouble, model->Np);

  model->names = g_new(GQuark, model->Np);
  model->names[PARAM_B] = g_quark_from_string("Diff");
  
  if (model->Ncov > 0)
  {
    gchar cov[20];
    guint i;
    model->covariates = model->names + NUM_PARAMS;
    for (i=0; i < model->Ncov; i++)
    {
      sprintf(cov, "Cov.%d", i+1);
      model->covariates[i] = g_quark_from_string(cov);
    }
  }

}

static guint8 get_max (const OscatsContModel *model)
{
  return 1;
}

static gdouble P(const OscatsContModel *model, guint resp,
                 const GGslVector *theta, const OscatsCovariates *covariates)
{
  guint i;
  gdouble z = 0;
  g_return_val_if_fail(resp == 0 || resp == 1, 0);
  switch (model->Ndims)
  {
    case 2:
      z = -gsl_vector_get(theta->v, model->dim2);
    case 1:
      z -= gsl_vector_get(theta->v, model->dim1);
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
        z -= gsl_vector_get(theta->v, model->dims[i]);
  }
  for (i=0; i < model->Ncov; i++)
    z -= oscats_covariates_get(covariates, model->covariates[i]) *
         model->params[NUM_PARAMS+i];
  z += model->params[PARAM_B];
  return 1/(1+exp(resp ? z : -z));
}

static gdouble distance(const OscatsContModel *model, const GGslVector *theta,
                        const OscatsCovariates *covariates)
{
  guint i;
  gdouble z = 0;
  switch (model->Ndims)
  {
    case 2:
      z = gsl_vector_get(theta->v, model->dim2);
    case 1:
      z += gsl_vector_get(theta->v, model->dim1);
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
        z += gsl_vector_get(theta->v, model->dims[i]);
  }
  for (i=0; i < model->Ncov; i++)
    z += oscats_covariates_get(covariates, model->covariates[i]) *
         model->params[NUM_PARAMS+i];
  z -= model->params[PARAM_B];
  return fabs(z);
}

/* d[log(P), theta_i] = Q
 * d[log(Q), theta_i] = -P
 * d[log(P), theta_i, theta_j] = -PQ
 * d[log(Q), theta_i, theta_j] = -PQ
 */
static void logLik_dtheta(const OscatsContModel *model,
                          guint resp, const GGslVector *theta,
                          const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint i, j, I, J;
  guint hes_stride = (hes ? hes_v->tda : 0);
  gdouble p, grad_val, hes_val;
  g_return_if_fail(resp == 0 || resp == 1);
  p = P(model, 1, theta, covariates);
  if (resp) grad_val = (1-p);
  else      grad_val = -p;
  hes_val = -p*(1-p);
  if (Inf)
    hes_val *= -(resp ? p : 1-p);
  switch (model->Ndims)
  {
    case 2:
      if (grad) grad_v->data[model->dim2 * grad_v->stride] += grad_val;
      if (hes)
      {
        hes_v->data[model->dim2 * hes_stride + model->dim2] += hes_val;
        hes_v->data[model->dim1 * hes_stride + model->dim2] += hes_val;
        hes_v->data[model->dim2 * hes_stride + model->dim1] += hes_val;
      }
    case 1:
      if (grad) grad_v->data[model->dim1 * grad_v->stride] += grad_val;
      if (hes)
        hes_v->data[model->dim1 * hes_stride + model->dim1] += hes_val;
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
      {
        I = model->dims[i];
        if (grad) grad_v->data[I*grad_v->stride] += grad_val;
        if (hes)
        {
          hes_v->data[I*hes_stride + I] += hes_val;
          for (j=i+1; j < model->Ndims; j++)
          {
            J = model->dims[j];
            hes_v->data[I*hes_stride + J] += hes_val;
            hes_v->data[J*hes_stride + I] += hes_val;
          }
        }
      }
  }
}

/* d[log(P), b] = -Q = P-1
 * d[log(Q), b] = P
 * d[log(P), b^2] = -PQ
 * d[log(Q), b^2] = -PQ
 */
static void logLik_dparam(const OscatsContModel *model,
                          guint resp, const GGslVector *theta,
                          const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes)
{
  gdouble p, grad_val, hes_val;
  g_return_if_fail(resp == 0 || resp == 1);
  p = P(model, 1, theta, covariates);
  if (resp) grad_val = p-1;
  else      grad_val = p;
  hes_val = -p*(1-p);
  if (grad) grad->v->data[0] += grad_val;
  if (hes) hes->v->data[0] += hes_val;
  if (model->Ncov > 0)
  {
    gdouble val, val2;
    gsl_vector *grad_v = (grad ? grad->v : NULL);
    gsl_matrix *hes_v = (hes ? hes->v : NULL);
    guint i, j;
    guint hes_stride = (hes ? hes_v->tda : 0);
    grad_val = -grad_val;
    for (i=NUM_PARAMS; i < model->Np; i++)
    {
      val = oscats_covariates_get(covariates, model->names[i]);
      if (grad_v) grad_v->data[i*grad_v->stride] += val * grad_val;
      if (hes_v)
      {
        hes_v->data[i*hes_stride + i] += val * val * hes_val;
        hes_v->data[i*hes_stride + PARAM_B] += -val * hes_val;
        hes_v->data[PARAM_B*hes_stride + i] += -val * hes_val;
        for (j=i+1; j < model->Np; j++)
        {
          val2 = oscats_covariates_get(covariates, model->names[j]);
          hes_v->data[i*hes_stride + j] += val * val2 * hes_val;
          hes_v->data[j*hes_stride + i] += val * val2 * hes_val;
        }
      }
    }
  } // covariates
}

