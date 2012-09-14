from marsyas import *
from marsyas_util import create
from play_melody import *


def main():
    gen = ["Series/fmnet", ["Plucked/plucked","Gain/gain","SoundFileSink/dest2"]]

# Create network and intialize parameter mapping 
    network = create(gen)

    network.updControl("Gain/gain/mrs_real/gain", 0.8)

# These mapping are to make the system work with play melody
    network.linkControl("Plucked/plucked/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("Plucked/plucked/mrs_real/frequency", "mrs_real/frequency")

# Set the systems sample rate
    sample_rate = 44100.0
    network.updControl( "mrs_real/israte", sample_rate)

# Set up Audio File
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "PluckedTest.wav")
    play_melody(network)

if __name__ == "__main__":
    main()
