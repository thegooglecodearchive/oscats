#! python 

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

# Note, gobject *must* be imported before oscats
import gobject, oscats

N_EXAMINEES = 1000
N_ITEMS = 400
LEN = 30

def gen_items() :
  # Create an item bank to store the items.
  # Setting the property "sizeHint" increases allocation efficiency
  bank = gobject.new(oscats.ItemBank, sizeHint=N_ITEMS)
  for i in range(N_ITEMS) :
    # First we create an IRT model container for our item
    model = gobject.new(oscats.IrtModelL1p)
    # Then, set the parameters.  Here there is only one, the difficulty (b).
    model.set_param_by_index(0, oscats.oscats_rnd_normal(1))
    # Create an item based on this model
    item = gobject.new(oscats.Item, irtmodel=model)
    # Add the item to the item bank
    bank.add_item(item)
    # Since Python is garbage collected, we don't have to worry about
    # reference counting.
  return bank

# Returns a list of new OscatsExaminee objects
def gen_examinees() :
  ret = []
  # Latent IRT ability parameter.  This is a one-dimensional test.
  theta = oscats.GslVector(1)
  for i in range(N_EXAMINEES) :
    # Sample the ability from N(0,1) distribution
    theta.set(0, oscats.oscats_rnd_normal(1))
    # Create a new examinee
    e = gobject.new(oscats.Examinee)
    # Set the examinee's true (simulated) ability
    # Note, theta is *copied* into examinee
    e.set_true_theta(theta)
    ret.append(e)
  return ret

## main()

test_names = ("random", "matched", "match.5", "match.10")

print "Creating examinees."
examinees = gen_examinees()
print "Creating items."
bank = gen_items()

print "Creating tests."
# Create new tests with the given properties
  # In this case, we know what the length of the test will be (30),
  # so, we set length_hint for allocation efficiency.
  # Any good/reasonable guess will do---the size will be adjusted
  # automatically as necessary.
tests = [ gobject.new(oscats.Test, id=name, itembank=bank,
                      length_hint=LEN)   for name in test_names ]
exposures = []
# Register the CAT algorithms for each test
# A test must have at minimum a selection algorithm, and administration
# algorithm, and a stoping critierion.
for test in tests :
  oscats.oscats_algorithm_register(oscats.AlgSimulateIrt, test)
  oscats.oscats_algorithm_register(oscats.AlgEstimateTheta, test)
  
  # All calls to oscats_algorithm_register() return an algorithm
  # data object.  In many cases, we don't care about this object, since
  # it doesn't contain any interesting information.  But, for the
  # item exposure counter, we want to have access to the item exposure
  # rates when the test is over, so we keep the object.
  exposures.append(
    oscats.oscats_algorithm_register(oscats.AlgExposureCounter, test) )
  
  oscats.oscats_algorithm_register(oscats.AlgFixedLength, test, len=LEN)

# Here we register the item selection criteria for the different tests
oscats.oscats_algorithm_register(oscats.AlgPickRand, tests[0])
# The default for OscatsAlgClosestDiff is to pick the exact closest item
oscats.oscats_algorithm_register(oscats.AlgClosestDiff, tests[1])
# But, we can have the algorithm choose randomly from among the num closest
oscats.oscats_algorithm_register(oscats.AlgClosestDiff, tests[2], num=5)
oscats.oscats_algorithm_register(oscats.AlgClosestDiff, tests[3], num=10)

print "Administering."
out = open("ex01-examinees.dat", "w")
out.write("ID\ttrue\t" + "\t".join([ name + "\t" + name + ".err"
                                     for name in test_names ]) + "\n")
theta = oscats.GslVector(1)	# Initialized to 0
i = 1
for e in examinees :
  # We want errors for the estimates: initialize them.
  e.init_theta_err(1)	# 1 is the test dimension

  out.write("%d\t%g" % (i, e.get_true_theta().get(0) ))
  for test in tests :
    # An initial estimate for latent IRT ability must be provided.
    # theta is *copied* into examinee
    e.set_theta_hat(theta)
    
    # Do the administration!
    test.administer(e)
    
    # Output the resulting theta.hat and its *variance*
    out.write("\t%g\t%g" % (e.get_theta_hat().get(0),
                            e.get_theta_err().get(0, 0) ))
  out.write("\n")
  i += 1
out.close()

out = open("ex01-items.dat", "w")
out.write("ID\tb\t" + "\t".join(test_names) + "\n")
for i in range(N_ITEMS) :
  item = bank.get_item(i)
  # Get item's difficulty paramter
  out.write("%d\t%g" % (i+1,
    item.get_property("irtmodel").get_param_by_index(0) ))
  for exposure in exposures :
    out.write("\t%g" % exposure.get_rate(item))
  out.write("\n")
out.close()

# No cleanup necessary, since python is garbage collected.
print "Done."

