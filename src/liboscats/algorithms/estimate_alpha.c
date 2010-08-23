/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Estimate latent classification
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

#include <math.h>
#include "gsl.h"
#include "algorithm.h"
#include "algorithms/estimate_alpha.h"

G_DEFINE_TYPE(OscatsAlgEstimateAlpha, oscats_alg_estimate_alpha, OSCATS_TYPE_ALGORITHM);

static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_estimate_alpha_class_init (OscatsAlgEstimateAlphaClass *klass)
{
//  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  
  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

}

static void oscats_alg_estimate_alpha_init (OscatsAlgEstimateAlpha *self)
{
}

static void administered (OscatsTest *test, OscatsExaminee *e,
                          OscatsItem *item, guint resp, gpointer alg_data)
{
  OscatsAttributes *attr = oscats_examinee_get_alpha_hat(e);
  gdouble L, L_max;
  _AttributesType K, max=0;
  guint i;
  
  if (!attr)
  {
    oscats_examinee_set_alpha_hat(e,
      attr = g_object_new(OSCATS_TYPE_ATTRIBUTES,
                          "num", oscats_item_bank_num_attrs(test->itembank),
                          NULL));
    g_object_unref(attr);
    attr = oscats_examinee_get_alpha_hat(e);
  }

  K = (1 << attr->num);
  attr->data = 0;
  for (L_max=0,i=0; i < e->items->len; i++)
  {
    OscatsItem *itm = g_ptr_array_index(e->items, i);
    L_max += log(oscats_discr_model_P(itm->discr_model,
                                      e->resp->data[i], attr));
  }
  for (attr->data++; attr->data < K; attr->data++)
  {
    for (L=0,i=0; i < e->items->len; i++)
    {
      OscatsItem *itm = g_ptr_array_index(e->items, i);
      L += log(oscats_discr_model_P(itm->discr_model,
                                    e->resp->data[i], attr));
    }
    if (L > L_max) { max = attr->data;  L_max = L; }
  }

  attr->data = max;
}

/*
 * Note that unless someone does something naughty, alg_data will be of the
 * appropriate type, and test will be an OscatsTest.  The signal connections
 * should include oscats_algorithm_closure_finalize as the destruction
 * callback.  The first connection should take alg_data's reference.  Any
 * subsequent connections should be accompanied by g_object_ref(alg_data).
 */
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
//  OscatsAlgEstimateAlpha *self = OSCATS_ALG_ESTIMATE_ALPHA(alg_data);
  g_return_if_fail(oscats_item_bank_is_discr(test->itembank));

  g_signal_connect_data(test, "administered", G_CALLBACK(administered),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}

