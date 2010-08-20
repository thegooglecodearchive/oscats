/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Noisy Inputs Deterministic And Gate (NIDA) Classification Model
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

#ifndef _LIBOSCATS_MODEL_NIDA_H_
#define _LIBOSCATS_MODEL_NIDA_H_
#include <glib.h>
#include <discrmodel.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_DISCR_MODEL_NIDA		(oscats_discr_model_nida_get_type())
#define OSCATS_DISCR_MODEL_NIDA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_DISCR_MODEL_NIDA, OscatsDiscrModelNida))
#define OSCATS_IS_DISCR_MODEL_NIDA(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_DISCR_MODEL_NIDA))
#define OSCATS_DISCR_MODEL_NIDA_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_DISCR_MODEL_NIDA, OscatsDiscrModelNidaClass))
#define OSCATS_IS_DISCR_MODEL_NIDA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_DISCR_MODEL_NIDA))
#define OSCATS_DISCR_MODEL_NIDA_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_DISCR_MODEL_NIDA, OscatsDiscrModelNidaClass))

typedef struct _OscatsDiscrModelNida OscatsDiscrModelNida;
typedef struct _OscatsDiscrModelNidaClass OscatsDiscrModelNidaClass;

/**
 * OscatsDiscrModelNida:
 *
 * Noisy Inputs Deterministic And Gate (NIDA) Classification model:
 * P(X=1|alpha) = prod_i (1-s_i)^(alpha_i) g_i^(1-prod_i alpha_i)
 * where s_i and g_i are the slipping guessing parameters for each attribute.
 *
 * Parameter names: Guess.i, Slip.i.
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Brian</firstname> <surname>Junker</surname></personname></author> and
 *    <author><personname><firstname>Klaas</firstname> <surname>Sijtsma</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2001</pubdate>).
 *    "<title>Cognitive Assessment Models with Few Assumptions, and Connections with Nonparametric Item Response Theory</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>25</volumenum>,</biblioset>
 *    <artpagenums>258-272</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>E.</firstname> <surname>Maris</surname></personname></author>
 *    (<pubdate>1999</pubdate>).
 *    "<title>Estimating Multiple Classification Latent Class Models</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>64</volumenum>,</biblioset>
 *    <artpagenums>187-212</artpagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsDiscrModelNida {
  OscatsDiscrModel parent_instance;
};

struct _OscatsDiscrModelNidaClass {
  OscatsDiscrModelClass parent_class;
};

GType oscats_discr_model_nida_get_type();

G_END_DECLS
#endif
