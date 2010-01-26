# MarSystemComposite
# Type = Series
# Name = playbacknet

# MarControls = 19
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/hasData = 0
# Links = 1
# /Series/playbacknet/SoundFileSource/src/mrs_bool/hasData
# mrs_bool/probe = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 100
# Links = 0
# mrs_natural/loopPos = 0
# Links = 1
# /Series/playbacknet/SoundFileSource/src/mrs_natural/loopPos
# mrs_natural/nChannels = 1
# Links = 2
# /Series/playbacknet/SoundFileSource/src/mrs_natural/nChannels
# /Series/playbacknet/AudioSink/dest/mrs_natural/nChannels
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 100
# Links = 0
# mrs_natural/pos = 661500
# Links = 1
# /Series/playbacknet/SoundFileSource/src/mrs_natural/pos
# mrs_real/israte = 22050
# Links = 1
# /Series/playbacknet/SoundFileSource/src/mrs_real/israte
# mrs_real/osrate = 22050
# Links = 0
# mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input1 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/filename = /home/gtzan/data/sound/music_speech/music/guitar.au
# Links = 1
# /Series/playbacknet/SoundFileSource/src/mrs_string/filename
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = audio,
# Links = 0

# nComponents = 3

# MarSystem
# Type = SoundFileSource
# Name = src

# MarControls = 27
# mrs_bool/active = 1
# Links = 0
# mrs_bool/advance = 0
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/hasData = 0
# Links = 1
# /Series/playbacknet/mrs_bool/hasData
# mrs_bool/noteon = 0
# Links = 0
# mrs_bool/shuffle = 0
# Links = 0
# mrs_natural/cindex = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 100
# Links = 0
# mrs_natural/loopPos = 0
# Links = 1
# /Series/playbacknet/mrs_natural/loopPos
# mrs_natural/nChannels = 1
# Links = 1
# /Series/playbacknet/mrs_natural/nChannels
# mrs_natural/numFiles = 1
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 100
# Links = 0
# mrs_natural/pos = 661500
# Links = 1
# /Series/playbacknet/mrs_natural/pos
# mrs_natural/size = 661500
# Links = 0
# mrs_real/duration = -1
# Links = 0
# mrs_real/frequency = 0
# Links = 0
# mrs_real/israte = 22050
# Links = 1
# /Series/playbacknet/mrs_real/israte
# mrs_real/osrate = 22050
# Links = 0
# mrs_real/repetitions = 1
# Links = 0
# mrs_string/allfilenames = ,
# Links = 0
# mrs_string/currentlyPlaying = daufile
# Links = 0
# mrs_string/filename = /home/gtzan/data/sound/music_speech/music/guitar.au
# Links = 1
# /Series/playbacknet/mrs_string/filename
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = audio,
# Links = 0

# MarSystem
# Type = Gain
# Name = gt

# MarControls = 15
# mrs_bool/RMScalc = 0
# Links = 0
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 100
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 100
# Links = 0
# mrs_real/gain = 1
# Links = 0
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 22050
# Links = 0
# mrs_realvec/inRMS = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/outRMS = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = audio,
# Links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/bufferSize = 256
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 100
# Links = 0
# mrs_natural/nChannels = 1
# Links = 1
# /Series/playbacknet/mrs_natural/nChannels
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 100
# Links = 0
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 22050
# Links = 0
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = audio,
# Links = 0


