/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Examinee Class
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
 * SECTION:examinee
 * @title:OscatsExaminee
 * @short_description: Examinee Class
 */

#include "examinee.h"

G_DEFINE_TYPE(OscatsExaminee, oscats_examinee, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
  PROP_COVARIATES,
};

static void oscats_examinee_dispose (GObject *object);
static void oscats_examinee_finalize (GObject *object);
static void oscats_examinee_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_examinee_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
                   
static void oscats_examinee_class_init (OscatsExamineeClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_examinee_dispose;
  gobject_class->finalize = oscats_examinee_finalize;
  gobject_class->set_property = oscats_examinee_set_property;
  gobject_class->get_property = oscats_examinee_get_property;
  
/**
 * OscatsExaminee:id:
 *
 * A string identifier for the examinee.
 */
  pspec = g_param_spec_string("id", "ID", 
                            "String identifier for the examinee",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsExaminee:covariates:
 *
 * A set of covariates for the examinee.
 */
  pspec = g_param_spec_object("covariates", "Covariates", 
                            "Covariates for the examinee",
                            OSCATS_TYPE_COVARIATES,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_COVARIATES, pspec);

}

static void safe_unref(gpointer obj)
{
  if (obj) g_object_unref(obj);
}

static void oscats_examinee_init (OscatsExaminee *self)
{
}

static void oscats_examinee_dispose (GObject *object)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  G_OBJECT_CLASS(oscats_examinee_parent_class)->dispose(object);
  if (self->items) g_ptr_array_unref(self->items);
  if (self->resp) g_byte_array_unref(self->resp);
  if (self->true_theta) g_object_unref(self->true_theta);
  if (self->theta_hat) g_object_unref(self->theta_hat);
  if (self->theta_err) g_object_unref(self->theta_err);
  if (self->true_alpha) g_object_unref(self->true_alpha);
  if (self->alpha_hat) g_object_unref(self->alpha_hat);
  if (self->covariates) g_object_unref(self->covariates);
  self->items = NULL;
  self->resp = NULL;
  self->true_theta = NULL;
  self->theta_hat = NULL;
  self->theta_err = NULL;
  self->true_alpha = NULL;
  self->alpha_hat = NULL;
  self->covariates = NULL;
}

static void oscats_examinee_finalize (GObject *object)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  g_free(self->id);
  G_OBJECT_CLASS(oscats_examinee_parent_class)->finalize(object);
}

static void oscats_examinee_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(22);
        g_string_printf(id, "[Examinee %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_examinee_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
    
    case PROP_COVARIATES:
      if (!self->covariates)
        self->covariates = g_object_new(OSCATS_TYPE_COVARIATES, NULL);
      g_value_set_object(value, self->covariates);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_examinee_set_true_theta:
 * @e: an #OscatsExaminee
 * @t: a #GGslVector for the true (simulated) latent IRT ability
 *
 * Sets the true latent IRT ability for the simulated examinee.
 * The vector @t is copied.  The internal dimension is adjusted
 * to match @t.
 */
void oscats_examinee_set_true_theta(OscatsExaminee *e, const GGslVector *t)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e) && G_GSL_IS_VECTOR(t) && t->v);
  if (!e->true_theta)
    e->true_theta = g_gsl_vector_new(t->v->size);
  else if (e->true_theta->v->size != t->v->size)
    g_gsl_vector_resize(e->true_theta, t->v->size);
  g_gsl_vector_copy(e->true_theta, t);
}

/**
 * oscats_examinee_get_true_theta:
 * @e: an #OscatsExaminee
 *
 * Returns %NULL if true latent IRT ability has never been set.
 * Does not increase reference count.
 *
 * Returns: a pointer to the simulated examinee's true latent IRT ability
 */
GGslVector * oscats_examinee_get_true_theta(OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->true_theta;
}

/**
 * oscats_examinee_set_theta_hat:
 * @e: an #OscatsExaminee
 * @t: a #GGslVector for the estimated latent IRT ability
 *
 * Sets the estimated latent IRT ability for the examinee.
 * The vector @t is copied.  The internal dimension is adjusted
 * to match @t.
 */
void oscats_examinee_set_theta_hat(OscatsExaminee *e, const GGslVector *t)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e) && G_GSL_IS_VECTOR(t) && t->v);
  if (!e->theta_hat)
    e->theta_hat = g_gsl_vector_new(t->v->size);
  else if (e->theta_hat->v->size != t->v->size)
    g_gsl_vector_resize(e->theta_hat, t->v->size);
  g_gsl_vector_copy(e->theta_hat, t);
}

