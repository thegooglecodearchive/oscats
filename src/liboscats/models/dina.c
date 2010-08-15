/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Deterministic Inputs Noisy And Gate (DINA) Classification Model
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
 * SECTION:dina
 * @title:OscatsClassModelDina
 * @short_description: Deterministic Inputs Noisy And Gate (DINA) Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/dina.h"

G_DEFINE_TYPE(OscatsClassModelDina, oscats_class_model_dina, OSCATS_TYPE_CLASS_MODEL);

enum
{
  PARAM_GUESS,
  PARAM_SLIP,
  NUM_PARAMS,
};

static void model_constructed (GObject *object);
static guint8 get_max (const OscatsClassModel *model);
static gdouble P(const OscatsClassModel *model, guint resp, const OscatsAttributes *attr);
static void logLik_dparam(const OscatsClassModel *model,
                          guint resp, const OscatsAttributes *attr,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_class_model_dina_class_init (OscatsClassModelDinaClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsClassModelClass *model_class = OSCATS_CLASS_MODEL_CLASS(klass);

  gobject_class->constructed = model_constructed;

  model_class->get_max = get_max;
  model_class->P = P;
  model_class->logLik_dparam = logLik_dparam;
  
}

static void oscats_class_model_dina_init (OscatsClassModelDina *self)
{
}

static void model_constructed(GObject *object)
{
  OscatsClassModel *model = OSCATS_CLASS_MODEL(object);
  G_OBJECT_CLASS(oscats_class_model_dina_parent_class)->constructed(object);

  model->Np = NUM_PARAMS;
  model->params = g_new0(gdouble, model->Np);

  model->names = g_new(GQuark, model->Np);
  model->names[PARAM_GUESS] = g_quark_from_string("Guess");
  model->names[PARAM_SLIP] = g_quark_from_string("Slip");
  
}

static guint8 get_max (const OscatsClassModel *model)
{
  return 1;
}

static gdouble P(const OscatsClassModel *model, guint resp,
                 const OscatsAttributes *attr)
{
  gboolean pass = TRUE;
  gdouble p;
  guint i;
  g_return_val_if_fail(resp == 0 || resp == 1, 0);
  for (i=0; i < model->Ndims && pass; i++)
    if (!oscats_attributes_get(attr, model->dims[i])) pass = FALSE;
  p = (pass ? 1-model->params[PARAM_SLIP] : model->params[PARAM_GUESS]);
  return (resp ? p : 1-p);
}

/* Derivative    pass && resp  pass && !resp  !pass && resp  !pass && !resp
 *   g             0             0              1/g            -1/g
 *   s             1/(s-1)       1/s            0              0
 *   g, g          0             0              -1/g^2         1/g^2
 *   s, s          -1/(s-1)^2    -1/s^2         0              0
 *   g, s          0             0              0              0
 */
static void logLik_dparam(const OscatsClassModel *model,
                          guint resp, const OscatsAttributes *attr,
                          GGslVector *grad, GGslMatrix *hes)
{
  gboolean pass = TRUE;
  gdouble tmp;
  guint i;
  g_return_if_fail(resp == 0 || resp == 1);
  for (i=0; i < model->Ndims && pass; i++)
    if (!oscats_attributes_get(attr, i)) pass = FALSE;
  if (pass)
  {
    if (resp)
    {
      tmp = model->params[PARAM_SLIP] - 1;
      grad->v->data[PARAM_SLIP*grad->v->stride] += 1/tmp;
      hes->v->data[PARAM_SLIP*hes->v->tda + PARAM_SLIP] -= 1/(tmp*tmp);
    } else { // !resp
      tmp = model->params[PARAM_SLIP];
      grad->v->data[PARAM_SLIP*grad->v->stride] += 1/tmp;
      hes->v->data[PARAM_SLIP*hes->v->tda + PARAM_SLIP] -= 1/(tmp*tmp);
    }
  } else { // !pass
    if (resp)
    {
      tmp = model->params[PARAM_GUESS];
      grad->v->data[PARAM_GUESS*grad->v->stride] += 1/tmp;
      hes->v->data[PARAM_GUESS*hes->v->tda + PARAM_GUESS] -= 1/(tmp*tmp);
    } else { // !resp
      tmp = model->params[PARAM_GUESS];
      grad->v->data[PARAM_GUESS*grad->v->stride] -= 1/tmp;
      hes->v->data[PARAM_GUESS*hes->v->tda + PARAM_GUESS] += 1/(tmp*tmp);
    }
  }
}

