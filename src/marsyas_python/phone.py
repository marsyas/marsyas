#!/usr/bin/env python

# Now, to step three of our tutorial!
# By this point, you should have passed through helloworld.py and windowing.py.
# This is a conclusion of the first part of the tutorial. After this, you should be able to
# use MarSystems and interact with them as needed.

from marsyas import *
from marsyas_util import *

# This network will be a little more tricky. So we will use a trick and specify it in parts.
# Our target network looks like this:
#
# Sin(xt)											gain => biquad
#     \\                     //            \\
#       => + => clip => biquad               => + => biquad => biquad => gain => sink
#     //                     \\            //
# Sin(yt)											clip  =>  gain
#
# We probably want to split that in parts of simple topology. 
# The first thing that we might notice is that the network is, as a whole, a series.
# Then, we have two parts where the network splits, which are the Sin() and the gain/biquad branches.
# When a network has one input and several outputs, it is called a Fanout.
# So, we could create the two fanout networks, and then, considering them atoms, we could
# create a series network. So, lets get down to business!

input_specification = ["Fanout/mix", ["SineSource/src1", "SineSource/src2"]];

tline_specification = ["Fanout/tline",
												[["Series/tline_b1", ["Gain/gain2",  "Biquad/biquad2"]],
												["Series/tline_b2", ["Gain/gain3", "Clip/clip2"]]]];

whole_net_specification = ["Series/pnet",
													[input_specification,
													"Sum/sum", "Clip/clip", "Biquad/biquad",
													tline_specification,
													"Sum/sum2",
													"Biquad/hpf1",
													"Biquad/hpf2",
													"Gain/gain",
													"AudioSink/dest"]];

pnet = create(whole_net_specification)

# And now we see how python's lists are great to describe networks of systems.
# We avoid having too many levels of nested brackets by using auxiliary lists
# (isn't that awesome???)
# The next following lines are simply configuring the network, so that it will work for what we want.
# If you understood so far, then you are ready to use Marsyas for your own purposes.

pnet.updControl("mrs_real/israte", 44100.0);

pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(440.0))
pnet.updControl("Fanout/mix/SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(350.0))
pnet.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))
pnet.updControl("Clip/clip/mrs_real/range", marsyas.MarControlPtr.from_real(0.9))
pnet.updControl("Fanout/tline/Series/tline_b2/Clip/clip2/mrs_real/range", marsyas.MarControlPtr.from_real(0.4))
pnet.updControl("Fanout/tline/Series/tline_b1/Gain/gain2/mrs_real/gain", marsyas.MarControlPtr.from_real(0.5))
pnet.updControl("Fanout/tline/Series/tline_b2/Gain/gain3/mrs_real/gain", marsyas.MarControlPtr.from_real(0.15))
pnet.updControl("Fanout/tline/Series/tline_b1/Biquad/biquad2/mrs_real/frequency", marsyas.MarControlPtr.from_real(400.0))
pnet.updControl("Fanout/tline/Series/tline_b1/Biquad/biquad2/mrs_real/resonance", marsyas.MarControlPtr.from_real(3.0))

pnet.updControl("Biquad/biquad/mrs_real/frequency", marsyas.MarControlPtr.from_real(2000.0))
pnet.updControl("Biquad/biquad/mrs_real/resonance", marsyas.MarControlPtr.from_real(12.0))
pnet.updControl("Biquad/biquad/mrs_string/type", marsyas.MarControlPtr.from_string("bandpass"))

pnet.updControl("Biquad/hpf1/mrs_real/frequency", marsyas.MarControlPtr.from_real(90.0))
pnet.updControl("Biquad/hpf1/mrs_string/type", marsyas.MarControlPtr.from_string("highpass"))
pnet.updControl("Biquad/hpf1/mrs_real/frequency", marsyas.MarControlPtr.from_real(90.0))
pnet.updControl("Biquad/hpf1/mrs_string/type", marsyas.MarControlPtr.from_string("highpass"))
pnet.updControl("Gain/gain/mrs_real/gain", marsyas.MarControlPtr.from_real(2.0))


while(1):
	pnet.tick()

# This is not anymore the end of the tutorial, as the continuation - twosinusoids.py - was added.


