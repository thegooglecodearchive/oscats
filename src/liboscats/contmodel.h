/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Abstract Continous IRT Model Class
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

#ifndef _LIBOSCATS_CONTMODEL_H_
#define _LIBOSCATS_CONTMODEL_H_
#include <glib.h>
#include <gsl.h>
#include <bitarray.h>
#include <covariates.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_CONT_MODEL		(oscats_cont_model_get_type())
#define OSCATS_CONT_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_CONT_MODEL, OscatsContModel))
#define OSCATS_IS_CONT_MODEL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_CONT_MODEL))
#define OSCATS_CONT_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_CONT_MODEL, OscatsContModelClass))
#define OSCATS_IS_CONT_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_CONT_MODEL))
#define OSCATS_CONT_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_CONT_MODEL, OscatsContModelClass))

typedef struct _OscatsContModel OscatsContModel;
typedef struct _OscatsContModelClass OscatsContModelClass;

struct _OscatsContModel {
  GObject parent_instance;
  guint testDim, Ndims;		// dimensions of test and this model
  guint Np, Ncov;		// number of parameters, covariates
  GBitArray *dimsFlags;
  gdouble *params;
  /*< private >*/
  GQuark *names, *covariates;
  guint dim1, dim2;		// short-cuts for one- and two-dim models
  guint *dims;
};

struct _OscatsContModelClass {
  GObjectClass parent_class;
  guint8 (*get_max) (const OscatsContModel *model);
  gdouble (*P) (const OscatsContModel *model, guint resp, const GGslVector *theta, const OscatsCovariates *covariates);
  gdouble (*distance) (const OscatsContModel *model, const GGslVector *theta, const OscatsCovariates *covariates);
  void (*logLik_dtheta) (const OscatsContModel *model,
                         guint resp, const GGslVector *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes, gboolean Information);
  void (*logLik_dparam) (const OscatsContModel *model,
                         guint resp, const GGslVector *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes);
};

GType oscats_cont_model_get_type();

guint8 oscats_cont_model_get_max(const OscatsContModel *model);
gdouble oscats_cont_model_P(const OscatsContModel *model, guint resp,
                           const GGslVector *theta, const OscatsCovariates *covariates);
gdouble oscats_cont_model_distance(const OscatsContModel *model,
                                  const GGslVector *theta,
                                  const OscatsCovariates *covariates);
void oscats_cont_model_logLik_dtheta(const OscatsContModel *model,
                                    guint resp, const GGslVector *theta,
                                    const OscatsCovariates *covariates,
                                    GGslVector *grad, GGslMatrix *hes);
void oscats_cont_model_logLik_dparam(const OscatsContModel *model,
                                    guint resp, const GGslVector *theta,
                                    const OscatsCovariates *covariates,
                                    GGslVector *grad, GGslMatrix *hes);
void oscats_cont_model_fisher_inf(const OscatsContModel *model,
                                 const GGslVector *theta,
                                 const OscatsCovariates *covariates,
                                 GGslMatrix *I);

const gchar* oscats_cont_model_get_param_name(const OscatsContModel *model,
                                             guint index);
gboolean oscats_cont_model_has_param_name(const OscatsContModel *model,
                                         const gchar *name);
gboolean oscats_cont_model_has_param(const OscatsContModel *model, GQuark name);
gdouble oscats_cont_model_get_param(const OscatsContModel *model, GQuark name);
gdouble oscats_cont_model_get_param_by_index(const OscatsContModel *model,
                                            guint index);
gdouble oscats_cont_model_get_param_by_name(const OscatsContModel *model,
                                           const gchar *name);
void oscats_cont_model_set_param(OscatsContModel *model, GQuark name,
                                gdouble value);
void oscats_cont_model_set_param_by_index(OscatsContModel *model, guint index,
                                gdouble value);
void oscats_cont_model_set_param_by_name(OscatsContModel *model,
                                        const gchar *name, gdouble value);

const gchar * oscats_cont_model_get_covariate_name(const OscatsContModel *model,
                                                  guint index);
gboolean oscats_cont_model_has_covariate(const OscatsContModel *model,
                                        GQuark name);
gboolean oscats_cont_model_has_covariate_name(const OscatsContModel *model,
                                             const gchar *name);
void oscats_cont_model_set_covariate_name(const OscatsContModel *model,
                                         guint index, GQuark name);
void oscats_cont_model_set_covariate_name_str(const OscatsContModel *model,
                                             guint index, const gchar *name);

G_END_DECLS
#endif
