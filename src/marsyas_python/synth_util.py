from marsyas import *

def play_melody(network, notes = None):
    if not notes:
        print "Using Default Melody"
        pitch = 440.0
        notes = [pitch, pitch * 2, (pitch * 3)/2.0, (pitch * 5)/3.0, pitch]

    bufferSize = network.getControl("mrs_natural/inSamples").to_natural()
    srate = network.getControl("mrs_real/osrate").to_real()
    tstep = bufferSize * 1.0 / srate

    for note in notes:
        time = 0.0
        nton = 'on'
        network.updControl("mrs_real/frequency", note)
        network.updControl("mrs_bool/noteon", MarControlPtr.from_bool(True))

        while time < 1.0:
            network.tick()

            if time > 0.7 and nton == 'on':
                network.updControl("mrs_bool/noteoff", MarControlPtr.from_bool(True))
                nton = 'off'
            time = time + tstep
