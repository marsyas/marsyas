
# How to create a FIR filter using Marsyas and Python
# In this advanced tutorial, we will create a FIR filter in the frequency domain.
# For you to understand this tutorial, you should have the following skills:
# - You can build simple Marsyas networks for audio reproduction
# - You understand what a DFT is
# - You understand why the Overlap-and-Add technique is important
# If you don't have these abilities, then you will be able to reproduce the system, but you will probably not understand why it works
# The tutorial is basically a demonstration of the Overlap-and-Add in the context of a Marsyas network.
# In this tutorial, the following MarSystems will be discussed:
# ShiftInput
# Windowing
# Spectrum
# Transposer
# Product
# InvSpectrum
# OverlapAdd
#
# Also, we will learn how to set controls using vectors as parameters
#
# We will begin by importing the modules:

import marsyas
import marsyas_util

# Now up to business.
# You remember that a convolution in the time domain may be represented by a multiplication in the frequency domain, right?
# That allows us to apply finite impulse response (FIR) filters in the frequency domain with a fast multiplication, instead of having to calculate a slow convolution.
# In fact, this technique also allows us to design filters which are stable and have a fairly good cutoff frequency.
# Also, you could draw some weird shape in the frequency domain (maybe a multiple-band-pass filter?) and it would work
# Of course, there are some limits on the shapes and frequency responses you can give to your filter, and if you really want to get into that
# you should refer to a textbook like A. Oppenhein's 'Digital Signal Processing' or even some open access toolbox (google it!)
#
# The first MarSystem that we will discuss is the ShiftInput.
# It works by receiving N samples, and storing them in a M-sample buffer.
# The system, then, yields M samples to the output.
# That means that, in each tick of the network, a ShiftInput yields M samples,
# and M-N of them overlap with the samples yielded in the previous tick.
# We will use a ShiftInput to get overlapping frames of audio from our input.
#
# After that, we will want some windowing to happen.
# In fact, we will need a Hanning window because we are using the Overlap-and-Add (OLA) technique.
# In brief, OLA (as most of the frequency-domain processing) will give you weird signals near frame edges.
# The Hanning window has two interesting properties:
# - It will attenuate the edges of the frames
# - When we sum two Hanning windows with 50% overlap, we obtain a constant signal
# Hence, if we have Hanning windows with 50% overlap we keep the desired signal while attenuating the undesired parts.
#
# The Spectrum MarSystem calculates the DFT of the input signal
# This implementation of the DFT will yield, for each signal row, a frequency-domain column.
# (if you are reading sound files, each row is a channel)
# Each frequency-domain column will have (N/2)+1 complex numbers, and each complex number will be represented
# by two real numbers, giving the real and imaginary parts (hence, o(n*2) and o(1+(n*2)) represent the real and
# imaginary parts of a DFT bin)
#
# The InvSpectrum gets something that is yielded by a spectrum and converts it back to the time-domain
# (A network consisting of ->Spectrum->InvSpectrum-> does nothing)
#
# The Product MarSystem combines all rows of a frame using multiplication. It also has a 'mask' control that
# allows us to have an 'external' fixed array of coefficients for multiplication.
# This what the filtering process will do: we will get a Spectrum, use Product to change the values of the DFT bins
# and then calculate the InvSpectrum.
#
# Unfortunately, Product works column-wise over a frame, while our multiplication should be row-wise to comply with the Spectrum
# output. To solve it, we use the Transpose MarSystem.
#
# Last, all we need to do is implement the OLA process, which is already done by the OverlapAdd MarSystem.
#
# So, we have this system:

fir_filtering = ["Series/fir", ["SoundFileSource/src", "ShiftInput/sft", "Windowing/win", "Spectrum/spk", "Transposer/tp1", "Product/prod", "Transposer/tp2", "InvSpectrum/ispk", "OverlapAdd/ola", "AudioSink/asnk"]]

# The following lines should be really self-explanatory
net = marsyas_util.create(fir_filtering)
snet = marsyas_util.mar_refs(fir_filtering)

net.updControl("mrs_natural/inSamples", 1024)
net.updControl(snet["sft"]+"/mrs_natural/winSize", 2048)
net.updControl(snet["win"]+"/mrs_string/type", "Hanning" )
net.updControl(snet['src']+"/mrs_string/filename", "input.wav")
net.updControl(snet['asnk']+"/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))

# ... but these lines here are new.
# We will operate the product and enable the use of a fixed mask:
net.updControl(snet['prod']+"/mrs_bool/use_mask", marsyas.MarControlPtr.from_bool(True))

# Now, we will define the shape of our FIR filter in the frequency domain.
# This is a low-pass filter.
vec = marsyas.realvec(2048+2)
for i in xrange (len(vec)/2):
    if i>30:
        vec[2*i]=0
        vec[1+(2*i)]=0
    else:
        vec[2*i]=1
        vec[1+(2*i)]=1

# Last, we set the mask to be our desired value.
net.updControl(snet['prod']+"/mrs_realvec/mask", marsyas.MarControlPtr.from_realvec(vec))

# Let's run and listen to the results!
while 1:
        net.tick()

# In our implementation, the FIR filter can be bypassed just by setting:
# net.updControl(snet['prod']+"/mrs_bool/use_mask", marsyas.MarControlPtr.from_bool(True))
# (I am 100% sure that this will be useful at some point in the future!)

# Here are some exercises for you who are taking this tutorial:
# Can you calculate the cutoff frequency of the filter we implemented?
# How would you change it to a high-pass filter instead of a low-pass one? What about a band-pass?
# What changes can you observe in your input signal when you bypass the FIR filter?
