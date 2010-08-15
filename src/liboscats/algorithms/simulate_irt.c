/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * CAT Algorithm: Simulate Item Administration with underlying IRT model
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

#include "random.h"
#include "algorithm.h"
#include "algorithms/simulate_irt.h"

G_DEFINE_TYPE(OscatsAlgSimulateIrt, oscats_alg_simulate_irt, OSCATS_TYPE_ALGORITHM);

static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_simulate_irt_class_init (OscatsAlgSimulateIrtClass *klass)
{
//  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;
}

static void oscats_alg_simulate_irt_init (OscatsAlgSimulateIrt *self)
{
}

static guint administer (OscatsTest *test, OscatsExaminee *e,
                         OscatsItem *item, gpointer alg_data)
{
  guint resp, max = oscats_irt_model_get_max(item->irt_model);
  gdouble p, rnd = oscats_rnd_uniform();
  for (resp=0; resp <= max; resp++)
    if (rnd < (p=oscats_irt_model_P(item->irt_model, resp,
                                    e->true_theta, e->covariates)))
      return resp;
    else
      rnd -= p;
  g_warn_if_reached();		// Model probabilities don't sum to 1.
  return 0;
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
  g_return_if_fail(oscats_item_bank_is_irt(test->itembank));
  g_signal_connect_data(test, "administer", G_CALLBACK(administer),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
                   
