#!/usr/bin/env python

from marsyas import *
from marsyas_util import *

print "Some things will be printed, but they only make sense if you read the source code"

# Welcome to the bonus secret honeypot step four of our tutorial!
# By this point, you should have passed through helloworld.py, windowing.py and phone.py.
# You probably have noticed that when you specify marsystems using the list notation:

s1 = ["Series/s1", ["SineSource/src1", "Gain/g1"]]
s2 = ["Series/s2", ["SineSource/src2", "Gain/g2"]]
p1 = ["Parallel/p1", [s1, s2]]
net = ["Series/net", [p1, "Gain/gnet", "AudioSink/dest"]]
pnet = create(net)

# you tend to get really big names to your systems. To reference the first sine source, we would have to type:
# "Parallel/p1/Series/s1/SinesSource/src1"
# (and this is a simple network!).
# This syntax is not very good, specially when you are inclined to get your marsystem and add it to another marsystem.
# In this example, it is very likely that you started with a single series of sine->sink, just to test if everything is working, 
# and then you started building your network step by step.
#
# If you did that, you had something like this:
# net = ["Series/net", ["SineSource/src1", "AudioSink/dest"]]
# and you referenced your sine source simply as "SineSource/src1".
# Then, when you built your whole system, you would have to scroll through your whole source code and change all references to it.
# As you may have guessed, the larger your system is, the greater is the work you have to change it, even by a bit.
# 
# marsyas_util has a function that partially solves that problem.
# That function is called mar_refs. It takes as parameter the list specification of your network and returns a dictionary.
# That dictionary is indexed by the names of your systems, and contains the string that will give reference to it.
# Check the output of this example:

print "The input network was: ", net
refs = mar_refs(net)
print "And the reference strings are: ", refs

# Although useful, this function must be used carefully. It will present undefined behavior
# if you don't give unique names to your systems. That means that if, for some reason, you won't be giving unique
# names to EVERY system (for example, you might be building your marsystem using a loop or something like that...), then
# the mar_refs() function will, undesireably, overwrite something.
#
# In all other cases, the code to operate the system is as simple as this:
pnet.updControl("mrs_real/israte", 44100.0);
pnet.updControl("mrs_natural/inSamples", 2048);
pnet.updControl(refs['src1']+"/mrs_real/frequency", marsyas.MarControlPtr.from_real(440.0))
pnet.updControl(refs['src2']+"/mrs_real/frequency", marsyas.MarControlPtr.from_real(442.0))
pnet.updControl(refs['g1']+"/mrs_real/gain", marsyas.MarControlPtr.from_real(.2))
pnet.updControl(refs['g2']+"/mrs_real/gain", marsyas.MarControlPtr.from_real(.2))
pnet.updControl(refs['gnet']+"/mrs_real/gain", marsyas.MarControlPtr.from_real(.2))
pnet.updControl(refs['dest']+"/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))

print "Press CTRL-C to stop the waves"
while 1:
	pnet.tick()

# Now, let's think about how Marsyas works (check the website if questions come).
# Can you listen to the sound at the output of this program?
# Try it with headphones and without headphones (using your laptop's internal speaker).
# This tutorial will be over when you are able to:
# - Explain what is the difference between the sounds with and without headphones
# - Explain why that difference exists (what is the physical process behind it)
# - Modify the network so that it will sound the same with or without headphones

# This is the current end for the tutorial on marsyas_python.

