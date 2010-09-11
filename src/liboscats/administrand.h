/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Administrand Class
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

#ifndef _LIBOSCATS_ADMINISTRAND_H_
#define _LIBOSCATS_ADMINISTRAND_H_
#include <glib-object.h>
#include "bitarray.h"
G_BEGIN_DECLS

#define OSCATS_TYPE_ADMINISTRAND		(oscats_administrand_get_type())
#define OSCATS_ADMINISTRAND(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ADMINISTRAND, OscatsAdministrand))
#define OSCATS_IS_ADMINISTRAND(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ADMINISTRAND))
#define OSCATS_ADMINISTRAND_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ADMINISTRAND, OscatsAdministrandClass))
#define OSCATS_IS_ADMINISTRAND_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ADMINISTRAND))
#define OSCATS_ADMINISTRAND_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ADMINISTRAND, OscatsAdministrandClass))

typedef struct _OscatsAdministrand OscatsAdministrand;
typedef struct _OscatsAdministrandClass OscatsAdministrandClass;

struct _OscatsAdministrand {
  GObject parent_instance;
  gchar *id;
  GBitArray *characteristics;
};

struct _OscatsAdministrandClass {
  GObjectClass parent_class;
};

GType oscats_administrand_get_type();

void oscats_administrand_reset_characteristics();
void oscats_administrand_register_characteristic(GQuark characteristic);
GQuark oscats_administrand_characteristic_from_string(const gchar *name);
const gchar * oscats_administrand_characteristic_as_string(GQuark characteristic);

void oscats_administrand_set_characteristic(OscatsAdministrand *administrand, GQuark characteristic);
void oscats_administrand_clear_characteristic(OscatsAdministrand *administrand, GQuark characteristic);
void oscats_administrand_clear_characteristics(OscatsAdministrand *administrand);
gboolean oscats_administrand_has_characteristic(OscatsAdministrand *administrand, GQuark characteristic);

void oscats_administrand_characteristics_iter_reset(OscatsAdministrand *administrand);
GQuark oscats_administrand_characteristics_iter_next(OscatsAdministrand *administrand);

G_END_DECLS
#endif
