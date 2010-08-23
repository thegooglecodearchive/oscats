/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Track classification rates
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

#include "algorithm.h"
#include "algorithms/class_rates.h"

enum
{
  PROP_0,
  PROP_BY_PATTERN,
};

G_DEFINE_TYPE(OscatsAlgClassRates, oscats_alg_class_rates, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_class_rates_dispose (GObject *object);
static void oscats_alg_class_rates_finalize (GObject *object);
static void oscats_alg_class_rates_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_class_rates_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_class_rates_class_init (OscatsAlgClassRatesClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_class_rates_dispose;
  gobject_class->finalize = oscats_alg_class_rates_finalize;
  gobject_class->set_property = oscats_alg_class_rates_set_property;
  gobject_class->get_property = oscats_alg_class_rates_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgClassRates:by-pattern:
 *
 * A string identifier for the examinee.
 */
  pspec = g_param_spec_boolean("by-pattern", "Rates by pattern", 
                               "Track misclassification rates by pattern",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_BY_PATTERN, pspec);

}

static void oscats_alg_class_rates_init (OscatsAlgClassRates *self)
{
}

static void oscats_alg_class_rates_dispose (GObject *object)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  G_OBJECT_CLASS(oscats_alg_class_rates_parent_class)->dispose(object);
  if (self->rate_by_pattern) g_tree_unref(self->rate_by_pattern);
  self->rate_by_pattern = NULL;
}

static void oscats_alg_class_rates_finalize (GObject *object)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  g_free(self->correct_attribute);
  g_free(self->misclassify_hist);
  G_OBJECT_CLASS(oscats_alg_class_rates_parent_class)->finalize(object);
}

static void oscats_alg_class_rates_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  switch (prop_id)
  {
    case PROP_BY_PATTERN:			// construction only
      if(g_value_get_boolean(value))
        self->rate_by_pattern = g_tree_new_full((GCompareDataFunc)oscats_attributes_compare,
                                                NULL, g_object_unref, g_free);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_class_rates_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  switch (prop_id)
  {
    case PROP_BY_PATTERN:
      g_value_set_boolean(value, self->rate_by_pattern != NULL);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void finalize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(alg_data);
  guint i, num = 0;	// num wrong

  self->num_examinees++;
  for (i=0; i < self->num_attrs; i++)
    if (oscats_attributes_get(e->true_alpha, i) !=
        oscats_attributes_get(e->alpha_hat, i))
      num++;
    else
      self->correct_attribute[i]++;
  if (num == 0) self->correct_patterns++;
  self->misclassify_hist[num]++;
  
  if (self->rate_by_pattern)
  {
    guint *data = g_tree_lookup(self->rate_by_pattern, e->true_alpha);
    if (!data)
    {
      OscatsAttributes *attr = g_object_new(OSCATS_TYPE_ATTRIBUTES, NULL);
      oscats_attributes_copy(attr, e->true_alpha);
      data = g_new0(guint, 2);
      g_tree_insert(self->rate_by_pattern, attr, data);
    }
    data[0]++;
    if (num == 0) data[1]++;
  }
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
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(alg_data);
  g_return_if_fail(oscats_item_bank_is_discr(test->itembank));

  g_signal_connect_data(test, "finalize", G_CALLBACK(finalize),
                        alg_data, oscats_algorithm_closure_finalize, 0);

  self->num_attrs = oscats_item_bank_num_attrs(test->itembank);
  self->correct_attribute = g_new0(guint, self->num_attrs);
  self->misclassify_hist = g_new0(guint, self->num_attrs+1);
}

/**
 * oscats_alg_class_rates_num_examinees:
 * @alg_data: the #OscatsAlgClassRates data object
 *
 * Returns: the number of examinees tested
 */
guint oscats_alg_class_rates_num_examinees(const OscatsAlgClassRates *alg_data)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  return alg_data->num_examinees;
}

