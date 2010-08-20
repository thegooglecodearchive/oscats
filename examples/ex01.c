/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
 *
 * Example 1
 *
 * 400 Items: 1PL, b ~ N(0,1)
 * 1000 Examinees: theta ~ N(0,1)
 * Item selection:
 *  - pick randomly
 *  - match theta with b, exactly
 *  - match theta with b, randomize 5, 10 items
 * Test length: 30
 * Report:
 *  - theta.hat, Item exposure
 */

#include <stdio.h>
#include <glib.h>
#include <oscats.h>

#define N_EXAMINEES 1000
#define N_ITEMS 400
#define LEN 30

OscatsItemBank * gen_items()
{
  OscatsContModel *model;
  OscatsItem *item;
  // Create an item bank to store the items.
  // Setting the property "sizeHint" increases allocation efficiency.
  // Be sure to include NULL at the end of calls to g_object_new()!
  OscatsItemBank *bank = g_object_new(OSCATS_TYPE_ITEM_BANK,
                                      "sizeHint", N_ITEMS, NULL);
  guint i;
  for (i=0; i < N_ITEMS; i++)
  {
    // First we create an IRT model container for our item
    model = g_object_new(OSCATS_TYPE_CONT_MODEL_L1P, NULL);
    // Then, set the parameters.  Here there is only one, the difficulty (b).
    oscats_cont_model_set_param_by_index(model, 0, oscats_rnd_normal(1));
    // Create an item based on this model
    item = g_object_new(OSCATS_TYPE_ITEM, "contmodel", model, NULL);
    // Add the item to the item bank
    oscats_item_bank_add_item(bank, item);
    // We no longer need item and model (they're stored safely in the bank)
    // so, we unref them here.
    g_object_unref(item);
    g_object_unref(model);
  }
  return bank;
}

// Returns an array of new OscatsExaminee pointers
OscatsExaminee ** gen_examinees()
{
  OscatsExaminee ** ret = g_new(OscatsExaminee*, N_EXAMINEES);
  GGslVector *theta;
  guint i;
  
  // Latent IRT ability parameter.  This is a one-dimensional test.
  theta = g_gsl_vector_new(1);
  for (i=0; i < N_EXAMINEES; i++)
  {
    // Sample the ability from N(0,1) distribution
    g_gsl_vector_set(theta, 0, oscats_rnd_normal(1));
    // Create a new examinee
    ret[i] = g_object_new(OSCATS_TYPE_EXAMINEE, NULL);
    // Set the examinee's true (simulated) ability
    // Note, theta is *copied* into examinee.
    oscats_examinee_set_true_theta(ret[i], theta);
  }
  
  // Clean up
  g_object_unref(theta);
  return ret;
}

