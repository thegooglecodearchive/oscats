/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Abstract CAT Algorithm Class
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

/**
 * SECTION:algorithm
 * @title:OscatsAlgorithm
 * @short_description: Abstract CAT Algorithm Class
 */

#include "algorithm.h"

G_DEFINE_TYPE(OscatsAlgorithm, oscats_algorithm, G_TYPE_OBJECT);

static void null_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
  g_critical("Abstract CAT Algorithm should be overloaded.");
}
                   
static void oscats_algorithm_class_init (OscatsAlgorithmClass *klass)
{
//  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  klass->reg = null_register;
}

static void oscats_algorithm_init (OscatsAlgorithm *self)
{
}

/**
 * oscats_algorithm_register:
 * @alg: the type id of the #OscatsAlgorithm descendant to register
 * @test: the #OscatsTest on which to register the algorithm
 * @first_property_name: the name of the first property (or %NULL)
 * @...: value of the first property, followed optionally by more name/value
 *  pairs, followed by %NULL
 *
 * Registers the algorithm @alg for use in @test, and sets its properties.
 * The (implementation-specific) object returned is owned by @test,
 * so its reference count must be increased if kept.
 *
 * Returns: a new #OscatsAlgorithm object for accessing algorithm variables
 */
gpointer oscats_algorithm_register(GType alg, OscatsTest *test,
                                   const gchar *first_property_name, ...)
/*
{
  OscatsAlgorithm *alg_data;
  va_list var_args;
  OscatsAlgorithmClass *klass;
  g_return_val_if_fail(g_type_is_a(alg, OSCATS_TYPE_ALGORITHM) &&
                       OSCATS_IS_TEST(test), NULL);
  va_start(var_args, first_property_name);
        // Cast to gpointer okay because we checked type of alg above.
  alg_data = (gpointer)g_object_new_valist(alg, first_property_name, var_args);
  va_end(var_args);
  g_return_val_if_fail(alg_data != NULL, alg_data);    // if bad properties
  klass = g_type_class_ref(alg);
  klass->reg(alg_data, test);
  g_type_class_unref(klass);
  return alg_data;
}
*/
{
  OscatsAlgorithm *alg_data;
  va_list var_args;
  va_start(var_args, first_property_name);
  alg_data = oscats_algorithm_register_valist(alg, test,
                                              first_property_name, var_args);
  va_end(var_args);
  return alg_data;
}

/**
 * oscats_algorithm_register_valist:
 * @alg: the type id of the #OscatsAlgorithm descendant to register
 * @test: the #OscatsTest on which to register the algorithm
 * @first_property_name: the name of the first property (or %NULL)
 * @var_args: value of the first property, followed optionally by more
 * name/value pairs, followed by %NULL
 *
 * Registers the algorithm @alg for use in @test, and sets its properties.
 * The (implementation-specific) object returned is owned by @test,
 * so its reference count must be increased if kept.
 *
 * Returns: a new #OscatsAlgorithm object for accessing algorithm variables
 */
gpointer oscats_algorithm_register_valist(GType alg, OscatsTest *test,  
                                          const gchar *first_property_name,
                                          va_list var_args)
{
  OscatsAlgorithm *alg_data;
  OscatsAlgorithmClass *klass;
  g_return_val_if_fail(g_type_is_a(alg, OSCATS_TYPE_ALGORITHM) &&
                       OSCATS_IS_TEST(test), NULL);
        // Cast to gpointer okay because we checked type of alg above.
  alg_data = (gpointer)g_object_new_valist(alg, first_property_name, var_args);
  g_return_val_if_fail(alg_data != NULL, alg_data);    // if bad properties
  klass = g_type_class_ref(alg);
  klass->reg(alg_data, test);
  g_type_class_unref(klass);
  return alg_data;
}

/**
 * oscats_algorithm_closure_finalize:
 * @alg_data: data to free
 * @closure: signal handler
 *
 * Calls g_object_unref(alg_data).  Should not be invoked directly, but
 * is supplied as the destroy_notifier at signal connection.
 */
void oscats_algorithm_closure_finalize(gpointer alg_data, GClosure *closure)
{ g_object_unref(alg_data); }
