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

#ifndef _LIBOSCATS_CLASSMODEL_H_
#define _LIBOSCATS_CLASSMODEL_H_
#include <glib.h>
#include <gsl.h>
#include <bitarray.h>
#include <covariates.h>
#include <attributes.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_CLASS_MODEL		(oscats_class_model_get_type())
#define OSCATS_CLASS_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_CLASS_MODEL, OscatsClassModel))
#define OSCATS_IS_CLASS_MODEL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_CLASS_MODEL))
#define OSCATS_CLASS_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_CLASS_MODEL, OscatsClassModelClass))
#define OSCATS_IS_CLASS_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_CLASS_MODEL))
#define OSCATS_CLASS_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_CLASS_MODEL, OscatsClassModelClass))

typedef struct _OscatsClassModel OscatsClassModel;
typedef struct _OscatsClassModelClass OscatsClassModelClass;

struct _OscatsClassModel {
  GObject parent_instance;
  guint Ndims, Np;		// number of attr and params for this model
  OscatsAttributes *dimsFlags;
  gdouble *params;
  /*< private >*/
  GQuark *names;
  guint8 *dims;
};

struct _OscatsClassModelClass {
  GObjectClass parent_class;
  guint8 (*get_max) (const OscatsClassModel *model);
  gdouble (*P) (const OscatsClassModel *model, guint resp, const OscatsAttributes *attr);
  void (*logLik_dparam) (const OscatsClassModel *model,
                         guint resp, const OscatsAttributes *attr,
                         GGslVector *grad, GGslMatrix *hes);
};

GType oscats_class_model_get_type();

guint8 oscats_class_model_get_max(const OscatsClassModel *model);
gdouble oscats_class_model_P(const OscatsClassModel *model, guint resp,
                             const OscatsAttributes *attr);
void oscats_class_model_logLik_dparam(const OscatsClassModel *model,
                                      guint resp, const OscatsAttributes *attr,
                                      GGslVector *grad, GGslMatrix *hes);

const gchar* oscats_class_model_get_param_name(const OscatsClassModel *model,
                                               guint index);
gboolean oscats_class_model_has_param_name(const OscatsClassModel *model,
                                           const gchar *name);
gboolean oscats_class_model_has_param(const OscatsClassModel *model, GQuark name);
gdouble oscats_class_model_get_param(const OscatsClassModel *model, GQuark name);
gdouble oscats_class_model_get_param_by_index(const OscatsClassModel *model,
                                              guint index);
gdouble oscats_class_model_get_param_by_name(const OscatsClassModel *model,
                                             const gchar *name);
void oscats_class_model_set_param(OscatsClassModel *model, GQuark name,
                                  gdouble value);
void oscats_class_model_set_param_by_index(OscatsClassModel *model, guint index,
                                  gdouble value);
void oscats_class_model_set_param_by_name(OscatsClassModel *model,
                                          const gchar *name, gdouble value);

G_END_DECLS
#endif
