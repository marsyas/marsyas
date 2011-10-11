#!/usr/bin/env python

# HelloWorld, Marsyas!

# This is a HelloWorld tutorial in Marsyas.
# It is the first part of the source-code python tutorial to marsyas.
# It should help the newbie user to understand the basic marsyas stuff.
#
#
# The following people have changed this text / source code:
# Tiago Fernandes Tavares - tiagoft [at] gmail [dot] com

# We need to import the marsyas library
from marsyas import * # (I used this form because I don't want to type marsyas.stuff all the time)
# If this part does not work, remember to compile marsyas with swig and remember to 'make install' after you have compiled.
# If it still does not work... then, if you happen to figure out what is happening, tell us!

# All apps made using Marsyas are called MarSystems.
# We will need a marsyas system manager, to begin with, so, lets do:
mng = MarSystemManager()

# All data flow will be stored in an object whose class is MarSystem. It will allow you to link thinks and that sort of thing.
# The data flow is often referred to as a 'network'.
# The parameters needed are the type of the network, which may be either 'Series' or 'Paralel',
# and the name of the network, which is the label of your choice
# MarSystems are created by the MarSystemManager object.
nType = "Series"
nName = "Julia Roberts" # This can be whatever you want, as long as you remember it
nwk = mng.create(nType, nName);

# The next line comes from the cpp source code line:
# network->addMarSystem(mng.create("SineSource", "src"));
# Which means:
# We will add a marsystem to our network variable.
# Note that msn (the MarSystemManager) is the object responsible for spawning information about the objects we are creating.
# The 'create' method will receive two strings: a type and a name. In fact, it outputs a MarSystem, so its the same logic as above.
nwk.addMarSystem(mng.create("SineSource", "StartHere"));

# The types of MarSystems that may be created are described in the documentation.
# Now we have an input. We need to add the output as well:
nwk.addMarSystem(mng.create("AudioSink", "EndHere"));

# Marsyas supposes you want to process audio things in frames of known length, which is rather common when we deal with audio.
# In this next line, we will use the updControl method, which means "update controls", and 'controls'
# are the execution parameters of our thing.
# The first parameter tells the system (using mrs_natural) that our data type is a natural number,
# and the /inSample tells us that that is meant to be in samples. The second parameter is the data itself. Hence: 
nwk.updControl("mrs_natural/inSamples", 256)
# means that each frame of our system will have 256 samples.

# Interestingly, as you probably have figured out on your own, MarSystems may be recursively defined as:
# a MarSystem is a basic block
# a MarSystem is a network of MarSystems
#
# Now, we will have to setup the frequency of our sinusoidal source, which we added above.
# The following line means:
# 'Set the parameter for the network's component SineSource named StartHere using a real frequency value of 440.0'
nwk.updControl("SineSource/StartHere/mrs_real/frequency", 440.0);

# Also, we will setup parameters for our audiosink, which are the bufferSize
# for that sink (that is, how many samples are going to be stored in the buffer
# before sending to the DA converter - we pick 512, but half our double that should work
# as well - and the initAudio parameter, which is 
nwk.updControl("AudioSink/EndHere/mrs_natural/bufferSize", 512); 

# We will have to update the sample rate for the network. Marsyas defaults to 22050, but some sound cards 
# (like mine) just don't 
nwk.updControl("mrs_real/israte", 44100.0);
nwk.updControl("AudioSink/EndHere/mrs_bool/initAudio", MarControlPtr.from_bool(True));

# Will I be able to print the network?
print nwk.toString()

# Everything seems to be set correctly.
# Now, we must make time pass.
# Every time we call a nwk.tick(), time will pass for that MarSystem by a time equivalent to the size of the network's buffer.
# (in our case, 256?)
while 1:
	nwk.tick();

# And this program will quit when we hit ctrl-c!
#
#
# When you are finished playing with this, go to windowing.py to continue the tutorial.


