/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Nominal Response IRT Model
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
 * SECTION:nominal
 * @title:OscatsContModelNominal
 * @short_description: Nominal Response Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/nominal.h"

G_DEFINE_TYPE(OscatsContModelNominal, oscats_cont_model_nominal, OSCATS_TYPE_CONT_MODEL);

enum
{
  PARAM_B,
  PARAM_A_FIRST,
};

enum
{
  PROP_0,
  PROP_NUM_CAT,
};

static void model_constructed (GObject *object);
static void model_set_property(GObject *object, guint prop_id,
                               const GValue *value, GParamSpec *pspec);
static void model_get_property(GObject *object, guint prop_id,
                               GValue *value, GParamSpec *pspec);
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

static void oscats_cont_model_nominal_class_init (OscatsContModelNominalClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsContModelClass *model_class = OSCATS_CONT_MODEL_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->constructed = model_constructed;
  gobject_class->set_property = model_set_property;
  gobject_class->get_property = model_get_property;

  model_class->get_max = get_max;
  model_class->P = P;
  model_class->distance = distance;
  model_class->logLik_dtheta = logLik_dtheta;
  model_class->logLik_dparam = logLik_dparam;
  
/**
 * OscatsContModelNominal:Ncat:
 *
 * The number of response categories in the IRT model,
 * numered 0, 1, ..., Ncat-1.  The default number of categories is 3,
 * which is also the minimum.
 */
  pspec = g_param_spec_uint("Ncat", "Number of Categories", 
                            "Number of response categories in the IRT model",
                            3, G_MAXUINT, 3,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM_CAT, pspec);

}

static void oscats_cont_model_nominal_init (OscatsContModelNominal *self)
{
}

static void model_constructed(GObject *object)
{
  GString *str;
  guint i, k, Ncat;
  OscatsContModel *model = OSCATS_CONT_MODEL(object);
  G_OBJECT_CLASS(oscats_cont_model_nominal_parent_class)->constructed(object);
  Ncat = OSCATS_CONT_MODEL_NOMINAL(model)->Ncat;

  model->Np = (model->Ndims+1)*Ncat + model->Ncov;
  model->params = g_new0(gdouble, model->Np);

  str = g_string_sized_new(10);
  model->names = g_new(GQuark, model->Np);
  for (k=0; k < Ncat; k++)
  {
    g_string_printf(str, "Diff.%d", k+1);
    model->names[k] = g_quark_from_string(str->str);
    if (model->Ndims < 3)
    {
        g_string_printf(str, "Discr.%d.%d", k+1, model->dim1+1);
        model->names[1*Ncat+k] = g_quark_from_string(str->str);
        if (model->Ndims == 2)
        {
          g_string_printf(str, "Discr.%d.%d", k+1, model->dim2+1);
          model->names[2*Ncat+k] = g_quark_from_string(str->str);
        }
    } else
      for (i=0; i < model->Ndims; i++)
      {
        g_string_printf(str, "Discr.%d.%d", k+1, model->dims[i]+1);
        model->names[(i+1)*Ncat+k] = g_quark_from_string(str->str);
      }
  }
  
  if (model->Ncov > 0)
  {
    model->covariates = model->names + (2*model->Ndims);
    for (i=0; i < model->Ncov; i++)
    {
      g_string_printf(str, "Cov.%d", i+1);
      model->covariates[i] = g_quark_from_string(str->str);
    }
  }
  g_string_free(str, TRUE);

}

