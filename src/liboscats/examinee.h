/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Examinee Class
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

#ifndef _LIBOSCATS_EXAMINEE_H_
#define _LIBOSCATS_EXAMINEE_H_
#include <glib.h>
#include "item.h"
#include "covariates.h"
G_BEGIN_DECLS

#define OSCATS_TYPE_EXAMINEE		(oscats_examinee_get_type())
#define OSCATS_EXAMINEE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_EXAMINEE, OscatsExaminee))
#define OSCATS_IS_EXAMINEE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_EXAMINEE))
#define OSCATS_EXAMINEE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_EXAMINEE, OscatsExamineeClass))
#define OSCATS_IS_EXAMINEE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_EXAMINEE))
#define OSCATS_EXAMINEE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_EXAMINEE, OscatsExamineeClass))

typedef struct _OscatsExaminee OscatsExaminee;
typedef struct _OscatsExamineeClass OscatsExamineeClass;

struct _OscatsExaminee {
  GObject parent_instance;
  gchar *id;
  GGslVector *true_theta, *theta_hat;
  GGslMatrix *theta_err;
  OscatsAttributes *true_alpha, *alpha_hat;
  OscatsCovariates *covariates;
  GPtrArray *items;
  GByteArray *resp;
};

struct _OscatsExamineeClass {
  GObjectClass parent_class;
};

GType oscats_examinee_get_type();

void oscats_examinee_set_true_theta(OscatsExaminee *e, const GGslVector *t);
GGslVector * oscats_examinee_get_true_theta(OscatsExaminee *e);
void oscats_examinee_set_theta_hat(OscatsExaminee *e, const GGslVector *t);
GGslVector * oscats_examinee_get_theta_hat(OscatsExaminee *e);
void oscats_examinee_init_theta_err(OscatsExaminee *e, guint dim);
GGslMatrix * oscats_examinee_get_theta_err(OscatsExaminee *e);

void oscats_examinee_set_true_alpha(OscatsExaminee *e,
                                    const OscatsAttributes *attr);
OscatsAttributes * oscats_examinee_get_true_alpha(OscatsExaminee *e);
void oscats_examinee_set_alpha_hat(OscatsExaminee *e,
                                   const OscatsAttributes *attr);
OscatsAttributes * oscats_examinee_get_alpha_hat(OscatsExaminee *e);

void oscats_examinee_prep(OscatsExaminee *e, guint length_hint);
void oscats_examinee_add_item(OscatsExaminee *e, OscatsItem *item, guint8 resp);
guint oscats_examinee_num_items(const OscatsExaminee *e);

G_END_DECLS
#endif
