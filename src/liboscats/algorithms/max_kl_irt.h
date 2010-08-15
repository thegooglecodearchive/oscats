/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Select Item based on Kullback-Leibler Divergence (IRT)
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

#ifndef _LIBOSCATS_ALGORITHM_MAX_KL_IRT_H_
#define _LIBOSCATS_ALGORITHM_MAX_KL_IRT_H_
#include <glib-object.h>
#include <algorithm.h>
#include <algorithms/chooser.h>
#include <integrate.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_MAX_KL_IRT	(oscats_alg_max_kl_irt_get_type())
#define OSCATS_ALG_MAX_KL_IRT(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_MAX_KL_IRT, OscatsAlgMaxKlIrt))
#define OSCATS_IS_ALG_MAX_KL_IRT(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_MAX_KL_IRT))
#define OSCATS_ALG_MAX_KL_IRT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_MAX_KL_IRT, OscatsAlgMaxKlIrtClass))
#define OSCATS_IS_ALG_MAX_KL_IRT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_MAX_KL_IRT))
#define OSCATS_ALG_MAX_KL_IRT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_MAX_KL_IRT, OscatsAlgMaxKlIrtClass))

typedef struct _OscatsAlgMaxKlIrt OscatsAlgMaxKlIrt;
typedef struct _OscatsAlgMaxKlIrtClass OscatsAlgMaxKlIrtClass;

/**
 * OscatsAlgMaxKlIrt:
 *
 * Item selection algorithm (#OscatsTest::select).
 * Picks the item with greatest Kullback-Leibler index.
 * Note: This algorithm may not work correctly if there are
 * multiple items with exactly the same optimality metric.
 *
 * Kullback-Leibler Divergence is:
 * KL(theta.hat || theta) = E_{X|theta.hat}[log{P(X|theta.hat)/P(X|theta)}]
 *
 * The KL Index is:
 * KLI(theta.hat) = Int KL(theta.hat||theta) dtheta
 *
 * Integration is either over the box theta.hat +/- c/sqrt(n) or
 * over the ellipsoid (x-theta.hat)' I_n(theta.hat)^-1 (x-theta.hat) <= c,
 * where n is the number of items already administered and I_n(theta.hat) is
 * the Fisher Information for the previously administered items.
 *
 * Alternatively, the KL Index may be posteriorly weighted:
 * PWKLI(theta.hat) = Int_{R^p} KL(theta.hat||theta) { prod_i P_i(x_i|theta) } g(theta) dtheta
 * where g(theta) is the prior for theta (Normal).
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Hua-Hua</firstname> <surname>Chang</surname></personname></author> and
 *    <author><personname><firstname>Zhiliang</firstname> <surname>Ying</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1996</pubdate>).
 *    "<title>A Global Information Appraoch to Computerized Adaptive Testing</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>20</volumenum>,</biblioset>
 *    <artpagenums>213-229</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Bernard</firstname> <surname>Veldkamp</surname></personname></author> and
 *    <author><personname><firstname>Wim</firstname> <surname>van der Linden</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2002</pubdate>).
 *    "<title>Multidimensional Adaptive Testing with Constraints on Test Content</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>67</volumenum>,</biblioset>
 *    <artpagenums>575-588</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>T.M.</firstname> <surname>Cover</surname></personname></author> and
 *    <author><personname><firstname>J.A.</firstname> <surname>Thomas</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1991</pubdate>).
 *    <title>Elements of Information Theory</title>.
 *    p <pagenums>18</pagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsAlgMaxKlIrt {
  OscatsAlgorithm parent_instance;
  gdouble c;
  /*< private >*/
  OscatsAlgChooser *chooser;
  gboolean inf_bounds, posterior;
  guint base_num, dims, max;
  GGslMatrix *Inf;
  OscatsIntegrate *integrator;
  gsl_vector *mu;
  gsl_matrix *Sigma_half;
  // Integration working space
  OscatsExaminee *e;
  OscatsIrtModel *model;
  gdouble p_sum, *p;
  gsl_vector *tmp, *tmp2;	// for posterior
  GGslMatrix *Inf_inv;		// for ellipse
};

struct _OscatsAlgMaxKlIrtClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_max_kl_irt_get_type();

G_END_DECLS
#endif
