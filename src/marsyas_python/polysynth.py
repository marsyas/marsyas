from marsyas import *
from marsyas_util import create
from collections import deque


def main():
    synth = Synth()

    last_tup = (0,0,0)
    while(True):
        msg = synth.get_midi_bytes()
        if msg != last_tup:
            # print msg
            (noteon, note, velo) = msg
            if noteon == 144:
                synth.noteon(note)
            if noteon in (128, 127) or velo == 0:
                synth.noteoff(note)
            if noteon == 176:
                synth.ffreq(velo/127.0)
            last_tup = msg
        synth()

class Synth(object):
    def __init__(self, voices=8):
        self.sample_rate = 44100.0

        self.playing = deque()
        self.off = deque([Osc(sample_rate=self.sample_rate) for _ in range(voices)])

        osc_bank = ["Fanout/bank", [osc.get_network() for osc in self.off]]
        net = ["Series/fmnet", [osc_bank,"Sum/sum","Gain/ogain","SVFilter/postmixfilter","MidiInput/midi","AudioSink/dest","SoundFileSink/dest2"]]

        # Create network and intialize parameter mapping
        self.network = create(net)

        self.network.updControl("Gain/ogain/mrs_real/gain", 0.4)

        # Output settings
        self.network.updControl("mrs_real/israte", self.sample_rate)
        self.network.updControl("AudioSink/dest/mrs_bool/initAudio", MarControlPtr.from_bool(True))
        self.network.updControl("SoundFileSink/dest2/mrs_string/filename", "synthout.wav")

        # Midi settings
        self.network.updControl("MidiInput/midi/mrs_bool/initmidi", MarControlPtr.from_bool(True))
        self.network.updControl("MidiInput/midi/mrs_natural/port", 0)

    def get_midi_bytes(self):
        return (self.network.getControl("MidiInput/midi/mrs_natural/byte1").to_natural(),
                self.network.getControl("MidiInput/midi/mrs_natural/byte2").to_natural(),
                self.network.getControl("MidiInput/midi/mrs_natural/byte3").to_natural())

    def ffreq(self, val):
        self.network.updControl("SVFilter/postmixfilter/mrs_real/frequency", val*(self.sample_rate/2.0))

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

class Osc(object):
    _num_of_oscs = 0

    def __init__(self, osc_type="BlitOsc/bl", sample_rate = 44100.0, in_samples=512):
        self.osc_type = osc_type
        osc_filter_ctrls = ["Fanout/fout", [self.osc_type,"ADSR/fadsr"]]
        self.spec = lambda i: ["Series/osc%d" % i, [osc_filter_ctrls,"SVFilter/filter","ADSR/gadsr","Gain/gain"]]

        self.osc = self._new_osc()
        self.osc.updControl("mrs_real/israte", sample_rate)
        self.osc.updControl("mrs_natural/inSamples", in_samples);

        self.noteon_ctrl = self.osc.getControl("ADSR/gadsr/mrs_bool/noteon")
        self.noteoff_ctrl = self.osc.getControl("ADSR/gadsr/mrs_bool/noteoff")

        self.osc.updControl("SVFilter/filter/mrs_bool/freqin", MarControlPtr.from_bool(True))
        self.osc.updControl("SVFilter/filter/mrs_real/res", 0.6)
        self.osc.updControl("SVFilter/filter/mrs_natural/type", 0)

        self.osc.updControl("Fanout/fout/ADSR/fadsr/mrs_bool/bypass", MarControlPtr.from_bool(True))
        self.osc.updControl("Fanout/fout/ADSR/fadsr/mrs_real/aTarget", 0.2)
        self.osc.updControl("Fanout/fout/ADSR/fadsr/mrs_real/susLevel", 0.08)
        self.osc.updControl("Fanout/fout/ADSR/fadsr/mrs_real/aTime", 1.6)

        self.fnoteon_ctrl = self.osc.getControl("Fanout/fout/ADSR/fadsr/mrs_bool/noteon")
        self.fnoteoff_ctrl = self.osc.getControl("Fanout/fout/ADSR/fadsr/mrs_bool/noteoff")

        self.freq_ctrl = self.osc.getControl("Fanout/fout/" + osc_type + "/mrs_real/frequency")
        self.osc.updControl("Fanout/fout/" + osc_type + "/mrs_bool/noteon", MarControlPtr.from_bool(True))
        self.osc.updControl("Fanout/fout/" + osc_type + "/mrs_natural/type", 1)

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
        self.fnoteon_ctrl.setValue_bool(True)

    def noteoff(self):
        self.num = None
        self.noteoff_ctrl.setValue_bool(True)
        self.fnoteoff_ctrl.setValue_bool(True)

    def get_network(self):
        return self.osc

def midi2freq(num):
    return 440.0 * pow(2.0,((num-69)/12.0))

if __name__ == "__main__":
    main()
