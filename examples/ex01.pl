#! perl -I../bindings/perl/blib/arch -I../bindings/perl/blib/lib

# OSCATS: Open-Source Computerized Adaptive Testing System
# Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
#
# Example 1
# 400 Items: 1PL, b ~ N(0,1)
# 1000 Examinees: theta ~ N(0,1)
# Item selection:
#  - pick randomly
#  - match theta with b, exactly
#  - match theta with b, randomize 5, 10 items
# Test length: 30
# Report:
#  - theta.hat, Item exposure

use Glib;
use oscats;

$N_EXAMINEES = 1000;
$N_ITEMS = 400;
$LEN = 30;

sub gen_items {
  # Create an item bank to store the items.
  # Setting the property "sizeHint" increases allocation efficiency
  my $bank = new oscats::ItemBank("sizeHint", $N_ITEMS);
  for my $i (1 .. $N_ITEMS) {
    # First we create an IRT model container for our item
    my $model = new oscats::IrtModelL1p();
    # Then, set the parameters.  Here there is only one, the difficulty (b).
    $model->set_param_by_index(0, oscats::Random::normal(1));
    # Create an item based on this model
    my $item = new oscats::Item("irtmodel", $model);
    # Add the item to the item bank
    $bank->add_item($item)
    # Since Perl is garbage collected, we don't have to worry about
    # reference counting.
  }
  return $bank;
}

# Returns a list of new OscatsExaminee objects
sub gen_examinees {
  my @ret;
  # Latent IRT ability parameter.  This is a one-dimensional test.
  my $theta = new oscats::GslVector(1);
  for my $i (1 .. $N_EXAMINEES) {
    # Sample the ability from N(0,1) distribution
    $theta->set(0, oscats::Random::normal(1));
    # Create a new examinee
    my $e = new oscats::Examinee();
    # Set the examinee's true (simulated) ability
    # Note, theta is *copied* into examinee
    $e->set_true_theta($theta);
    push @ret, $e;
  }
  return @ret;
}

## main()

@test_names = ("random", "matched", "match.5", "match.10");

print "Creating examinees.\n";
@examinees = gen_examinees();
print "Creating items.\n";
$bank = gen_items();

print "Creating tests.\n";
# Create new tests with the given properties
  # In this case, we know what the length of the test will be (30),
  # so, we set length_hint for allocation efficiency.
  # Any good/reasonable guess will do---the size will be adjusted
  # automatically as necessary.
for $name (@test_names) {
  push @tests, new oscats::Test("id", $name, "itembank", $bank,
                                "length_hint", $LEN);
}
# Register the CAT algorithms for each test
# A test must have at minimum a selection algorithm, and administration
# algorithm, and a stoping critierion.
for $test (@tests) {
  oscats::Algorithm::register(oscats::AlgSimulateIrt, $test);
  oscats::Algorithm::register(oscats::AlgEstimateTheta, $test);
  
  # All calls to oscats_algorithm_register() return an algorithm
  # data object.  In many cases, we don't care about this object, since
  # it doesn't contain any interesting information.  But, for the
  # item exposure counter, we want to have access to the item exposure
  # rates when the test is over, so we keep the object.
  push @exposures,
    oscats::Algorithm::register(oscats::AlgExposureCounter, $test);
  
  oscats::Algorithm::register(oscats::AlgFixedLength, $test, "len", $LEN);
}

# Here we register the item selection criteria for the different tests
oscats::Algorithm::register(oscats::AlgPickRand, $tests[0]);
# The default for OscatsAlgClosestDiff is to pick the exact closest item
oscats::Algorithm::register(oscats::AlgClosestDiff, $tests[1]);
# But, we can have the algorithm choose randomly from among the num closest
oscats::Algorithm::register(oscats::AlgClosestDiff, $tests[2], "num", 5);
oscats::Algorithm::register(oscats::AlgClosestDiff, $tests[3], "num", 10);

print "Administering.\n";
open OUT, ">ex01-examinees.dat" or die "Can't open >ex01-examinees.dat: $!\n";
print OUT "ID\ttrue\t" . join("\t", map "$_\t$_.err", @test_names) . "\n";
$theta = new oscats::GslVector(1);	# Initialized to 0
$i = 1;
for $e (@examinees) {
  # We want errors for the estimates: initialize them.
  $e->init_theta_err(1);		# 1 is the test dimension

  printf OUT "%d\t%g", $i, $e->get_true_theta()->get(0);
  for $test (@tests) {
    # An initial estimate for latent IRT ability must be provided.
    # theta is *copied* into examinee
    $e->set_theta_hat($theta);
    
    # Do the administration!
    $test->administer($e);
    
    # Output the resulting theta.hat and its *variance*
    printf OUT "\t%g\t%g", $e->get_theta_hat()->get(0),
                           $e->get_theta_err()->get(0, 0);
  }
  print OUT "\n";
  $i += 1;
}
close OUT;

open OUT, ">ex01-items.dat" or die "Can't open >ex01-items.dat: $!\n";
print OUT "ID\tb\t" . join("\t", @test_names) . "\n";
for $i (1 .. $N_ITEMS) {
  $item = $bank->get_item($i-1);
  # Get item's difficulty paramter
  printf OUT "%d\t%g", $i, $item->get_property("irtmodel")->get_param_by_index(0);
  for $exposure (@exposures) {
    printf OUT "\t%g", $exposure->get_rate($item);
  }
  print OUT "\n";
}
close OUT;

# No cleanup necessary, since perl is garbage collected.
print "Done.\n";

