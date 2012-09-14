from marsyas import *
from marsyas_util import create
from play_melody import *


def main():
    gen = ["Series/fmnet", ["ADSR/pitch","WaveguideOsc/waveguide","ADSR/adsr","Gain/gain","SoundFileSink/dest2"]]

# Create network and intialize parameter mapping 
    network = create(gen)

    network.updControl("ADSR/adsr/mrs_real/aTime", 0.1)
    network.updControl("Gain/gain/mrs_real/gain", 0.6)

# These mapping are to make the system work with play melody
    network.linkControl("ADSR/adsr/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("ADSR/adsr/mrs_bool/noteoff", "mrs_bool/noteoff")

    network.linkControl("ADSR/pitch/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("ADSR/pitch/mrs_bool/noteoff", "mrs_bool/noteoff")

    network.linkControl("WaveguideOsc/waveguide/mrs_real/frequency", "mrs_real/frequency")

# Set the systems sample rate
    sample_rate = 44100.0
    network.updControl( "mrs_real/israte", sample_rate)

# Set up Audio File
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "WaveguideTest.wav")
    play_melody(network)

    network.updControl("ADSR/pitch/mrs_bool/bypass", MarControlPtr.from_bool(True))
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "WaveguideTestPitch.wav")
    play_melody(network)


if __name__ == "__main__":
    main()
