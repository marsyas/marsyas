# MarSystemComposite
# Type = Series
# Name = playbacknet

# MarControls = 14
# /Series/playbacknet/mrs_bool/active = 1
# /Series/playbacknet/mrs_bool/debug = 0
# /Series/playbacknet/mrs_bool/mute = 0
# /Series/playbacknet/mrs_bool/probe = 0
# /Series/playbacknet/mrs_natural/inObservations = 1
# /Series/playbacknet/mrs_natural/inSamples = 100
# /Series/playbacknet/mrs_natural/onObservations = 1
# /Series/playbacknet/mrs_natural/onSamples = 100
# /Series/playbacknet/mrs_real/israte = 22050
# /Series/playbacknet/mrs_real/osrate = 22050
# /Series/playbacknet/mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/playbacknet/mrs_realvec/input1 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/playbacknet/mrs_string/inObsNames = audio,
# /Series/playbacknet/mrs_string/onObsNames = audio,

# Number of links = 6
# Synonyms of /Series/playbacknet/mrs_bool/notEmpty = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/mrs_bool/notEmpty
# Synonyms of /Series/playbacknet/mrs_natural/loopPos = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/mrs_natural/loopPos
# Synonyms of /Series/playbacknet/mrs_natural/nChannels = 
# Number of synonyms = 2
# /Series/playbacknet/SoundFileSource/src/mrs_natural/nChannels
# /Series/playbacknet/AudioSink/dest/mrs_natural/nChannels
# Synonyms of /Series/playbacknet/mrs_natural/pos = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/mrs_natural/pos
# Synonyms of /Series/playbacknet/mrs_real/israte = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/mrs_real/israte
# Synonyms of /Series/playbacknet/mrs_string/filename = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/mrs_string/filename

# nComponents = 3

# MarSystem
# Type = SoundFileSource
# Name = src

# MarControls = 27
# /SoundFileSource/src/mrs_bool/active = 1
# /SoundFileSource/src/mrs_bool/advance = 0
# /SoundFileSource/src/mrs_bool/debug = 0
# /SoundFileSource/src/mrs_bool/mute = 0
# /SoundFileSource/src/mrs_bool/notEmpty = 0
# /SoundFileSource/src/mrs_bool/noteon = 0
# /SoundFileSource/src/mrs_bool/shuffle = 0
# /SoundFileSource/src/mrs_natural/cindex = 0
# /SoundFileSource/src/mrs_natural/inObservations = 1
# /SoundFileSource/src/mrs_natural/inSamples = 100
# /SoundFileSource/src/mrs_natural/loopPos = 0
# /SoundFileSource/src/mrs_natural/nChannels = 1
# /SoundFileSource/src/mrs_natural/numFiles = 1
# /SoundFileSource/src/mrs_natural/onObservations = 1
# /SoundFileSource/src/mrs_natural/onSamples = 100
# /SoundFileSource/src/mrs_natural/pos = 661500
# /SoundFileSource/src/mrs_natural/size = 661500
# /SoundFileSource/src/mrs_real/duration = -1
# /SoundFileSource/src/mrs_real/frequency = 0
# /SoundFileSource/src/mrs_real/israte = 22050
# /SoundFileSource/src/mrs_real/osrate = 22050
# /SoundFileSource/src/mrs_real/repetitions = 1
# /SoundFileSource/src/mrs_string/allfilenames = ,
# /SoundFileSource/src/mrs_string/currentlyPlaying = daufile
# /SoundFileSource/src/mrs_string/filename = /home/gtzan/data/sound/music_speech/music/bmarsalis.au
# /SoundFileSource/src/mrs_string/inObsNames = audio,
# /SoundFileSource/src/mrs_string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = Gain
# Name = gt

# MarControls = 12
# /Gain/gt/mrs_bool/active = 1
# /Gain/gt/mrs_bool/debug = 0
# /Gain/gt/mrs_bool/mute = 0
# /Gain/gt/mrs_natural/inObservations = 1
# /Gain/gt/mrs_natural/inSamples = 100
# /Gain/gt/mrs_natural/onObservations = 1
# /Gain/gt/mrs_natural/onSamples = 100
# /Gain/gt/mrs_real/gain = 1
# /Gain/gt/mrs_real/israte = 22050
# /Gain/gt/mrs_real/osrate = 22050
# /Gain/gt/mrs_string/inObsNames = audio,
# /Gain/gt/mrs_string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 14
# /AudioSink/dest/mrs_bool/active = 1
# /AudioSink/dest/mrs_bool/debug = 0
# /AudioSink/dest/mrs_bool/init = 0
# /AudioSink/dest/mrs_bool/mute = 0
# /AudioSink/dest/mrs_natural/bufferSize = 256
# /AudioSink/dest/mrs_natural/inObservations = 1
# /AudioSink/dest/mrs_natural/inSamples = 100
# /AudioSink/dest/mrs_natural/nChannels = 1
# /AudioSink/dest/mrs_natural/onObservations = 1
# /AudioSink/dest/mrs_natural/onSamples = 100
# /AudioSink/dest/mrs_real/israte = 22050
# /AudioSink/dest/mrs_real/osrate = 22050
# /AudioSink/dest/mrs_string/inObsNames = audio,
# /AudioSink/dest/mrs_string/onObsNames = audio,

# Number of links = 0


