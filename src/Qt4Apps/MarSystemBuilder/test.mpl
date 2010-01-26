# MarSystemComposite
# Type = Series
# Name = playbacknet

# MarControls = 12
# /Series/playbacknet/bool/debug = 0
# /Series/playbacknet/bool/mute = 0
# /Series/playbacknet/bool/probe = 0
# /Series/playbacknet/natural/inObservations = 1
# /Series/playbacknet/natural/inSamples = 512
# /Series/playbacknet/natural/onObservations = 1
# /Series/playbacknet/natural/onSamples = 512
# /Series/playbacknet/real/israte = 22050
# /Series/playbacknet/real/osrate = 22050
# /Series/playbacknet/realvec/input = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/playbacknet/string/inObsNames = audio,
# /Series/playbacknet/string/onObsNames = audio,

# Number of links = 6
# Synonyms of /Series/playbacknet/bool/mute = 
# Number of synonyms = 1
# /Series/playbacknet/Gain/gt/bool/mute
# Synonyms of /Series/playbacknet/bool/hasData = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/bool/hasData
# Synonyms of /Series/playbacknet/natural/nChannels = 
# Number of synonyms = 2
# /Series/playbacknet/SoundFileSource/src/natural/nChannels
# /Series/playbacknet/AudioSink/dest/natural/nChannels
# Synonyms of /Series/playbacknet/natural/pos = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/natural/pos
# Synonyms of /Series/playbacknet/real/israte = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/real/israte
# Synonyms of /Series/playbacknet/string/filename = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/string/filename

# nComponents = 3

# MarSystem
# Type = SoundFileSource
# Name = src

# MarControls = 23
# /SoundFileSource/src/bool/advance = 0
# /SoundFileSource/src/bool/debug = 0
# /SoundFileSource/src/bool/mute = 0
# /SoundFileSource/src/bool/hasData = 0
# /SoundFileSource/src/bool/noteon = 0
# /SoundFileSource/src/natural/inObservations = 1
# /SoundFileSource/src/natural/inSamples = 512
# /SoundFileSource/src/natural/loopPos = 0
# /SoundFileSource/src/natural/nChannels = 1
# /SoundFileSource/src/natural/numFiles = 0
# /SoundFileSource/src/natural/onObservations = 1
# /SoundFileSource/src/natural/onSamples = 512
# /SoundFileSource/src/natural/pos = 0
# /SoundFileSource/src/natural/size = 661500
# /SoundFileSource/src/real/duration = -1
# /SoundFileSource/src/real/frequency = 0
# /SoundFileSource/src/real/israte = 22050
# /SoundFileSource/src/real/osrate = 22050
# /SoundFileSource/src/real/repetitions = 1
# /SoundFileSource/src/string/allfilenames = ,
# /SoundFileSource/src/string/filename = defaultfile
# /SoundFileSource/src/string/inObsNames = audio,
# /SoundFileSource/src/string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = Gain
# Name = gt

# MarControls = 11
# /Gain/gt/bool/debug = 0
# /Gain/gt/bool/mute = 0
# /Gain/gt/natural/inObservations = 1
# /Gain/gt/natural/inSamples = 512
# /Gain/gt/natural/onObservations = 1
# /Gain/gt/natural/onSamples = 512
# /Gain/gt/real/gain = 1
# /Gain/gt/real/israte = 22050
# /Gain/gt/real/osrate = 22050
# /Gain/gt/string/inObsNames = audio,
# /Gain/gt/string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 13
# /AudioSink/dest/bool/debug = 0
# /AudioSink/dest/bool/init = 0
# /AudioSink/dest/bool/mute = 0
# /AudioSink/dest/natural/bufferSize = 512
# /AudioSink/dest/natural/inObservations = 1
# /AudioSink/dest/natural/inSamples = 512
# /AudioSink/dest/natural/nChannels = 1
# /AudioSink/dest/natural/onObservations = 1
# /AudioSink/dest/natural/onSamples = 512
# /AudioSink/dest/real/israte = 22050
# /AudioSink/dest/real/osrate = 22050
# /AudioSink/dest/string/inObsNames = audio,
# /AudioSink/dest/string/onObsNames = audio,

# Number of links = 0

