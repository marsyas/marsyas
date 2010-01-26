# MarSystemComposite
# Type = Series
# Name = playbacknet

# MarControls = 8
# /Series/playbacknet/mrs_bool/debug = 0
# /Series/playbacknet/mrs_bool/mute = 0
# /Series/playbacknet/mrs_natural/inObservations = 1
# /Series/playbacknet/mrs_natural/inSamples = 512
# /Series/playbacknet/mrs_natural/onObservations = 1
# /Series/playbacknet/mrs_natural/onSamples = 512
# /Series/playbacknet/mrs_real/israte = 22050
# /Series/playbacknet/mrs_real/osrate = 22050

# Number of links = 6
# Synonyms of /Series/playbacknet/mrs_bool/mute = 
# Number of synonyms = 1
# /Series/playbacknet/Gain/gt/mrs_bool/mute
# Synonyms of /Series/playbacknet/mrs_bool/hasData = 
# Number of synonyms = 1
# /Series/playbacknet/SoundFileSource/src/mrs_bool/hasData
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

# MarControls = 17
# /SoundFileSource/src/mrs_bool/debug = 0
# /SoundFileSource/src/mrs_bool/mute = 0
# /SoundFileSource/src/mrs_bool/hasData = 0
# /SoundFileSource/src/mrs_bool/noteon = 0
# /SoundFileSource/src/mrs_natural/inObservations = 1
# /SoundFileSource/src/mrs_natural/inSamples = 512
# /SoundFileSource/src/mrs_natural/nChannels = 1
# /SoundFileSource/src/mrs_natural/onObservations = 1
# /SoundFileSource/src/mrs_natural/onSamples = 512
# /SoundFileSource/src/mrs_natural/pos = 0
# /SoundFileSource/src/mrs_natural/size = 661500
# /SoundFileSource/src/mrs_real/duration = -1
# /SoundFileSource/src/mrs_real/frequency = 0
# /SoundFileSource/src/mrs_real/israte = 22050
# /SoundFileSource/src/mrs_real/osrate = 22050
# /SoundFileSource/src/mrs_real/repetitions = 1
# /SoundFileSource/src/mrs_string/filename = /home/gtzan/data/sound/music_speech/music/gravity.au

# Number of links = 0

# MarSystem
# Type = Gain
# Name = gt

# MarControls = 9
# /Gain/gt/mrs_bool/debug = 0
# /Gain/gt/mrs_bool/mute = 0
# /Gain/gt/mrs_natural/inObservations = 1
# /Gain/gt/mrs_natural/inSamples = 512
# /Gain/gt/mrs_natural/onObservations = 1
# /Gain/gt/mrs_natural/onSamples = 512
# /Gain/gt/mrs_real/gain = 1
# /Gain/gt/mrs_real/israte = 22050
# /Gain/gt/mrs_real/osrate = 22050

# Number of links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 10
# /AudioSink/dest/mrs_bool/debug = 0
# /AudioSink/dest/mrs_bool/init = 0
# /AudioSink/dest/mrs_bool/mute = 0
# /AudioSink/dest/mrs_natural/inObservations = 1
# /AudioSink/dest/mrs_natural/inSamples = 512
# /AudioSink/dest/mrs_natural/nChannels = 1
# /AudioSink/dest/mrs_natural/onObservations = 1
# /AudioSink/dest/mrs_natural/onSamples = 512
# /AudioSink/dest/mrs_real/israte = 22050
# /AudioSink/dest/mrs_real/osrate = 22050

# Number of links = 0


