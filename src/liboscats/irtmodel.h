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

#ifndef _LIBOSCATS_IRTMODEL_H_
#define _LIBOSCATS_IRTMODEL_H_
#include <glib.h>
#include <gsl.h>
#include <bitarray.h>
#include <covariates.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_IRT_MODEL		(oscats_irt_model_get_type())
#define OSCATS_IRT_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_IRT_MODEL, OscatsIrtModel))
#define OSCATS_IS_IRT_MODEL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_IRT_MODEL))
#define OSCATS_IRT_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_IRT_MODEL, OscatsIrtModelClass))
#define OSCATS_IS_IRT_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_IRT_MODEL))
#define OSCATS_IRT_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_IRT_MODEL, OscatsIrtModelClass))

typedef struct _OscatsIrtModel OscatsIrtModel;
typedef struct _OscatsIrtModelClass OscatsIrtModelClass;

struct _OscatsIrtModel {
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

struct _OscatsIrtModelClass {
  GObjectClass parent_class;
  guint8 (*get_max) (const OscatsIrtModel *model);
  gdouble (*P) (const OscatsIrtModel *model, guint resp, const GGslVector *theta, const OscatsCovariates *covariates);
  gdouble (*distance) (const OscatsIrtModel *model, const GGslVector *theta, const OscatsCovariates *covariates);
  void (*logLik_dtheta) (const OscatsIrtModel *model,
                         guint resp, const GGslVector *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes, gboolean Information);
  void (*logLik_dparam) (const OscatsIrtModel *model,
                         guint resp, const GGslVector *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes);
};

GType oscats_irt_model_get_type();

guint8 oscats_irt_model_get_max(const OscatsIrtModel *model);
gdouble oscats_irt_model_P(const OscatsIrtModel *model, guint resp,
                           const GGslVector *theta, const OscatsCovariates *covariates);
gdouble oscats_irt_model_distance(const OscatsIrtModel *model,
                                  const GGslVector *theta,
                                  const OscatsCovariates *covariates);
void oscats_irt_model_logLik_dtheta(const OscatsIrtModel *model,
                                    guint resp, const GGslVector *theta,
                                    const OscatsCovariates *covariates,
                                    GGslVector *grad, GGslMatrix *hes);
void oscats_irt_model_logLik_dparam(const OscatsIrtModel *model,
                                    guint resp, const GGslVector *theta,
                                    const OscatsCovariates *covariates,
                                    GGslVector *grad, GGslMatrix *hes);
void oscats_irt_model_fisher_inf(const OscatsIrtModel *model,
                                 const GGslVector *theta,
                                 const OscatsCovariates *covariates,
                                 GGslMatrix *I);

const gchar* oscats_irt_model_get_param_name(const OscatsIrtModel *model,
                                             guint index);
gboolean oscats_irt_model_has_param_name(const OscatsIrtModel *model,
                                         const gchar *name);
gboolean oscats_irt_model_has_param(const OscatsIrtModel *model, GQuark name);
gdouble oscats_irt_model_get_param(const OscatsIrtModel *model, GQuark name);
gdouble oscats_irt_model_get_param_by_index(const OscatsIrtModel *model,
                                            guint index);
gdouble oscats_irt_model_get_param_by_name(const OscatsIrtModel *model,
                                           const gchar *name);
void oscats_irt_model_set_param(OscatsIrtModel *model, GQuark name,
                                gdouble value);
void oscats_irt_model_set_param_by_index(OscatsIrtModel *model, guint index,
                                gdouble value);
void oscats_irt_model_set_param_by_name(OscatsIrtModel *model,
                                        const gchar *name, gdouble value);

const gchar * oscats_irt_model_get_covariate_name(const OscatsIrtModel *model,
                                                  guint index);
gboolean oscats_irt_model_has_covariate(const OscatsIrtModel *model,
                                        GQuark name);
gboolean oscats_irt_model_has_covariate_name(const OscatsIrtModel *model,
                                             const gchar *name);
void oscats_irt_model_set_covariate_name(const OscatsIrtModel *model,
                                         guint index, GQuark name);
void oscats_irt_model_set_covariate_name_str(const OscatsIrtModel *model,
                                             guint index, const gchar *name);

G_END_DECLS
#endif