static void model_set_property(GObject *object, guint prop_id,
                               const GValue *value, GParamSpec *pspec)
{
  OscatsContModelNominal *self = OSCATS_CONT_MODEL_NOMINAL(object);
  switch (prop_id)
  {
    case PROP_NUM_CAT:			// construction only
      self->Ncat = g_value_get_uint(value)-1;
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void model_get_property(GObject *object, guint prop_id,
                               GValue *value, GParamSpec *pspec)
{
  OscatsContModelNominal *self = OSCATS_CONT_MODEL_NOMINAL(object);
  switch (prop_id)
  {
    case PROP_NUM_CAT:
      g_value_set_uint(value, self->Ncat+1);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static guint8 get_max (const OscatsContModel *model)
{
  return ((OscatsContModelNominal*)model)->Ncat;
}

static gdouble P(const OscatsContModel *model, guint resp,
                 const GGslVector *theta, const OscatsCovariates *covariates)
{
  guint i, k, Ncat = ((OscatsContModelNominal*)model)->Ncat;
  gdouble z[Ncat], denom=1, cov=0;
  g_return_val_if_fail(resp >= 0 && resp <= Ncat, 0);
  switch (model->Ndims)
  {
    case 2:
      for (k=0; k < Ncat; k++)
        z[k] = model->params[(PARAM_A_FIRST)*Ncat+k] * gsl_vector_get(theta->v, model->dim1)
             + model->params[(PARAM_A_FIRST+1)*Ncat+k] * gsl_vector_get(theta->v, model->dim2)
             - model->params[(PARAM_B)*Ncat+k];
      break;
    case 1:
      for (k=0; k < Ncat; k++)
        z[k] = model->params[PARAM_A_FIRST*Ncat+k] * gsl_vector_get(theta->v, model->dim1)
             - model->params[PARAM_B*Ncat+k];
      break;
    
    default:
      for (k=0; k < Ncat; k++)
      {
        z[k] = -model->params[PARAM_B*Ncat+k];
        for (i=0; i < model->Ndims; i++)
          z[k] += model->params[(PARAM_A_FIRST+i)*Ncat+k] * gsl_vector_get(theta->v, model->dims[i]);
      }
  }
  for (i=model->Np-model->Ncov; i < model->Np; i++)
    cov += oscats_covariates_get(covariates, model->names[i]) * model->params[i];
  for (k=0; k < Ncat; k++)
    denom += exp(z[k]+cov);
  return (resp == 0 ? 1 : exp(z[resp-1]+cov)) / denom;
}

static gdouble distance(const OscatsContModel *model, const GGslVector *theta,
                        const OscatsCovariates *covariates)
{
  guint i, k, Ncat = ((OscatsContModelNominal*)model)->Ncat;
  gdouble z, min, cov=0;
  for (i=model->Np-model->Ncov; i < model->Np; i++)
    cov += oscats_covariates_get(covariates, model->names[i]) * model->params[i];
  switch (model->Ndims)
  {
    case 2:
      min = fabs(
               model->params[(PARAM_A_FIRST)*Ncat+0] * gsl_vector_get(theta->v, model->dim1)
             + model->params[(PARAM_A_FIRST+1)*Ncat+0] * gsl_vector_get(theta->v, model->dim2)
             - model->params[PARAM_B*Ncat+0] + cov);
      for (k=1; k < Ncat; k++)
        if (min < (z = fabs(
               model->params[(PARAM_A_FIRST)*Ncat+k] * gsl_vector_get(theta->v, model->dim1)
             + model->params[(PARAM_A_FIRST+1)*Ncat+k] * gsl_vector_get(theta->v, model->dim2)
             - model->params[(PARAM_B)*Ncat+k] + cov)))
          min = z;
      break;
    case 1:
      min = fabs(
               model->params[PARAM_A_FIRST*Ncat+0] * gsl_vector_get(theta->v, model->dim1)
             - model->params[PARAM_B*Ncat+0] + cov);
      for (k=1; k < Ncat; k++)
        if (min < (z = fabs(
               model->params[PARAM_A_FIRST*Ncat+k] * gsl_vector_get(theta->v, model->dim1)
             - model->params[PARAM_B*Ncat+k] + cov)))
          min = z;
      break;
    
    default:
      z = cov - model->params[PARAM_B*Ncat+0];
      for (i=0; i < model->Ndims; i++)
        z += model->params[(PARAM_A_FIRST+i)*Ncat+0] * gsl_vector_get(theta->v, model->dims[i]);
      min = fabs(z);
      for (k=1; k < Ncat; k++)
      {
        z = cov - model->params[PARAM_B*Ncat+k];
        for (i=0; i < model->Ndims; i++)
          z += model->params[(PARAM_A_FIRST+i)*Ncat+k] * gsl_vector_get(theta->v, model->dims[i]);
        if (min < (z = fabs(z)))
          min = z;
      }
  }
  return min;
}

/* a_0i = b_0 = 0
 * d[log(P_k), theta_j] = a_kj - sum_y a_yj P_y
 * d[P_x, theta_j] = P_x (a_xj - sum_y a_yj P_y)
 * d[log(P_k), theta_i, theta_j] = - sum_x a_xi P_x (a_xj - sum_y a_yj P_y)
 */
static void logLik_dtheta(const OscatsContModel *model,
                          guint resp, const GGslVector *theta,
                          const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint i, j, I, J, x, y, Ncat = ((OscatsContModelNominal*)model)->Ncat;
  guint hes_stride = (hes ? hes_v->tda : 0);
  gdouble p[Ncat], grad_val, inf_factor = 1;
  g_return_if_fail(resp >= 0 && resp <= Ncat);

  for (i=0; i < Ncat; i++)
    p[i] = P(model, i+1, theta, covariates);
  if (Inf)
  {
    if (resp == 0)
      for(inf_factor = -1, i=0; i < Ncat; i++)
        inf_factor += p[i];
    else
      inf_factor = -p[resp-1];
  }

  switch (model->Ndims)
  {
    case 2:
      I = model->dim2;
      J = model->dim1;
      grad_val = 0;
      for (y=0; y < Ncat; y++)
        grad_val += model->params[(PARAM_A_FIRST+1)*Ncat+y] * p[y];
      if (grad) grad_v->data[I] +=
        (resp > 0 ? model->params[(PARAM_A_FIRST+1)*Ncat+resp-1] : 0) - grad_val;
      if (hes)
        for (x=0; x < Ncat; x++)
        {
          gdouble tmp;
          hes_v->data[I*hes_stride+I] -=
            model->params[(PARAM_A_FIRST+1)*Ncat+x] * p[x] *
            (model->params[(PARAM_A_FIRST+1)*Ncat+x] - grad_val);
          tmp = model->params[(PARAM_A_FIRST)*Ncat+x] * p[x] *
                (model->params[(PARAM_A_FIRST+1)*Ncat+x] - grad_val);
          hes_v->data[I*hes_stride+J] -= tmp;
          hes_v->data[J*hes_stride+I] -= tmp;
        }
    case 1:
      J = model->dim1;
      grad_val = 0;
      for (y=0; y < Ncat; y++)
        grad_val += model->params[PARAM_A_FIRST*Ncat+y] * p[y];
      if (grad) grad_v->data[J*grad_v->stride] +=
        (resp > 0 ? model->params[PARAM_A_FIRST*Ncat+resp-1] : 0) - grad_val;
      if (hes)
        for (x=0; x < Ncat; x++)
          hes_v->data[J*hes_stride+J] -=
            model->params[PARAM_A_FIRST*Ncat+x] * p[x] *
            (model->params[PARAM_A_FIRST*Ncat+x] - grad_val);
      break;
    
    default:
      for (j=0; j < model->Ndims; j++)
      {
        J = model->dims[j];
        grad_val = 0;
        for (y=0; y < Ncat; y++)
          grad_val += model->params[(PARAM_A_FIRST+j)*Ncat+y] * p[y];
        if (grad) grad_v->data[J*grad_v->stride] +=
          (resp > 0 ? model->params[(PARAM_A_FIRST+j)*Ncat+resp-1] : 0) - grad_val;
        if (hes)
          for (x=0; x < Ncat; x++)
          {
            hes_v->data[J*hes_stride+J] -=
              model->params[(PARAM_A_FIRST+j)*Ncat+x] * p[x] *
              (model->params[(PARAM_A_FIRST+j)*Ncat+x] - grad_val);
            for (i=j+1; i < model->Ndims; i++)
            {
              gdouble tmp = model->params[(PARAM_A_FIRST+i)*Ncat+x] * p[x] *
                        (model->params[(PARAM_A_FIRST+j)*Ncat+x] - grad_val);
              I = model->dims[i];
              hes_v->data[J*hes_stride+I] -= tmp;
              hes_v->data[I*hes_stride+J] -= tmp;
            }
          }
      }
  }
}

/* a_0i = b_0 = 0
 * D[log(P_k), b_i] = P_i - I_{k==i}
 * D[log(P_k), a_ix] = theta_x (I_{k==i} - P_i)
 * D[log(P_k), d_x] = cov_x (P_0 - I_{k==0})

 * D[P_k, b_i] = P_k (P_i - I_{k==i})
 * D[P_k, a_ix] = P_k theta_x (I_{k==i} - P_i)
 * D[P_k, d_x] = cov_x P_0 (P_k - I_{k==0})
 
 * D[log(P_k), b_i, b_j] = P_i (P_j - I_{i==j})
 * D[log(P_k), b_i, a_jy] = - P_i theta_y (P_j - I_{i==j})
 * D[log(P_k), a_ix, a_jy] = theta_x theta_y P_i (P_j - I_{i==j})
 * D[log(P_k), b_i, d_y] = cov_y P_0 P_i
 * D[log(P_k), a_ix, d_y] = - theta_x cov_y P_0 P_i
 * D[log(P_k), d_x, d_y] = cov_x cov_y P_0 (P_0 - 1)
 */
static void logLik_dparam(const OscatsContModel *model,
                          guint resp, const GGslVector *theta,
                          const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint Ncat = ((OscatsContModelNominal*)model)->Ncat;
  guint first_covariate = model->Np-model->Ncov;
  gdouble p[Ncat], p0, theta_1, theta_2, tmp;
  guint i, j, x, y;
  gint k = resp-1;
  guint hes_stride = (hes ? hes_v->tda : 0);
  g_return_if_fail(resp >= 0 && resp <= Ncat);

  p0 = P(model, 0, theta, covariates);
  for (i=0; i < Ncat; i++)
    p[i] = P(model, i+1, theta, covariates);

#define HES(x,y) (hes_v->data[(x)*hes_stride+(y)])

  // Difficulties
  if (grad)
    for (i=0; i < Ncat; i++)
      grad_v->data[(PARAM_B*Ncat+i)*grad_v->stride] +=
        p[i] - (k==i ? 1 : 0);
  if (hes)
    for (i=0; i < Ncat; i++)
      for (j=0; j < Ncat; j++)
        HES(PARAM_B*Ncat+i, PARAM_B*Ncat+j) +=
          p[i] * (p[j] - (i==j ? 1 : 0));

  // Discriminations
  switch (model->Ndims)
  {
    case 2:
      theta_1 = gsl_vector_get(theta->v, model->dim1);
      theta_2 = gsl_vector_get(theta->v, model->dim2);
      if (grad)
        for (i=0; i < Ncat; i++)
          grad_v->data[((PARAM_A_FIRST+1)*Ncat+i)*grad_v->stride] +=
            theta_2 * ((k==i ? 1 : 0) - p[i]);
      if (hes)
        for (i=0; i < Ncat; i++)
        {
          for (j=0; j < Ncat; j++)
          {
            tmp = p[i] * (p[j] - (i==j ? 1 : 0));
            HES(PARAM_B*Ncat+i, (PARAM_A_FIRST+1)*Ncat+j) -= tmp * theta_2;
            HES((PARAM_A_FIRST+1)*Ncat+i, PARAM_B*Ncat+j) -= tmp * theta_2;
            HES((PARAM_A_FIRST+1)*Ncat+i, (PARAM_A_FIRST+1)*Ncat+j) +=
              tmp * theta_2 * theta_2;
            HES((PARAM_A_FIRST)*Ncat+i, (PARAM_A_FIRST+1)*Ncat+j) +=
              tmp * theta_1 * theta_2;
            HES((PARAM_A_FIRST+1)*Ncat+i, (PARAM_A_FIRST)*Ncat+j) +=
              tmp * theta_1 * theta_2;
          }
          for (y=first_covariate; y < model->Np; y++)
          {
            gdouble val = oscats_covariates_get(covariates, model->names[y]);
            tmp = theta_2 * val * p0 * p[i];
            HES((PARAM_A_FIRST+1)*Ncat+i, y) -= tmp;
            HES(y, (PARAM_A_FIRST+1)*Ncat+i) -= tmp;
          }
        }
    case 1:
      theta_1 = gsl_vector_get(theta->v, model->dim1);
      if (grad)
        for (i=0; i < Ncat; i++)
          grad_v->data[(PARAM_A_FIRST*Ncat+i)*grad_v->stride] +=
            theta_1 * ((k==i ? 1 : 0) - p[i]);
      if (hes)
        for (i=0; i < Ncat; i++)
        {
          for (j=0; j < Ncat; j++)
          {
            tmp = p[i] * (p[j] - (i==j ? 1 : 0));
            HES(PARAM_B*Ncat+i, PARAM_A_FIRST*Ncat+j) -= tmp * theta_1;
            HES(PARAM_A_FIRST*Ncat+i, PARAM_B*Ncat+j) -= tmp * theta_1;
            HES(PARAM_A_FIRST*Ncat+i, PARAM_A_FIRST*Ncat+j) +=
              tmp * theta_1 * theta_1;
          }
          for (y=first_covariate; y < model->Np; y++)
          {
            gdouble val = oscats_covariates_get(covariates, model->names[y]);
            tmp = theta_1 * val * p0 * p[i];
            HES(PARAM_A_FIRST*Ncat+i, y) -= tmp;
            HES(y, PARAM_A_FIRST*Ncat+i) -= tmp;
          }
        }
      break;

    default :
      for (x=0; x < model->Ndims; x++)
      {
        theta_1 = gsl_vector_get(theta->v, model->dims[x]);
        if (grad)
          for (i=0; i < Ncat; i++)
            grad_v->data[((PARAM_A_FIRST+x)*Ncat+i)*grad_v->stride] +=
              theta_1 * ((k==i ? 1 : 0) - p[i]);
        if (hes)
          for (i=0; i < Ncat; i++)
          {
            for (j=0; j < Ncat; j++)
            {
              tmp = p[i] * (p[j] - (i==j ? 1 : 0));
              HES(PARAM_B*Ncat+i, (PARAM_A_FIRST+x)*Ncat+j) -= tmp * theta_1;
              HES((PARAM_A_FIRST+x)*Ncat+i, PARAM_B*Ncat+j) -= tmp * theta_1;
              HES((PARAM_A_FIRST+x)*Ncat+i, (PARAM_A_FIRST+x)*Ncat+j) +=
                tmp * theta_1 * theta_1;
              for (y=x+1; y < model->Ndims; y++)
              {
                theta_2 = gsl_vector_get(theta->v, model->dims[y]);
                HES((PARAM_A_FIRST+x)*Ncat+i, (PARAM_A_FIRST+y)*Ncat+j) +=
                  tmp * theta_1 * theta_2;
                HES((PARAM_A_FIRST+y)*Ncat+i, (PARAM_A_FIRST+x)*Ncat+j) +=
                  tmp * theta_1 * theta_2;
              }
            }
            for (y=first_covariate; y < model->Np; y++)
            {
              gdouble val = oscats_covariates_get(covariates, model->names[y]);
              tmp = theta_1 * val * p0 * p[i];
              HES((PARAM_A_FIRST+x)*Ncat+i, y) -= tmp;
              HES(y, (PARAM_A_FIRST+x)*Ncat+i) -= tmp;
            }
          }
      }
  }

  // Covariates
  for (y=first_covariate; y < model->Np; y++)
  {
    theta_1 = oscats_covariates_get(covariates, model->names[y]);
    if (grad_v)
      grad_v->data[y*grad_v->stride] +=
        theta_1 * (p0 - (resp==0 ? 1 : 0));
    if (hes_v)
    {
      for (i=0; i < Ncat; i++)
      {
        tmp = theta_1 * p0 * p[i];
        HES(PARAM_B*Ncat+i, y) += tmp;
        HES(y, PARAM_B*Ncat+i) += tmp;
      }
      tmp = p0 * (p0 - 1);
      HES(y, y) += theta_1 * theta_1 * tmp;
      for (x=y+1; x < model->Np; x++)
      {
        theta_2 = oscats_covariates_get(covariates, model->names[x]);
        HES(x, y) += theta_1 * theta_2 * tmp;
        HES(y, x) += theta_1 * theta_2 * tmp;
      }
    }
  }
}