int main()
{
  FILE *f;
  OscatsExaminee **examinees;
  OscatsItemBank *bank;
  const guint num_tests = 4;
  const gchar *test_names[] = { "random", "matched", "match.5", "match.10" };
  OscatsTest *test[num_tests];
  OscatsAlgExposureCounter *exposure[num_tests];
  guint i, j;
  
  // With G_TYPE_DEBUG_OBJECTS, we check for leaks at the end of the program
  // Could just have called g_type_init(), instead.
  g_type_init_with_debug_flags(G_TYPE_DEBUG_OBJECTS);
  
  printf("Creating examinees.\n");
  examinees = gen_examinees();
  printf("Creating items.\n");
  bank = gen_items();

  printf("Creating tests.\n");
  for (j=0; j < num_tests; j++)
  {
    // Create a new test with the given properties
    // Be sure to end calls to g_object_new() with NULL!

    // In this case, we know what the length of the test will be (30),
    // so, we set length_hint for allocation efficiency.
    // Any good/reasonable guess will do---the size will be adjusted
    // automatically as necessary.
    test[j] = g_object_new(OSCATS_TYPE_TEST, "id", test_names[j],
                           "itembank", bank, "length_hint", LEN, NULL);

    // Register the CAT algorithms for this test.
    // A test must have at minimum a selection algorithm, and administration
    // algorithm, and a stoping critierion.

    // Notice that the GType (not an instantiated object) is given
    // Be sure to end all calls to oscats_algorithm_register() with NULL!
    oscats_algorithm_register(OSCATS_TYPE_ALG_SIMULATE_THETA, test[j], NULL);
    oscats_algorithm_register(OSCATS_TYPE_ALG_ESTIMATE_THETA, test[j], NULL);

    // All calls to oscats_algorithm_register() return an algorithm
    // data object.  In many cases, we don't care about this object, since
    // it doesn't contain any interesting information.  But, for the
    // item exposure counter, we want to have access to the item exposure
    // rates when the test is over, so we keep the object.
    exposure[j] = oscats_algorithm_register(OSCATS_TYPE_ALG_EXPOSURE_COUNTER, 
                                            test[j], NULL);
    // But, the object doesn't belong to us, yet, so increase ref count:
    g_object_ref(exposure[j]);

    oscats_algorithm_register(OSCATS_TYPE_ALG_FIXED_LENGTH, test[j],
                              "len", LEN, NULL);
  }
  
  // Here we register the item selection criteria for the different tests
  oscats_algorithm_register(OSCATS_TYPE_ALG_PICK_RAND, test[0], NULL);
  // The default for OscatsAlgClosestDiff is to pick the exact closest item
  oscats_algorithm_register(OSCATS_TYPE_ALG_CLOSEST_DIFF, test[1], NULL);
  // But, we can have the algorithm choose randomly from among the num closest
  oscats_algorithm_register(OSCATS_TYPE_ALG_CLOSEST_DIFF, test[2],
                            "num", 5, NULL);
  oscats_algorithm_register(OSCATS_TYPE_ALG_CLOSEST_DIFF, test[3],
                            "num", 10, NULL);
  
  printf("Administering.\n");
  f = fopen("ex01-examinees.dat", "w");
  fprintf(f, "ID\ttrue");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s\t%s.err", test_names[j], test_names[j]);
  fprintf(f, "\n");
  for (i=0; i < N_EXAMINEES; i++)
  {
//    printf("  %s\n", examinees[i]->id);

    // An initial estimate for latent IRT ability must be provided.
    examinees[i]->theta_hat = g_gsl_vector_new(1);
    // We want errors for the estimates: initialize them.
    // 1 is the test dimension
    oscats_examinee_init_theta_err(examinees[i], 1);

    fprintf(f, "%d\t%g", i+1,
            g_gsl_vector_get(examinees[i]->true_theta, 0));
    for (j=0; j < num_tests; j++)
    {
      // Reset initial latent ability for this test
      g_gsl_vector_set(examinees[i]->theta_hat, 0, 0);
      // Do the administration!
      oscats_test_administer(test[j], examinees[i]);
      // Output the resulting theta.hat and its *variance*
      fprintf(f, "\t%g\t%g", g_gsl_vector_get(examinees[i]->theta_hat, 0),
            g_gsl_matrix_get(examinees[i]->theta_err, 0, 0));
    }
    fprintf(f, "\n");
  }
  fclose(f);
  
  f = fopen("ex01-items.dat", "w");
  fprintf(f, "ID\tb");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s", test_names[j]);
  fprintf(f, "\n");
  for (i=0; i < N_ITEMS; i++)
  {
    // Get the item's difficulty parameter
    fprintf(f, "%d\t%g", i+1,
            oscats_cont_model_get_param_by_index(oscats_item_bank_get_item(bank, i)->cont_model, 0));
    // Get the exposure rate for this item in each test
    for (j=0; j < num_tests; j++)
      fprintf(f, "\t%g",
            oscats_alg_exposure_counter_get_rate(exposure[j],
                  oscats_item_bank_get_item(bank, i)) );
    fprintf(f, "\n");
  }
  fclose(f);

  // Clean up
  printf("Done.\n");
  for (j=0; j < num_tests; j++)
  {
    g_object_unref(exposure[j]);
    g_object_unref(test[j]);
  }
  g_object_unref(bank);
  for (i=0; i < N_EXAMINEES; i++)
    g_object_unref(examinees[i]);
  g_free(examinees);
  
  return 0;
}
