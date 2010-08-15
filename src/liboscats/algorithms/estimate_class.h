/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Estimate latent class
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

#ifndef _LIBOSCATS_ALGORITHM_ESTIMATE_CLASS_H_
#define _LIBOSCATS_ALGORITHM_ESTIMATE_CLASS_H_
#include <glib-object.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_ESTIMATE_CLASS	(oscats_alg_estimate_class_get_type())
#define OSCATS_ALG_ESTIMATE_CLASS(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_ESTIMATE_CLASS, OscatsAlgEstimateClass))
#define OSCATS_IS_ALG_ESTIMATE_CLASS(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_ESTIMATE_CLASS))
#define OSCATS_ALG_ESTIMATE_CLASS_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_ESTIMATE_CLASS, OscatsAlgEstimateClassClass))
#define OSCATS_IS_ALG_ESTIMATE_CLASS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_ESTIMATE_CLASS))
#define OSCATS_ALG_ESTIMATE_CLASS_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_ESTIMATE_CLASS, OscatsAlgEstimateClassClass))

typedef struct _OscatsAlgEstimateClass OscatsAlgEstimateClass;
typedef struct _OscatsAlgEstimateClassClass OscatsAlgEstimateClassClass;

/**
 * OscatsAlgEstimateClass
 *
 * Statistics algorithm (#OscatsTest::administered).
 * Update the examinee's latent classification estimate.
 *
 * The estimation is via maximum likelihood, and the algorithm
 * iterates through all 2^K classes, where K is the number of attributes
 * in the test.  Consequently, this algorithm may be very slow for large
 * numbers of attributes.
 */
struct _OscatsAlgEstimateClass {
  OscatsAlgorithm parent_instance;
};

struct _OscatsAlgEstimateClassClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_estimate_class_get_type();

G_END_DECLS
#endif
