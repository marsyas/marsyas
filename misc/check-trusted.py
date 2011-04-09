#!/usr/bin/env python

import os

# yes, it's another Not Invented Here solution for Marsyas!
# I'm in the mood to actually look at code, not play with build
# systems, so I'm setting up this framework here.


# these are in the src/marsyas/ dir
# the overall idea is to SLOWLY increase the number of "trusted
# files", once each file passes the below checks, and maybe
# has a unit test or something.
TRUSTED_FILES = """
	realvec.cpp
""".split()

CHECK_COMMAND = "g++ -c -Wall -Wextra -Werror "
# apparently you can be even stricter than -Wall -Wextra:
#CHECK_COMMAND = "g++ -c -Wall -Wextra -Weffc++ -Werror "

# TODO: maybe add other checkers, like cppcheck, oink, ccc,
# maybe even clang/llvm.

for filename in TRUSTED_FILES:
	print ("Checking %s" % (filename))
	cmd = CHECK_COMMAND + os.path.join("..", "src", "marsyas", filename)
	#print (cmd)
	os.system(cmd)



