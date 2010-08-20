/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Select Item based on Kullback-Leibler Divergence (Classification)
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

#ifndef _LIBOSCATS_ALGORITHM_MAX_KL_DISCR_H_
#define _LIBOSCATS_ALGORITHM_MAX_KL_DISCR_H_
#include <glib-object.h>
#include <algorithm.h>
#include <algorithms/chooser.h>
#include <integrate.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_MAX_KL_DISCR	(oscats_alg_max_kl_discr_get_type())
#define OSCATS_ALG_MAX_KL_DISCR(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_MAX_KL_DISCR, OscatsAlgMaxKlDiscr))
#define OSCATS_IS_ALG_MAX_KL_DISCR(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_MAX_KL_DISCR))
#define OSCATS_ALG_MAX_KL_DISCR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_MAX_KL_DISCR, OscatsAlgMaxKlDiscrClass))
#define OSCATS_IS_ALG_MAX_KL_DISCR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_MAX_KL_DISCR))
#define OSCATS_ALG_MAX_KL_DISCR_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_MAX_KL_DISCR, OscatsAlgMaxKlDiscrClass))

typedef struct _OscatsAlgMaxKlDiscr OscatsAlgMaxKlDiscr;
typedef struct _OscatsAlgMaxKlDiscrClass OscatsAlgMaxKlDiscrClass;

/**
 * OscatsAlgMaxKlDiscr:
 *
 * Item selection algorithm (#OscatsTest::select).
 * Picks the item with greatest Kullback-Leibler index.
 * Note: This algorithm may not work correctly if there are
 * multiple items with exactly the same optimality metric.
 *
 * Kullback-Leibler Divergence is:
 * KL(alpha.hat || alpha) = E_{X|theta.hat}[log{P(X|alpha.hat)/P(X|alpha)}]
 *
 * The KL Index is:
 * KLI(alpha.hat) = sum_alpha KL(alpha.hat||alpha)
 *
 * Note that the sum is over all 2^K attribute patterns, for K attributes. 
 * Consequently, this algorithm may be very slow for large numbers of
 * attributes.
 *
 * Optionally, the KL Index may be posteriorly weighted:
 * PWKLI(alpha.hat) = sum_alpha KL(alpha.hat||alpha) { prod_i P_i(x_i|alpha) } g(alpha)
 * where g(theta) is a prior specified for alpha.
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>Ying</firstname> <surname>Cheng</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    "<title>When Cognitive Diagnosis Meets Computerized Adaptive Testing: CD-CAT</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>74</volumenum>,</biblioset>
 *    <artpagenums>619-632</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Xueli</firstname> <surname>Xu</surname></personname></author>,
 *    <author><personname><firstname>Hua-Hua</firstname> <surname>Chang</surname></personname></author>, and
 *    <author><personname><firstname>Jeff</firstname> <surname>Douglas</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2003</pubdate>).
 *    "<title>A Simulation Study to Compare CAT Strategies for Cognitive Diagnosis</title>."
 *    Paper presented at the annual meeting of the National Council on Measurement in Education, Montreal, Canada.
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
struct _OscatsAlgMaxKlDiscr {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  gboolean posterior;
  OscatsAlgChooser *chooser;
  guint numAttrs;
  GGslVector *prior;
  // Working space
  OscatsAttributes *attr;
};

struct _OscatsAlgMaxKlDiscrClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_max_kl_discr_get_type();

G_END_DECLS
#endif
