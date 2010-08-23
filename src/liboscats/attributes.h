/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Latent Attributes (Classification) Identifier
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

#ifndef _LIBOSCATS_ATTRIBUTES_H_
#define _LIBOSCATS_ATTRIBUTES_H_
#include <glib.h>
#include <glib-object.h>
G_BEGIN_DECLS

typedef guint32 _AttributesType;
#define OSCATS_MAX_ATTRIBUTES 32

#define OSCATS_TYPE_ATTRIBUTES		(oscats_attributes_get_type())
#define OSCATS_ATTRIBUTES(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ATTRIBUTES, OscatsAttributes))
#define OSCATS_IS_ATTRIBUTES(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ATTRIBUTES))
#define OSCATS_ATTRIBUTES_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ATTRIBUTES, OscatsAttributesClass))
#define OSCATS_IS_ATTRIBUTES_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ATTRIBUTES))
#define OSCATS_ATTRIBUTES_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ATTRIBUTES, OscatsAttributesClass))

typedef struct _OscatsAttributes OscatsAttributes;
typedef struct _OscatsAttributesClass OscatsAttributesClass;

struct _OscatsAttributes {
  GObject parent_instance;
  _AttributesType data;
  guint8 num;
};

struct _OscatsAttributesClass {
  GObjectClass parent_class;
};

GType oscats_attributes_get_type();

OscatsAttributes * oscats_attributes_from_string(const gchar *str);
void oscats_attributes_set(OscatsAttributes *attributes,
                              guint8 index, gboolean value);
gboolean oscats_attributes_get(const OscatsAttributes *attributes,
                                  guint8 index);
guint32 oscats_attributes_as_int(const OscatsAttributes *attributes);
gchar * oscats_attributes_as_string(const OscatsAttributes *attributes);
void oscats_attributes_copy(OscatsAttributes *lhs,
                               const OscatsAttributes *rhs);
gint oscats_attributes_compare(OscatsAttributes *a, OscatsAttributes *b);

G_END_DECLS
#endif
