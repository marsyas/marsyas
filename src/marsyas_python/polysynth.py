from marsyas import *
from marsyas_util import create
from collections import deque

from pygame.midi import *

def main():
    init()
    poll = Input.poll
    read = Input.read
    midi = Input(0)

    synth = Synth()

    while(True):
        if poll(midi):
            # format [[[128,  49 , 127,   0], 19633]]
            #           cc#| note| vel| n/a
            msg = read(midi, 1)
            note = msg[0][0][1]
            noteon = msg[0][0][0]
            if noteon == 144 or noteon == 145:
                synth.noteon(note)
            elif noteon == 128 or noteon == 129:
                synth.noteoff(note)
        synth()

class Synth:
    def __init__(self, voices=8):
        sample_rate = 44100.0

        self.playing = deque()
        self.off = deque([Osc(sample_rate=sample_rate) for _ in range(voices)])

        osc_bank = ["Fanout/bank", [osc.get_network() for osc in self.off]]
        net = ["Series/fmnet", [osc_bank,"Sum/sum", "AudioSink/dest"]]

        # Create network and intialize parameter mapping 
        self.network = create(net)

        # Output settings
        self.network.updControl("mrs_real/israte", sample_rate)
        self.network.updControl("AudioSink/dest/mrs_bool/initAudio", MarControlPtr.from_bool(True))

    def noteon(self, num):
        if self.off:
            osc = self.off.pop()
            osc.noteon(num)
            self.playing.appendleft(osc)
        elif self.playing:
            osc = self.playing.pop()
            osc.noteon(num)
            self.playing.appendleft(osc)

    def noteoff(self, num):
        for osc in self.playing:
            if osc.num == num:
                osc.noteoff()
                self.off.append(osc)

    def __call__(self):
        self.network.tick()

class Osc:
    _num_of_oscs = 0

    def __init__(self, osc_type="APDelayOsc/bl", sample_rate = 44100.0):
        self.osc_type = osc_type
        self.spec = lambda i: ["Series/osc%d" % i, [self.osc_type,"ADSR/adsr","Gain/gain"]]

        self.osc = self._new_osc()
        self.osc.updControl("mrs_real/israte", sample_rate)

        self.noteon_ctrl = self.osc.getControl("ADSR/adsr/mrs_bool/noteon")
        self.noteoff_ctrl = self.osc.getControl("ADSR/adsr/mrs_bool/noteoff")
        self.freq_ctrl = self.osc.getControl(osc_type + "/mrs_real/frequency")
        self.osc.updControl(osc_type + "/mrs_bool/noteon", MarControlPtr.from_bool(True))
        self.osc.updControl(osc_type + "/mrs_natural/type", 0)

        self.note = None
        self.num = None

    def _new_osc(self):
        osc = create(self.spec(Osc._num_of_oscs))
        Osc._num_of_oscs = Osc._num_of_oscs + 1
        return osc

    def noteon(self, num, pitch=None):
        if pitch:
            self.freq_ctrl.setValue_real(pitch)
        else:
            self.freq_ctrl.setValue_real(midi2freq(num))
        self.num = num
        self.noteon_ctrl.setValue_bool(True)

    def noteoff(self):
        self.num = None
        self.noteoff_ctrl.setValue_bool(True)

    def get_network(self):
        return self.osc

def midi2freq(num):
    return 3 * 440.0 * pow(2.0,((num-69)/12.0))

if __name__ == "__main__":
    main()
