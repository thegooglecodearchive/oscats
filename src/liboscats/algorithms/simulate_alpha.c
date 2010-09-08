/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Simulate Item Administration with underlying Classification model
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
#include "algorithms/simulate_alpha.h"

G_DEFINE_TYPE(OscatsAlgSimulateAlpha, oscats_alg_simulate_alpha, OSCATS_TYPE_ALGORITHM);

enum
{
  PROP_0,
  PROP_AUTO_RECORD,
};

static void oscats_alg_set_property(GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec);
static void oscats_alg_get_property(GObject *object, guint prop_id,
                                    GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);
static void oscats_alg_simulate_alpha_class_init (OscatsAlgSimulateAlphaClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->set_property = oscats_alg_set_property;
  gobject_class->get_property = oscats_alg_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgSimulateAlpha:auto-record:
 *
 * Whether to record all simulated responses.
 */
  pspec = g_param_spec_boolean("auto-record", "Auto-record", 
                               "Record all simulated responses",
                               TRUE,
                               G_PARAM_READWRITE |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_AUTO_RECORD, pspec);

}

static void oscats_alg_simulate_alpha_init (OscatsAlgSimulateAlpha *self)
{
}

static void oscats_alg_set_property(GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec)
{
  OscatsAlgSimulateAlpha *self = OSCATS_ALG_SIMULATE_ALPHA(object);
  switch (prop_id)
  {
    case PROP_AUTO_RECORD:
      self->record = g_value_get_boolean(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_get_property(GObject *object, guint prop_id,
                                    GValue *value, GParamSpec *pspec)
{
  OscatsAlgSimulateAlpha *self = OSCATS_ALG_SIMULATE_ALPHA(object);
  switch (prop_id)
  {
    case PROP_AUTO_RECORD:
      g_value_set_boolean(value, self->record);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static guint administer (OscatsTest *test, OscatsExaminee *e,
                         OscatsItem *item, gpointer alg_data)
{
  guint resp, max = oscats_discr_model_get_max(item->discr_model);
  gdouble p, rnd = oscats_rnd_uniform();
  for (resp=0; resp <= max; resp++)
    if (rnd < (p=oscats_discr_model_P(item->discr_model, resp,
                                      e->true_alpha)))
    {
      if (OSCATS_ALG_SIMULATE_ALPHA(alg_data)->record)
        oscats_examinee_add_item(e, item, resp);
      return resp;
    }
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
  g_return_if_fail(oscats_item_bank_is_discr(test->itembank));
  g_signal_connect_data(test, "administer", G_CALLBACK(administer),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