/**
 * oscats_examinee_get_theta_hat:
 * @e: an #OscatsExaminee
 *
 * Does not increase reference count.
 * Returns %NULL if estimated latent IRT ability has never been set.
 *
 * Returns: a pointer to the examinee's estimated latent IRT ability
 */
GGslVector * oscats_examinee_get_theta_hat(OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->theta_hat;
}

/**
 * oscats_examinee_init_theta_err:
 * @e: an #OscatsExaminee
 * @dim: the test dimension
 *
 * Initializes a matrix for the covariance of the estimated latent IRT ability.
 */
void oscats_examinee_init_theta_err(OscatsExaminee *e, guint dim)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e) && dim > 0);
  e->theta_err = g_gsl_matrix_new(dim, dim);
}

/**
 * oscats_examinee_get_theta_err:
 * @e: an #OscatsExaminee
 *
 * Returns the covariance matrix of the examinee's estimated latent IRT
 * ability.  Does not increase reference count.  Returns %NULL if covariance
 * matrix for the estimated latent IRT ability has never been set.
 *
 * Returns: a pointer to the covariance matrix
 */
GGslMatrix * oscats_examinee_get_theta_err(OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->theta_err;
}

/**
 * oscats_examinee_set_true_alpha:
 * @e: an #OscatsExaminee
 * @attr: an #OscatsAttributes for the true (simulated) latent attributes
 *
 * Sets the true latent attributes for the simulated examinee.
 * The vector @status is copied.  The internal dimension is adjusted
 * to match @status.
 */
void oscats_examinee_set_true_alpha(OscatsExaminee *e,
                                    const OscatsAttributes *attr)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  if (!e->true_alpha)
    e->true_alpha = g_object_new(OSCATS_TYPE_ATTRIBUTES, NULL);
  oscats_attributes_copy(e->true_alpha, attr);
}

/**
 * oscats_examinee_get_true_alpha:
 * @e: an #OscatsExaminee
 *
 * Returns %NULL if true latent attributes have never been set.
 * Does not increase reference count.
 *
 * Returns: a pointer to the simulated examinee's true latent classification
 */
OscatsAttributes * oscats_examinee_get_true_alpha(OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->true_alpha;
}

/**
 * oscats_examinee_set_alpha_hat:
 * @e: an #OscatsExaminee
 * @attr: an #OscatsAttributes for the estimated latent classification
 *
 * Sets the estimated latent attributes for the examinee.  The vector
 * @status is copied.  The internal dimension is adjusted to match @status.
 */
void oscats_examinee_set_alpha_hat(OscatsExaminee *e, 
                                   const OscatsAttributes *attr)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  if (!e->alpha_hat)
    e->alpha_hat = g_object_new(OSCATS_TYPE_ATTRIBUTES, NULL);
  oscats_attributes_copy(e->alpha_hat, attr);
}

/**
 * oscats_examinee_get_alpha_hat:
 * @e: an #OscatsExaminee
 *
 * Returns %NULL if estimated latent attributes have never been set.
 * Does not increase reference count.
 *
 * Returns: a pointer to the examinee's estimated latent classification
 */
OscatsAttributes * oscats_examinee_get_alpha_hat(OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->alpha_hat;
}

/**
 * oscats_examinee_prep:
 * @e: an #OscatsExaminee
 * @length_hint: guess for test length
 *
 * Prepares the examinee for the CAT by reseting the item/resp arrays.
 * If the arrays do not already exist, they are preallocated to hold
 * @length_hint elements (recommended), the expected length of the test.
 */
void oscats_examinee_prep(OscatsExaminee *e, guint length_hint)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  if (e->items)
    g_ptr_array_set_size(e->items, 0);
  else
  {
    e->items = g_ptr_array_sized_new(length_hint);
    g_ptr_array_set_free_func(e->items, safe_unref);
  }
  if (e->resp)
    g_byte_array_set_size(e->resp, 0);
  else
    e->resp = g_byte_array_sized_new(length_hint);
}

/**
 * oscats_examinee_add_item:
 * @e: an #OscatsExaminee
 * @item: the #OscatsItem this examinee has taken
 * @resp: the examinee's response to the item
 *
 * Adds the (@item, @resp) pair to the list of items this examinee has been
 * administered.  The reference count for @item is increased.
 */
void oscats_examinee_add_item(OscatsExaminee *e, OscatsItem *item, guint8 resp)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e) && OSCATS_IS_ITEM(item));
  g_ptr_array_add(e->items, item);
  g_object_ref(item);
  g_byte_array_append(e->resp, &resp, 1);
}

/**
 * oscats_examinee_num_items:
 * @e: an #OscatsExaminee
 *
 * Returns: the number of items this examinee has been administered
 */
guint oscats_examinee_num_items(const OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), 0);
  if (!e->items) return 0;
  return e->items->len;
}

