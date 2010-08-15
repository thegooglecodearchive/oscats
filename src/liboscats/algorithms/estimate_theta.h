/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Estimate latent IRT ability
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

#ifndef _LIBOSCATS_ALGORITHM_ESTIMATE_THETA_H_
#define _LIBOSCATS_ALGORITHM_ESTIMATE_THETA_H_
#include <glib-object.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_ESTIMATE_THETA	(oscats_alg_estimate_theta_get_type())
#define OSCATS_ALG_ESTIMATE_THETA(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_ESTIMATE_THETA, OscatsAlgEstimateTheta))
#define OSCATS_IS_ALG_ESTIMATE_THETA(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_ESTIMATE_THETA))
#define OSCATS_ALG_ESTIMATE_THETA_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_ESTIMATE_THETA, OscatsAlgEstimateThetaClass))
#define OSCATS_IS_ALG_ESTIMATE_THETA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_ESTIMATE_THETA))
#define OSCATS_ALG_ESTIMATE_THETA_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_ESTIMATE_THETA, OscatsAlgEstimateThetaClass))

typedef struct _OscatsAlgEstimateTheta OscatsAlgEstimateTheta;
typedef struct _OscatsAlgEstimateThetaClass OscatsAlgEstimateThetaClass;

/**
 * OscatsAlgEstimateTheta
 *
 * Statistics algorithm (#OscatsTest::administered).
 * Update the examinee's latent IRT ability estimate.
 */
struct _OscatsAlgEstimateTheta {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  gint flag;			// indicates must use eap
  GGslVector *mu;		// population parameters for eap
  GGslMatrix *Sigma;
};

struct _OscatsAlgEstimateThetaClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_estimate_theta_get_type();

gboolean oscats_estimate_theta_mle(const GPtrArray *items, const GByteArray *resp,
                                   const OscatsCovariates *covariates,
                                   GGslVector *theta, GGslMatrix *err);
void oscats_estimate_theta_eap(const GPtrArray *items, const GByteArray *resp,
                               const OscatsCovariates *covariates,
                               GGslVector *theta, GGslMatrix *err,
                               const GGslVector *mu, const GGslMatrix *Sigma);

void oscats_estimate_theta_mle_set_tol(gdouble tol);
void oscats_estimate_theta_eap_set_eps(gdouble eps);

G_END_DECLS
#endif
