from marsyas import *
from marsyas_util import create
from play_melody import *


def main():
    mod = ["Fanout/fo", ["ADSR/pitch",
                         "ADSR/pwm"]]
    osc = ["Series/osc",[mod, "AliasingOsc/osc"]]
    gen = ["Series/fmnet",[osc, "ADSR/adsr", "Gain/gain", "SoundFileSink/dest2"]]

# Create network and intialize parameter mapping 
    network = create(gen)
# Set the systems sample rate
    sample_rate = 44100.0
    network.updControl( "mrs_real/israte", sample_rate)

    network.updControl("Series/osc/Fanout/fo/ADSR/pwm/mrs_real/aTime", 0.7)
    network.updControl("Gain/gain/mrs_real/gain", 0.7)

    network.updControl("ADSR/adsr/mrs_real/aTime", 0.1)
    network.updControl("Gain/gain/mrs_real/gain", 0.8)

# These mapping are to make the system work with play melody
    network.linkControl("ADSR/adsr/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("ADSR/adsr/mrs_bool/noteoff", "mrs_bool/noteoff")

    network.linkControl("Series/osc/Fanout/fo/ADSR/pwm/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("Series/osc/Fanout/fo/ADSR/pwm/mrs_bool/noteoff", "mrs_bool/noteoff")

    network.linkControl("Series/osc/Fanout/fo/ADSR/pitch/mrs_bool/noteon", "mrs_bool/noteon")
    network.linkControl("Series/osc/Fanout/fo/ADSR/pitch/mrs_bool/noteoff", "mrs_bool/noteoff")

    network.linkControl("Series/osc/AliasingOsc/osc/mrs_real/frequency", "mrs_real/frequency")

# PWM Example
    network.updControl("Series/osc/AliasingOsc/osc/mrs_natural/type", 1)
    network.updControl("Series/osc/Fanout/fo/ADSR/pwm/mrs_bool/bypass", MarControlPtr.from_bool(True))
    network.updControl("Series/osc/AliasingOsc/osc/mrs_bool/cyclicin", MarControlPtr.from_bool(True))
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "AliasingTestPWM.wav")
    play_melody(network)

# Saw Wave example
    network.updControl("Series/osc/AliasingOsc/osc/mrs_natural/type", 0)
    network.updControl("Series/osc/Fanout/fo/ADSR/pwm/mrs_bool/bypass", MarControlPtr.from_bool(False))
    network.updControl("Series/osc/AliasingOsc/osc/mrs_bool/cyclicin", MarControlPtr.from_bool(False))
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "AliasingTestSaw.wav")
    play_melody(network)

# Pitch modulation example
    network.updControl("Series/osc/Fanout/fo/ADSR/pitch/mrs_bool/bypass", MarControlPtr.from_bool(True))
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "AliasingTestPitch.wav")
    play_melody(network)
    network.updControl("Series/osc/Fanout/fo/ADSR/pitch/mrs_bool/bypass", MarControlPtr.from_bool(False))

# Square Wave example
    network.updControl("Series/osc/AliasingOsc/osc/mrs_natural/type", 1)
    network.updControl("SoundFileSink/dest2/mrs_string/filename", "AliasingTestSquare.wav")
    play_melody(network)



if __name__ == "__main__":
    main()
