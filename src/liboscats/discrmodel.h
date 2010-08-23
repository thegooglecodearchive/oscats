/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Discrete IRT Model (Classification) Class
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

#ifndef _LIBOSCATS_DISCRMODEL_H_
#define _LIBOSCATS_DISCRMODEL_H_
#include <glib.h>
#include <gsl.h>
#include <bitarray.h>
#include <covariates.h>
#include <attributes.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_DISCR_MODEL		(oscats_discr_model_get_type())
#define OSCATS_DISCR_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_DISCR_MODEL, OscatsDiscrModel))
#define OSCATS_IS_DISCR_MODEL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_DISCR_MODEL))
#define OSCATS_DISCR_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_DISCR_MODEL, OscatsDiscrModelClass))
#define OSCATS_IS_DISCR_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_DISCR_MODEL))
#define OSCATS_DISCR_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_DISCR_MODEL, OscatsDiscrModelClass))

typedef struct _OscatsDiscrModel OscatsDiscrModel;
typedef struct _OscatsDiscrModelClass OscatsDiscrModelClass;

struct _OscatsDiscrModel {
  GObject parent_instance;
  guint Ndims, Np;		// number of attr and params for this model
  OscatsAttributes *dimsFlags;
  gdouble *params;
  /*< private >*/
  GQuark *names;
  guint8 *dims;
};

struct _OscatsDiscrModelClass {
  GObjectClass parent_class;
  guint8 (*get_max) (const OscatsDiscrModel *model);
  gdouble (*P) (const OscatsDiscrModel *model, guint resp, const OscatsAttributes *attr);
  void (*logLik_dparam) (const OscatsDiscrModel *model,
                         guint resp, const OscatsAttributes *attr,
                         GGslVector *grad, GGslMatrix *hes);
};

GType oscats_discr_model_get_type();

guint8 oscats_discr_model_get_max(const OscatsDiscrModel *model);
gdouble oscats_discr_model_P(const OscatsDiscrModel *model, guint resp,
                             const OscatsAttributes *attr);
void oscats_discr_model_logLik_dparam(const OscatsDiscrModel *model,
                                      guint resp, const OscatsAttributes *attr,
                                      GGslVector *grad, GGslMatrix *hes);

const gchar* oscats_discr_model_get_param_name(const OscatsDiscrModel *model,
                                               guint index);
gboolean oscats_discr_model_has_param_name(const OscatsDiscrModel *model,
                                           const gchar *name);
gboolean oscats_discr_model_has_param(const OscatsDiscrModel *model, GQuark name);
gdouble oscats_discr_model_get_param(const OscatsDiscrModel *model, GQuark name);
gdouble oscats_discr_model_get_param_by_index(const OscatsDiscrModel *model,
                                              guint index);
gdouble oscats_discr_model_get_param_by_name(const OscatsDiscrModel *model,
                                             const gchar *name);
void oscats_discr_model_set_param(OscatsDiscrModel *model, GQuark name,
                                  gdouble value);
void oscats_discr_model_set_param_by_index(OscatsDiscrModel *model, guint index,
                                  gdouble value);
void oscats_discr_model_set_param_by_name(OscatsDiscrModel *model,
                                          const gchar *name, gdouble value);

G_END_DECLS
#endif
