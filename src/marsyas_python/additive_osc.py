from marsyas_util import create
from play_melody import *


def main():
    gen = ["Series/fmnet", ["AdditiveOsc/additve","ADSR/adsr","Gain/gain","SoundFileSink/dest2"]]

# Create network and intialize parameter mapping 
    network = create(gen)

    network.updControl("ADSR/adsr/mrs_real/aTime", 0.1)
    network.updControl("Gain/gain/mrs_real/gain", 0.8)

# These mapping are to make the system work with play melody
    network.linkControl("ADSR/adsr/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("ADSR/adsr/mrs_bool/noteoff", "mrs_bool/noteoff")
    network.linkControl("AdditiveOsc/additve/mrs_real/frequency", "mrs_real/frequency")

# Set the systems sample rate
    sample_rate = 44100.0
    network.updControl( "mrs_real/israte", sample_rate)

# Set up Audio File
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "AdditiveTestSaw.wav")
    play_melody(network)


if __name__ == "__main__":
    main()
