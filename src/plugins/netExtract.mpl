# MarSystemComposite
# Type = Series
# Name = extractNet

# MarControls = 8
# /Series/extractNet/bool/debug = 0
# /Series/extractNet/bool/mute = 0
# /Series/extractNet/natural/inObservations = 1
# /Series/extractNet/natural/inSamples = 512
# /Series/extractNet/natural/onObservations = 1
# /Series/extractNet/natural/onSamples = 1
# /Series/extractNet/real/israte = 22050
# /Series/extractNet/real/osrate = 43.0664

# Number of links = 1
# Synonyms of /Series/extractNet/bool/notEmpty = 
# Number of synonyms = 1
# /Series/extractNet/NetworkTCPSource/src/bool/notEmpty

# nComponents = 6

# MarSystem
# Type = NetworkTCPSource
# Name = src

# MarControls = 12
# /NetworkTCPSource/src/bool/debug = 0
# /NetworkTCPSource/src/bool/mute = 0
# /NetworkTCPSource/src/bool/notEmpty = 1
# /NetworkTCPSource/src/natural/controlsPort = 5010
# /NetworkTCPSource/src/natural/dataPort = 5009
# /NetworkTCPSource/src/natural/inObservations = 1
# /NetworkTCPSource/src/natural/inSamples = 512
# /NetworkTCPSource/src/natural/onObservations = 1
# /NetworkTCPSource/src/natural/onSamples = 512
# /NetworkTCPSource/src/real/gain = 1
# /NetworkTCPSource/src/real/israte = 22050
# /NetworkTCPSource/src/real/osrate = 22050

# Number of links = 0

# MarSystem
# Type = Hamming
# Name = ham

# MarControls = 8
# /Hamming/ham/bool/debug = 0
# /Hamming/ham/bool/mute = 0
# /Hamming/ham/natural/inObservations = 1
# /Hamming/ham/natural/inSamples = 512
# /Hamming/ham/natural/onObservations = 1
# /Hamming/ham/natural/onSamples = 512
# /Hamming/ham/real/israte = 22050
# /Hamming/ham/real/osrate = 22050

# Number of links = 0

# MarSystem
# Type = Spectrum
# Name = spectrum

# MarControls = 8
# /Spectrum/spectrum/bool/debug = 0
# /Spectrum/spectrum/bool/mute = 0
# /Spectrum/spectrum/natural/inObservations = 1
# /Spectrum/spectrum/natural/inSamples = 512
# /Spectrum/spectrum/natural/onObservations = 512
# /Spectrum/spectrum/natural/onSamples = 1
# /Spectrum/spectrum/real/israte = 22050
# /Spectrum/spectrum/real/osrate = 43.0664

# Number of links = 0

# MarSystem
# Type = PowerSpectrum
# Name = psk

# MarControls = 9
# /PowerSpectrum/psk/bool/debug = 0
# /PowerSpectrum/psk/bool/mute = 0
# /PowerSpectrum/psk/natural/inObservations = 512
# /PowerSpectrum/psk/natural/inSamples = 1
# /PowerSpectrum/psk/natural/onObservations = 256
# /PowerSpectrum/psk/natural/onSamples = 1
# /PowerSpectrum/psk/real/israte = 43.0664
# /PowerSpectrum/psk/real/osrate = 43.0664
# /PowerSpectrum/psk/string/spectrumType = power

# Number of links = 0

# MarSystem
# Type = Centroid
# Name = cntrd

# MarControls = 8
# /Centroid/cntrd/bool/debug = 0
# /Centroid/cntrd/bool/mute = 0
# /Centroid/cntrd/natural/inObservations = 256
# /Centroid/cntrd/natural/inSamples = 1
# /Centroid/cntrd/natural/onObservations = 1
# /Centroid/cntrd/natural/onSamples = 1
# /Centroid/cntrd/real/israte = 43.0664
# /Centroid/cntrd/real/osrate = 43.0664

# Number of links = 0

# MarSystem
# Type = NetworkTCPSink
# Name = dest

# MarControls = 12
# /NetworkTCPSink/dest/bool/debug = 0
# /NetworkTCPSink/dest/bool/mute = 0
# /NetworkTCPSink/dest/natural/controlsPort = 5010
# /NetworkTCPSink/dest/natural/dataPort = 5009
# /NetworkTCPSink/dest/natural/inObservations = 1
# /NetworkTCPSink/dest/natural/inSamples = 1
# /NetworkTCPSink/dest/natural/onObservations = 1
# /NetworkTCPSink/dest/natural/onSamples = 1
# /NetworkTCPSink/dest/real/gain = 1
# /NetworkTCPSink/dest/real/israte = 43.0664
# /NetworkTCPSink/dest/real/osrate = 43.0664
# /NetworkTCPSink/dest/string/host = cyprus.cs.uvic.ca

# Number of links = 0