/**
 * oscats_alg_class_rates_get_pattern_rate:
 * @alg_data: the #OscatsAlgClassRates data object
 *
 * The overall pattern classification success rate is:
 * r = N^-1 sum_i I_{alpha.hat_i = alpha_i}.
 *
 * Returns: the overall pattern classification success rate
 */
gdouble oscats_alg_class_rates_get_pattern_rate(const OscatsAlgClassRates *alg_data)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  return (gdouble)(alg_data->correct_patterns) /
         (gdouble)(alg_data->num_examinees);
}

/**
 * oscats_alg_class_rates_get_attribute_rate:
 * @alg_data: the #OscatsAlgClassRates data object
 * @i: the index of the attribute for which to query the rate
 *
 * The attribute classification success rate is:
 * r_k = N^-1 sum_i I_{alpha.hat_ik = alpha_ik}.
 *
 * Returns: the classification success rate for attribute @i
 */
gdouble oscats_alg_class_rates_get_attribute_rate(
                          const OscatsAlgClassRates *alg_data, guint i)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  g_return_val_if_fail(i < alg_data->num_attrs, 0);
  return (gdouble)(alg_data->correct_attribute[i]) /
         (gdouble)(alg_data->num_examinees);
}

/**
 * oscats_alg_class_rates_get_misclassify_freq:
 * @alg_data: the #OscatsAlgClassRates data object
 * @num: the number of misclassified attributes
 *
 * The frequency of having @num misclassified attributes is:
 * f_j = N^-1 sum_i I_{j = sum_k abs(alpha.hat_ik - alpha_ik)}.
 *
 * Returns: the rate of misclassifying @num attributes
 */
gdouble oscats_alg_class_rates_get_misclassify_freq(
                          const OscatsAlgClassRates *alg_data, guint num)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  g_return_val_if_fail(num <= alg_data->num_attrs, 0);
  return (gdouble)(alg_data->misclassify_hist[num]) /
         (gdouble)(alg_data->num_examinees);
}

/**
 * oscats_alg_class_rates_num_examinees_by_pattern:
 * @alg_data: the #OscatsAlgClassRates data object
 * @attr: the attribute pattern to query
 *
 * Rates for individual patterns are tabulated only if
 * #OscatsAlgClassRates:by-pattern is set to %TRUE at registration.
 *
 * Returns: the number of examinees with true attribute pattern @attr.
 */
guint oscats_alg_class_rates_num_examinees_by_pattern(
          const OscatsAlgClassRates *alg_data, const OscatsAttributes *attr)
{
  guint *data;
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data) &&
                       OSCATS_IS_ATTRIBUTES(attr), 0);
  g_return_val_if_fail(alg_data->rate_by_pattern != NULL, 0);
  data = g_tree_lookup(alg_data->rate_by_pattern, attr);
  if (!data) return 0;
  return data[0];
}

/**
 * oscats_alg_class_rates_get_rate_by_pattern:
 * @alg_data: the #OscatsAlgClassRates data object
 * @attr: the attribute pattern to query
 *
 * The correct classification rate for a given pattern is:
 * r_alpha = sum_i I_{alpha.hat_i = alpha_i} / sum_i I_{alpha_i = alpha}.
 *
 * Rates for individual patterns are tabulated only if
 * #OscatsAlgClassRates:by-pattern is set to %TRUE at registration.
 *
 * Returns: the number of examinees with true attribute pattern @attr.
 */
gdouble oscats_alg_class_rates_get_rate_by_pattern(
          const OscatsAlgClassRates *alg_data, const OscatsAttributes *attr)
{
  guint *data;
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data) &&
                       OSCATS_IS_ATTRIBUTES(attr), 0);
  g_return_val_if_fail(alg_data->rate_by_pattern != NULL, 0);
  data = g_tree_lookup(alg_data->rate_by_pattern, attr);
  if (!data) return 0;
  return (gdouble)(data[1])/(gdouble)(data[0]);
}
