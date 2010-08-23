/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Two-Parameter Logistic IRT Model
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

#ifndef _LIBOSCATS_MODEL_L2P_H_
#define _LIBOSCATS_MODEL_L2P_H_
#include <glib.h>
#include <contmodel.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_CONT_MODEL_L2P		(oscats_cont_model_l2p_get_type())
#define OSCATS_CONT_MODEL_L2P(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_CONT_MODEL_L2P, OscatsContModelL2p))
#define OSCATS_IS_CONT_MODEL_L2P(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_CONT_MODEL_L2P))
#define OSCATS_CONT_MODEL_L2P_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_CONT_MODEL_L2P, OscatsContModelL2pClass))
#define OSCATS_IS_CONT_MODEL_L2P_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_CONT_MODEL_L2P))
#define OSCATS_CONT_MODEL_L2P_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_CONT_MODEL_L2P, OscatsContModelL2pClass))

typedef struct _OscatsContModelL2p OscatsContModelL2p;
typedef struct _OscatsContModelL2pClass OscatsContModelL2pClass;

/**
 * OscatsContModelL2p:
 *
 * The Two-Paramter Logistic IRT model:
 * P(X=1|theta) = 1/1+exp[-(sum_i a_i theta_i -b + sum_j d_j covariate_j)],
 * where a_i are the item discrimination for each dimension 
 * and b is the scaled item difficulty.  Note that this differs from the
 * usual IRT parameterization 1/1+exp[-a(theta-b)].
 * Distance is defined as: abs(sum_i a_i theta_i - b + sum_j d_j covariate_j).
 *
 * Parameter names: Diff, Discr.i
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    <title>The Theory and Practice of Item Response Theory</title>.
 *    p <pagenums>100, 277</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Mark</firstname> <othername>D.</othername> <surname>Reckase</surname></personname></author>
 *    (<pubdate>1985</pubdate>).
 *    "<title>The Difficulty of Test Items That Measure More Than One Ability</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>9</volumenum>,</biblioset>
 *    <artpagenums>401-412</artpagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsContModelL2p {
  OscatsContModel parent_instance;
};

struct _OscatsContModelL2pClass {
  OscatsContModelClass parent_class;
};

GType oscats_cont_model_l2p_get_type();

G_END_DECLS
#endif
