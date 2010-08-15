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

#ifndef _LIBOSCATS_ALGORITHM_H_
#define _LIBOSCATS_ALGORITHM_H_
#include <glib.h>
#include <test.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALGORITHM		(oscats_algorithm_get_type())
#define OSCATS_ALGORITHM(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALGORITHM, OscatsAlgorithm))
#define OSCATS_IS_ALGORITHM(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALGORITHM))
#define OSCATS_ALGORITHM_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALGORITHM, OscatsAlgorithmClass))
#define OSCATS_IS_ALGORITHM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALGORITHM))
#define OSCATS_ALGORITHM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALGORITHM, OscatsAlgorithmClass))

typedef struct _OscatsAlgorithm OscatsAlgorithm;
typedef struct _OscatsAlgorithmClass OscatsAlgorithmClass;

struct _OscatsAlgorithm {
  GObject parent_instance;
};

struct _OscatsAlgorithmClass {
  GObjectClass parent_class;
  void (*reg) (OscatsAlgorithm *alg_data, OscatsTest *test);
};

GType oscats_algorithm_get_type();

gpointer oscats_algorithm_register(GType alg, OscatsTest *test,
                                   const gchar *first_property_name, ...);
gpointer oscats_algorithm_register_valist(GType alg, OscatsTest *test,
                                          const gchar *first_property_name,
                                          va_list var_args);

// Protected
void oscats_algorithm_closure_finalize (gpointer alg_data, GClosure *closure);

G_END_DECLS
#endif
